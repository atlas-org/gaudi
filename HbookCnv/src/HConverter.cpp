// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HConverter.cpp,v 1.9 2005/10/03 15:42:55 hmd Exp $
#define HBOOKCNV_HCONVERTER_CPP


// Include files
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "HConverter.h"
#include "HbookDef.h"

// Maximum length of RZ directory names
static const long MAX_RZ_DIR_LENGTH = 16;

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::HConverter
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

// Create RZ sub directories if they do not exist according to the store location
StatusCode HbookCnv::HConverter::createSubDirs(const std::string& top, const std::string& loc)    {
  MsgStream log(msgSvc(), "HConverter");
  std::string loc2 = top;
  ::HCDIR(loc2," ");
  long ll = loc.find("/", 1);
  ll += (ll==1) ? 2 : 1;

 Again:
  if ( ll > 0 )   {
    long idh = 0;
    std::string nloc = loc.substr(ll, loc.length());
    while( 1 )    {
      std::string type, title = "", opt = "D";
      ::HLNEXT(idh, type, title, opt);
      long ind = nloc.find("/", 1);
      long indSlash = ind;
      bool badName  = false;
      long nlocLen  = nloc.length();
      if ( ind > MAX_RZ_DIR_LENGTH )    {
        ind = MAX_RZ_DIR_LENGTH;
        badName = true;
      }
      else if ( ind < 0 && nlocLen > MAX_RZ_DIR_LENGTH )    {
        ind += (ind>0) ? MAX_RZ_DIR_LENGTH : MAX_RZ_DIR_LENGTH+1;
        badName = true;
      }

      std::string dd = nloc.substr(0, (ind>0) ? ind : nlocLen);
      long ddLen = dd.length();
      if ( idh == 0 || title.length() == 0 )   {
        loc2 += '/';
        // loc2 += (indSlash < 0) ? nloc : dd;
        loc2 += dd;
        if ( (nlocLen <= MAX_RZ_DIR_LENGTH && ind < 0) ||
             (ddLen   <  MAX_RZ_DIR_LENGTH && ind > 0) ||
             badName )
        {
          ::HMDIR(loc2,"S");
          if ( badName )   {
            log << MSG::WARNING << "Truncated directory name to "
                << loc2 << "> (Name too long)" << endreq;
          }
          else  {
            log << MSG::VERBOSE << "Create dir <" << loc2 << ">" << endreq;
          }
          if ( indSlash >= 0 )   {
            ll += indSlash + 1;
            goto Again;
          }
        }
        else  {
          log << MSG::ERROR << "Cannot create dir <" << loc2 << "> (unknown reason)" << endreq;
          return StatusCode::FAILURE;
        }
        break;
      }
      else if ( type[0] == 'D' && ::strncasecmp(dd.c_str(),title.c_str(),MAX_RZ_DIR_LENGTH)==0 )  {
        loc2 += '/';
        loc2 += (indSlash < 0) ? nloc : dd;
        log << MSG::VERBOSE << "Set dir <" << loc2 << ">" << endreq;
        ::HCDIR(loc2," ");
        if ( indSlash >= 0 )   {
          ll += indSlash + 1;
          goto Again;
        }
        break;
      }
      else if ( type[0] == 0 )      {
        goto Again;
      }
    }
    ::HCDIR(loc2, " ");
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}


/// Create RZ directory if not present
StatusCode HbookCnv::HConverter::createDirectory(const std::string& loc)    {
  StatusCode status=StatusCode::SUCCESS;
  long ll = loc.find("/", 1);
  std::string disk = "";
  long ll2 = loc.find("/", 1+((ll=1) ? 2 : 1));
  if ( ll2 >= 0 )    {
    long ll3 = loc.find("/", ll2+1);
    disk = "/" + loc.substr(ll2, (ll3 > 0 ? ll3 : loc.length())-ll2);
    std::string sub = loc.substr(ll2, loc.length());
    status=createSubDirs( "//PAWC", loc );
    if ( status.isSuccess() )   {
      status=createSubDirs( disk, sub );
    }
  }
  return status;
}


/// Retrieve name of memory directory from location
std::string HbookCnv::HConverter::diskDirectory(const std::string& loc)   {
  long ll = loc.find("/", 1);
  ll = loc.find("/", (ll=1) ? loc.find("/", 2) : ll);
  return "/" + loc.substr(ll, loc.length());
}


/// Retrieve name of memory directory from location
std::string HbookCnv::HConverter::directory(const std::string& loc)   {
  long ll = loc.find("/", 1);
  std::string pawc = "//PAWC";
  if ( ll == 1 ) ll = loc.find("/", 2);
  if ( ll > 0 )   {
    pawc += loc.substr(ll>0 ? ll : 0, loc.length());
  }
  return pawc;
}


// Set RZ sub directories if they do not exist according to the store location
void HbookCnv::HConverter::setDirectory(const std::string& loc)    {
  ::HCDIR(directory(loc), " ");
}


// Set RZ sub directories if they do not exist according to the store location
void HbookCnv::HConverter::setDiskDirectory(const std::string& loc)    {
  ::HCDIR(diskDirectory(loc), " ");
}

/// Create address of the transient object according to the requested representation.
StatusCode HbookCnv::HConverter::createAddress(const std::string& rzdir,
                                               const CLID& clid,
                                               const std::string& title,
                                               IOpaqueAddress*& refpAddress)   {
  GenericAddress* pA = new GenericAddress(repSvcType(),
                                          clid,
                                          rzdir,
                                          title,
                                          0,
                                          0);
  refpAddress = pA;
  return StatusCode::SUCCESS;
}


/// Create address of the transient object according to the requested representation.
StatusCode HbookCnv::HConverter::createAddress(const std::string& rzdir,
                                               const CLID& clid,
                                               long id,
                                               IOpaqueAddress*& refpAddress)   {
  char obj[32];
  StatusCode status = createAddress(rzdir, clid, ::_itoa(id, obj, 10), refpAddress);
  if ( status.isSuccess() )   {
    unsigned long* ipar = (unsigned long*)refpAddress->ipar();
    ipar[0] = id;
  }
  return status;
}


/// Create the transient representation of an object.
StatusCode HbookCnv::HConverter::readObject(const std::string& loc, long id)   {
  long icycle = 999999, ioff = 0;
  long l = loc.rfind('/');
  if ( l > 0 )    {
    std::string dir = loc.substr(0,l);
    setDiskDirectory(dir);
    setDirectory(dir);
    ::HRIN(id, icycle, ioff);
    if ( ::HEXIST( id ) )    {
      //::HLDIR(directory(dir)," ");
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

/// Update the converted representation of a transient object.
StatusCode HbookCnv::HConverter::writeObject(const std::string& loc, long id)   {
  setDirectory( loc );
  setDiskDirectory( loc );
  if ( ::HEXIST( id ) )    {
    MsgStream log(msgSvc(), "HConverter");
    int icycle;
    ::HROUT( id , &icycle, " ", 1);
    log << MSG::INFO;
    if ( log.isActive() )       {
      ::HPRINT( id );
    }
    if ( m_deleteAfterSave )    {
      ::HDELET( id );
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

/// Convert the transient object to the requested persistent representation.
StatusCode HbookCnv::HConverter::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress)  {
  refpAddress = 0;
  typedef std::pair<std::string,long> hID;
  typedef std::map< hID, bool > hIDMap;
  static hIDMap usedIDs;
  if ( 0 != pObject )   {
    IRegistry* pDir  = pObject->registry();
    if ( 0 != pDir )    {
      StatusCode status = StatusCode::SUCCESS;
      refpAddress = pDir->address();
      if ( 0 == refpAddress )   {
        const std::string & loc   = pDir->name();
        const std::string full  = m_prefix + pDir->identifier();
        std::string rzLoc = full.substr(0, full.rfind('/'));
        long histoID = atol(loc.c_str()+1); // Histogram ID
        // test if the hID is invalid (non-numeric) or already assigned
        if ( 0 >= histoID || usedIDs.find(hID(rzLoc,histoID)) != usedIDs.end() ) 
        {
          MsgStream log(msgSvc(), "HbookCnv");
          if ( 0 >= histoID )
          { 
            // Numeric representation of the histogram ID not valid
            log << MSG::WARNING << "Histogram ID '" << pDir->identifier() 
                << "' is an invalid HBOOK identifier" << endreq;
          }
          else
          {
            // ID is already used 
            log << MSG::WARNING << "Histogram ID '" << pDir->identifier() 
                << "' is already used" << endreq;
          }
          histoID = 1001;
          while ( usedIDs.find(hID(rzLoc,histoID)) != usedIDs.end() ) { ++histoID; }
          const std::string root = pDir->identifier().substr(0, pDir->identifier().rfind('/'));
          log << MSG::WARNING 
              << "  -> Adjusting PERSISTENT ID to '" << root << "/" << histoID << "'" << endreq;
        }
        usedIDs[hID(rzLoc,histoID)] = true;
        status = createAddress(rzLoc, objType(), histoID, refpAddress);
        if ( status.isSuccess() )   {
          setDiskDirectory(rzLoc);
          setDirectory(rzLoc);
          status = book(refpAddress, pObject);
        }
      }
      if ( status.isSuccess() )   {
        const std::string& rzLoc = refpAddress->par()[0];
        setDiskDirectory(rzLoc);
        setDirectory(rzLoc);
        status = updateRep(refpAddress, pObject);
        if ( status.isSuccess() )   {
          status = writeObject(rzLoc, refpAddress->ipar()[0]);
          if ( status.isSuccess() )   {
            return status;
          }
        }
        delete refpAddress;
        refpAddress = 0;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Inquire storage type
long HbookCnv::HConverter::storageType()      {
  return HBOOK_StorageType;
}

/// Standard constructor
HbookCnv::HConverter::HConverter( const CLID& clid, ISvcLocator* svc )
  : Converter(HbookCnv::HConverter::storageType(), clid, svc)  
{
  m_deleteAfterSave = false;
}


/// Standard destructor
HbookCnv::HConverter::~HConverter()                          {
}
