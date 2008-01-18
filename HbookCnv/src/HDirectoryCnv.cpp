// $Id: HDirectoryCnv.cpp,v 1.8 2006/01/10 20:11:14 hmd Exp $
#define HBOOKCNV_HDIRECTORYCNV_CPP


// Include files
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/NTuple.h"

#include <stdio.h>

#include "HbookDef.h"
#include "HDirectoryCnv.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::HDirectoryCnv
//
// Author :                   Pavel Binko
//
//------------------------------------------------------------------------------


// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_CONVERTER_FACTORY(HbookCnv,HDirectoryCnv)

HbookCnv::HDirectoryCnv::HDirectoryCnv(ISvcLocator* svc)
 : HConverter(CLID_StatisticsDirectory, svc)                 {
}


HbookCnv::HDirectoryCnv::HDirectoryCnv(ISvcLocator* svc, const CLID& clid)
 : HConverter(clid, svc)                 {
}


/// Standard Destructor
HbookCnv::HDirectoryCnv::~HDirectoryCnv()                                         {
}

/// Inquire class type
const CLID& HbookCnv::HDirectoryCnv::classID()      {
  return CLID_StatisticsDirectory;
}

/// Create the transient representation of an object.
StatusCode HbookCnv::HDirectoryCnv::createObj(IOpaqueAddress* /* pAddress */, DataObject*& refpObject)  {
  refpObject = new NTuple::Directory();
  return StatusCode::SUCCESS;
}


/// Convert the transient object to the requested representation.
StatusCode HbookCnv::HDirectoryCnv::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress)  {
  if ( pObject )    {
    IRegistry* pRegistry = pObject->registry();
    if ( pRegistry )   {
      std::string loc = m_prefix + pRegistry->identifier();
      if ( createDirectory(loc).isSuccess() )   {
        setDirectory(loc);
        setDiskDirectory(loc);
        return createAddress(loc, objType(), pRegistry->name(), refpAddress);
      }
    }
  }
  refpAddress = 0;
  return StatusCode::FAILURE;
}


/// Convert the transient object to the requested representation.
StatusCode HbookCnv::HDirectoryCnv::updateRep( IOpaqueAddress* /* pAddress */ , DataObject* pObject)  {
  if ( pObject )    {
    IRegistry* pRegistry = pObject->registry();
    if ( pRegistry )   {
      std::string loc = m_prefix + pRegistry->identifier();
      if ( createDirectory(loc).isSuccess() )   {
        setDirectory(loc);
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}


StatusCode HbookCnv::HDirectoryCnv::fillObjRefs(IOpaqueAddress* /* pAddress */,
                                                DataObject* pObject)   {
  IRegistry* pReg = pObject->registry();
  // Strip of store name to get the top level RZ directory
  std::string full = m_prefix + pReg->identifier();
  std::string lloc = full.substr(full.find('/',1),full.length());
  std::string loc  = "/" + lloc;
  std::string pawc = "//PAWC" + lloc;
  long idh = 0;
  ::HMDIR(pawc, "S");
  ::HCDIR(loc, " ");
  while( 1 )    {
    std::string type = " ", title = "";
    long icycle = 99999, ioff = 0;
    IOpaqueAddress* pA = 0;
    CLID clid = CLID_NULL;
    char leaf_name[32];
    ::HLNEXT(idh, type, title, "12ND");
    if ( idh == 0 || title.length() == 0 ) break;
    sprintf(leaf_name, "/%ld", idh);

    switch ( type[0] )    {
    case 'D':
      createAddress(full, CLID_StatisticsDirectory, title, pA);
      break;
    case '1':
      title = leaf_name;
      createAddress(full, CLID_H1D, idh, pA);
      break;
    case '2':   
      title = leaf_name;
      createAddress(full, CLID_H2D, idh, pA);
      break;
    case 'N':
      ::HRIN( idh, icycle, ioff);
      clid = (1 == ::NTUPLETYPE(idh)) ? CLID_ColumnWiseTuple : CLID_RowWiseTuple;
      createAddress(full, clid, idh, pA);
      title = leaf_name;
      break;
    default:
      break;
    }
    if ( 0 != pA )    {
      SmartIF<IDataManagerSvc> registrator(dataProvider());
      registrator->registerAddress(pReg, title, pA);
    }
  }
  return StatusCode::SUCCESS;
}
