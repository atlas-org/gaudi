// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HFileCnv.cpp,v 1.10 2006/01/10 20:11:14 hmd Exp $
#define HBOOKCNV_HFILECNV_CPP


// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "HbookDef.h"
#include "HFileCnv.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::HFileCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------


// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_CONVERTER_FACTORY(HbookCnv,HFileCnv)

StatusCode HbookCnv::HFileCnv::initialize() 
{ 
  StatusCode sc = HbookCnv::HDirectoryCnv::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  // Get Tool Service 
  IToolSvc* tSvc = 0  ;
  MsgStream log ( msgSvc() , "HbookNFileCnv" ) ;
  ServiceLocatorHelper hlp( *serviceLocator() , log , "HbookNFileCnv" ) ;
  sc = hlp.getService( "ToolSvc", 
                       true     , 
                       IToolSvc::interfaceID() , 
                       (void**)&tSvc ) ;
  if ( sc.isSuccess() &&  0 != tSvc ) 
  {
    IAlgTool* tool = 0 ;
    sc = tSvc->retrieve( "HbookTool" , IAlgTool::interfaceID() , tool ) ;
    if ( sc.isSuccess() && 0 != tool ) 
    {
      IntegerProperty lrecl ( "LRECL" , m_LRECL ) ;
      IntegerProperty nrecl ( "NRECL" , m_NRECL ) ;
      StringProperty  mode  ( "MODE"  , m_MODE  ) ;
      IProperty* prop = 0 ;
      sc = tool->queryInterface( IProperty::interfaceID() , 
                                 (void**)&prop ) ;
      if ( sc.isSuccess() && 0 != prop ) 
      {
        if ( prop->getProperty ( &lrecl ).isSuccess() )
        { m_LRECL = lrecl.value() ; }
        if ( prop->getProperty ( &nrecl ).isSuccess() )
        { m_NRECL = nrecl.value() ; }
        if ( prop->getProperty ( &mode  ).isSuccess() ) 
        { m_MODE  = mode.value() ; }
        prop->release() ; 
        prop = 0 ;
      }
      tSvc->releaseTool( tool ) ;
      tool = 0 ;
    }
    tSvc->release() ; 
  };                     

  return StatusCode::SUCCESS ;
};

StatusCode HbookCnv::HFileCnv::createObj(IOpaqueAddress* pAddress, DataObject*& refpObject)   {
  MsgStream log(msgSvc(), "HbookNFileCnv");
  unsigned long* ipar = (unsigned long*)pAddress->ipar();
  std::string fname = pAddress->par()[0];
  std::string oname = pAddress->registry()->name().c_str()+1;
  // V.B.:  char mode[3] = { char(ipar[1]), 'P', 0 };
  // V.B.: long istat, lrec=1024, lun = ::F77GETLUN();
  std::string mode ( 1 , char(ipar[1]) ) ;
  mode += m_MODE ;
  long istat, lrec=m_LRECL, lun = ::F77GETLUN();
  // Do the init
  ::INIT_HBOOK(250000);
  // Do not forget to remove the first "/"
  if ( mode[0] == 'O' ) mode[0] = ' ';  
  if ( std::string::npos != mode.find('Q')  && 0 < m_NRECL ) 
  { ::SETQUEST( 10 , m_NRECL ) ; }
  ::HROPEN(lun, oname, fname, mode, lrec, istat);
  
  if ( istat != 0 )   {
    log << MSG::ERROR << "Error opening HBook RZ file:" << fname 
  << " as " << oname << endreq;
    return StatusCode::FAILURE;
  }
  else if ( mode[0] == 'N' )   {
    log << MSG::INFO << "Opened NEW HBook RZ file:" << fname 
  << " as " << oname << endreq;
  }
  else if ( mode[0] == 'U' )   {
    log << MSG::INFO << "Updating EXISTING HBook RZ file:" << fname
  << " as " << oname << endreq;
  }
  else   {
    log << MSG::INFO << "Opening EXISTING HBook RZ file:" << fname
  << " as " << oname << endreq;
  }
  NTuple::File* pFile = new NTuple::File((unsigned char)objType(), fname, oname);
  ipar[0] = lun;
  pFile->setOpen(true);
  refpObject = pFile;
  return StatusCode::SUCCESS;
}


/// Convert the transient object to the requested representation.
StatusCode HbookCnv::HFileCnv::createRep( DataObject* pObject, IOpaqueAddress*& refpAddress )   {
  IRegistry* pRegistry = pObject->registry();
  if ( 0 != pRegistry )   {
    refpAddress = pRegistry->address();
    return HFileCnv::updateRep( refpAddress, pObject );
  }
  return StatusCode::FAILURE;
}


/// Convert the transient object to the requested representation.
StatusCode HbookCnv::HFileCnv::updateRep( IOpaqueAddress* pAddress, DataObject* pObject )   {
  NTuple::File* pFile = dynamic_cast<NTuple::File*>(pObject);
  if ( pFile->isOpen() )    {
    MsgStream log(msgSvc(), "HbookNFileCnv");
    std::string oname = pFile->logicalName();
    long lun = pAddress->ipar()[0];
    ::HREND(oname.c_str(), oname.length());
    F77CLOSE(lun);
    log << MSG::INFO << "Closing HBook RZ file:" << pFile->name() 
                   << " as " << pObject->registry()->identifier() << endreq;
    pFile->setOpen(false);
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}
