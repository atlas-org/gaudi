// $Id: DLLClassManager.cpp,v 1.17 2007/12/12 16:03:19 marcocle Exp $

// Include files
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ModuleIncident.h"
#include "GaudiKernel/MsgStream.h"
#include "DLLClassManager.h"
#include "GaudiKernel/System.h"

#include <iostream>
#include <cassert>

// default creator
DLLClassManager::DLLClassManager( IInterface* iface ) {
  m_pOuter = iface;
  m_pOuter->queryInterface(IID_ISvcLocator, pp_cast<void>(&m_svclocator)).ignore();
  assert( 0 != m_svclocator );
  m_msgsvc     = 0;
//  m_algmanager = 0;
//  m_svcmanager = 0;
//  m_cnvmanager = 0;
//  m_objmanager = 0;
  m_refcount   = 1;
}

// virtual destructor
DLLClassManager::~DLLClassManager() {
  if( m_msgsvc ) m_msgsvc->release();
  //if( m_algmanager ) m_algmanager->release();
  //if( m_svcmanager ) m_svcmanager->release();
  //if( m_cnvmanager ) m_cnvmanager->release();
  //if( m_objmanager ) m_objmanager->release();
  if( m_svclocator ) m_svclocator->release();
}

// implementation of IClassManager::loadModule 
StatusCode DLLClassManager::loadModule( const std::string& module,
					bool fireIncident ) {
  // Access the message service if not yet done already
  if( m_msgsvc == 0 ) {
    m_svclocator->getService( "MessageSvc", IID_IMessageSvc, *pp_cast<IInterface>(&m_msgsvc) ).ignore();
  }
  MsgStream log(m_msgsvc, "DllClassManager");

  std::string mod = module=="" ? System::moduleNameFull() : module;
  if( module == "NONE" ) return StatusCode::SUCCESS;

  void* libHandle = 0;
  StatusCode status = StatusCode::FAILURE;
  try
  {
    status = System::loadDynamicLib( module, &libHandle);
  }
  catch ( const std::exception & excpt )
  {
    if ( m_msgsvc )
    {
      log << MSG::ERROR << "Exception whilst loading " << module << " : " << excpt.what() << endmsg;
    }
    status = StatusCode::FAILURE;
  }

  if( status.isFailure() ) {
    // DLL library not loaded
    log << MSG::ERROR << "Could not load module " << module << endmsg;
    log << MSG::ERROR << "System Error: " << System::getLastErrorString() << endmsg;
    return StatusCode::FAILURE; 
  }
  //FIXME this is a hack to avoid a very early call to moduleLoad from
  //FIXME AppMgr::i_startup
  if (fireIncident && module != "") {
    //now fire ModuleLoadedIncident
    IIncidentSvc* pIncidentSvc(0);
    const bool CREATEIF(true);
    StatusCode sc(m_svclocator->service("IncidentSvc", pIncidentSvc, CREATEIF));
    if( !sc.isSuccess() )  {
      log << MSG::FATAL << "Can not locate IncidentSvc" << endreq;
      throw GaudiException("Error retrieving IncidentSvc", 
			   "DLLClassManager::DLLClassManager", sc);
    }
    pIncidentSvc->fireIncident(ModuleLoadedIncident("DLLClassManager",
						     module) );
    pIncidentSvc->release();
  }

  return StatusCode::SUCCESS;
}
  
// implmentation of IInterface::addRef
unsigned long DLLClassManager::addRef() {
  m_refcount++;
	return m_refcount;
}
  
// implmentation of IInterface::release
unsigned long DLLClassManager::release() {
  unsigned long count = --m_refcount;
  if( count <= 0) {
    delete this;
  }
  return count;
}
  
// implementation of IInterface::queryInterface
StatusCode DLLClassManager::queryInterface(const InterfaceID& iid, void** pinterface) {
  if( iid == IID_IInterface ) {  
    *pinterface = (IInterface*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  else if ( iid == IID_IClassManager ) {
    *pinterface = (IClassManager*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  else {
    return m_pOuter->queryInterface(iid, pinterface);
  } 
  return StatusCode::SUCCESS;
}




