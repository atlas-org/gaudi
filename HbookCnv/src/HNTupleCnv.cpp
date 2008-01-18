// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HNTupleCnv.cpp,v 1.9 2006/03/13 17:17:32 hmd Exp $
#define HBOOKCNV_HNTUPLECNV_CPP


// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "NTupleInfo.h"
#include "HNTupleCnv.h"
#include "HbookDef.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::HNTupleCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

/// Standard constructor
HbookCnv::HNTupleCnv::HNTupleCnv(ISvcLocator* svc, const CLID& clid)
: HConverter(clid, svc), m_ntupleSvc(0)         {
}

/// Standard destructor
HbookCnv::HNTupleCnv::~HNTupleCnv()             {
}

/// Initialize the converter
StatusCode HbookCnv::HNTupleCnv::initialize()   {
  StatusCode status = Converter::initialize();
  if ( status.isSuccess() )   {
    status = dataProvider()->queryInterface(IID_INTupleSvc, (void**)&m_ntupleSvc);
  }
  return status;
}

/// Finalize the converter
StatusCode HbookCnv::HNTupleCnv::finalize()     {
  if ( m_ntupleSvc != 0 )     {
    m_ntupleSvc->release();
  }
  m_ntupleSvc = 0;
  return Converter::finalize();
}

/// Update the transient object from the other representation.
StatusCode HbookCnv::HNTupleCnv::updateObj(IOpaqueAddress* pAddress, DataObject* pObject)   {
  StatusCode status = StatusCode::FAILURE;
  try   {
    unsigned long* info = (unsigned long*)pAddress->ipar();
    const std::string& dir = pAddress->par()[0];
    setDiskDirectory(dir);
    setDirectory(dir);
    status = readData(info[0], dynamic_cast<INTuple*>(pObject), info[1]++);
  }
  catch (...)   {
  }
  return status;
}

/// Create the transient representation of an object.
StatusCode HbookCnv::HNTupleCnv::createObj(IOpaqueAddress* pAddress, DataObject*& refpObject)   {
  long id = pAddress->ipar()[0];
  IRegistry* pReg = pAddress->registry();
  StatusCode status = readObject( pReg->identifier(), id );
  if ( status.isSuccess() )   {
    MsgStream log(msgSvc(), "HbookNTupleCnv");
    INTuple* nt = 0;
    log << MSG::DEBUG;
    if ( log.isActive() ) ::HPRINT (id);
    status = load(id, nt);
    refpObject = dynamic_cast<DataObject*>(nt);
  }
  return status;
}

/// Convert the transient object to the requested representation.
StatusCode HbookCnv::HNTupleCnv::createRep(DataObject* pObject, IOpaqueAddress*& pAddr)  {
  pAddr = 0;
  try   {  
    typedef std::pair<std::string,long> hID;
    typedef std::map< hID, bool > hIDMap;
    static hIDMap usedIDs;
    IRegistry* pReg = pObject->registry();
    INTuple* pTuple = dynamic_cast<INTuple*>(pObject);
    // Get address again....it does not change
    pAddr = pReg->address();
    if ( 0 == pAddr )    {
      char* stop = 0;
      const std::string& regName = pReg->identifier();
      const std::string rzLoc = regName.substr(0, regName.rfind('/'));
      long id = ::strtol(pReg->name().c_str()+1, &stop, 10);
      if ( 0 >= id || usedIDs.find(hID(rzLoc,id)) != usedIDs.end() )
      {
        MsgStream msg(msgSvc(), "HNTupleCnv");
        if ( 0 >= id )
        {
          msg << MSG::WARNING
              << "Ntuple ID " << pReg->identifier()
              << "' is an invalid HBOOK identifier" << endreq;
        }
        else
        {
          // ID is already used 
          msg << MSG::WARNING << "Ntuple ID '" << pReg->identifier() 
              << "' is already used" << endreq;
        }
        id = 1001;
        while ( usedIDs.find(hID(rzLoc,id)) != usedIDs.end() ) { ++id; }
        const std::string root = pReg->identifier().substr(0,pReg->identifier().rfind('/'));
        msg << MSG::WARNING 
            << "  -> Adjusting PERSISTENT ID to '" << root << "/" << id << "'" << endreq;
      }
      usedIDs[hID(rzLoc,id)] = true;
      StatusCode status = createDirectory(rzLoc);
      if ( !status.isSuccess() )   {
        return status;
      }
      setDirectory(rzLoc);
      setDiskDirectory(rzLoc);
      status = book(id, diskDirectory(rzLoc), pTuple);
      if ( !status.isSuccess() )   {
        return status;
      }
      status = createAddress( rzLoc, objType(), id, pAddr );
      if ( !status.isSuccess() )   {
        return status;
      }
    }
    setDirectory(pAddr->par()[0]);
    return writeData(pAddr->ipar()[0], pTuple);
  }
  catch (...)   {
  }
  return StatusCode::FAILURE;
}

/// Update the converted representation of a transient object.
StatusCode HbookCnv::HNTupleCnv::updateRep(IOpaqueAddress* pAddress, DataObject* pObject )
{
  MsgStream log(msgSvc(), "HNTupleCnv");
  if ( 0 != pAddress )    {
    IRegistry* pDir = pAddress->registry();
    if ( 0 != pDir )    {
      const std::string& regName = pDir->identifier();
      const std::string loc = regName.substr(0, regName.rfind('/'));
      long id = pAddress->ipar()[0];
      log << MSG::INFO << "Saving:" << regName << " with ID:" << id << endreq;
      return writeObject( loc, id );
    }
  } 
  else {
    log << MSG::WARNING << "empty ntuple: " 
	<< pObject->registry()->identifier() << endreq;
    return ( createRep(pObject,pAddress) );
  }
  return StatusCode::FAILURE;
}

bool HbookCnv::parseName( std::string full,
                          std::string &blk,
			                    std::string &var)
{ 
  int sp;
  if ( (sp=full.find("/")) != -1 ) {
    blk = full.substr(0,sp);
    var = full.substr(sp+1,full.length());
    return true;
  }
  else {
    blk = "AUTO_BLK";
    var = full;
    return false;
  }
}



/// Make the compiler instantiate the code...
#define INSTANTIATE(TYP)  \
template INTupleItem* createNTupleItem<TYP>(NTUPLEINFO& tags, long i, INTuple* tuple, TYP minimum, TYP maximum, long& size);

namespace HbookCnv  {
  template<class TYP>
  INTupleItem* createNTupleItem (NTUPLEINFO& tags, long i, INTuple* tuple, TYP /* minimum */, TYP /* maximum */ , long& size)   {
    size = 0;
    INTupleItem* col = 0;

    std::string varname;
    std::string blkname = tags.block[i];

    if (blkname != "AUTO_BLK") {
      varname = blkname + "/" + tags.name[i];
    } else {
      varname = tags.name[i];
    }

    //if ( tags.isRangeWithin(i, minimum,maximum))
    {
      long ind = tags.getIndex(tags.index[i]);
      long dim1 = 1, dim2 = 1;
      TYP min, max, null;
      tags.getBounds(i, min, max, null); 
      switch( tags.dim[i] )   {
      case 0:            
        col = NTuple::_Item<TYP>::create (tuple, varname, typeid(TYP), min, max, null);
        break;            
      case 1:             
        dim1 = (ind>=0) ? tags.idist(ind) : tags.theDim[i][0];
	  col = NTuple::_Array<TYP>::create (tuple, varname,typeid(TYP),tags.index[i],dim1,min,max,null);
        break;           
      case 2:              
        dim1 = (ind>=0) ? tags.idist(ind) : tags.theDim[i][0];
        dim2 = (ind>=0) ? tags.theDim[i][0] : tags.theDim[i][1];
        col = NTuple::_Matrix<TYP>::create (tuple, varname,typeid(TYP),tags.index[i],dim1,dim2,min,max,null);
        break;
      default:
        break;
      }
      size += dim1 * dim2 * ((sizeof(TYP) < 4) ? 4 : sizeof(TYP)); 
    }
    return col;
  }

  INSTANTIATE(float)
  INSTANTIATE(double)
  INSTANTIATE(bool)
  INSTANTIATE(char)
  INSTANTIATE(int)
  INSTANTIATE(short)
  INSTANTIATE(long)
  INSTANTIATE(unsigned char)
  INSTANTIATE(unsigned int)
  INSTANTIATE(unsigned short)
  INSTANTIATE(unsigned long)
    //  INSTANTIATE(IOpaqueAddress*)

    INTupleItem* createNTupleItem (NTUPLEINFO& tags, long i, INTuple* tuple, IOpaqueAddress* /* minimum */, IOpaqueAddress* /* maximum */, long& size)   {
    size = 0;
    INTupleItem* col = 0;
    long ind = tags.getIndex(tags.index[i]);
    long dim1 = 1, dim2 = 1;

    std::string varname;
    std::string blkname = tags.block[i];

    if (blkname != "AUTO_BLK") {
      varname = blkname + "/" + tags.name[i];
    } else {
      varname = tags.name[i];
    }

    switch( tags.dim[i] )   {
    case 0:            
      col = NTuple::_Item<IOpaqueAddress*>::create (tuple, varname,typeid(IOpaqueAddress*), 0,(IOpaqueAddress*)0xffffffff,0);
      break;            
    case 1:             
      dim1 = (ind>=0) ? tags.idist(ind) : tags.theDim[i][0];
      col = NTuple::_Array<IOpaqueAddress*>::create (tuple, varname,typeid(IOpaqueAddress*), 
						     tags.index[i], dim1, 0,(IOpaqueAddress*)0xffffffff,0);
      break;           
    case 2:              
      dim1 = (ind>=0) ? tags.idist(ind) : tags.theDim[i][0];
      dim2 = (ind>=0) ? tags.theDim[i][0] : tags.theDim[i][1];
      col = NTuple::_Matrix<IOpaqueAddress*>::create (tuple, varname,typeid(IOpaqueAddress*), 
						      tags.index[i],dim1,dim2,0,(IOpaqueAddress*)0xffffffff,0);
      break;
    default:
      break;
    }
    size += dim1 * dim2 * ((sizeof(IOpaqueAddress*) < 4) ? 4 : sizeof(IOpaqueAddress*)); 
    return col;
  }
};
