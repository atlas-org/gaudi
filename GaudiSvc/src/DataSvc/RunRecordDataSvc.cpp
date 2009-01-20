//====================================================================
//	RunRecordDataSvc.cpp
//--------------------------------------------------------------------
//
//	Package    : System ( The LHCb Offline System)
//
//  Description: implementation of the Transient event data service.
//
//  Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 10/12/08| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#define  DATASVC_RUNRECORDDATASVC_CPP

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/RegistryEntry.h"

#include "RunRecordDataSvc.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(RunRecordDataSvc)

/// Service initialisation
StatusCode RunRecordDataSvc::initialize()    {
  // Nothing to do: just call base class initialisation
  StatusCode      sc  = DataSvc::initialize();
  ISvcLocator*    svc_loc = serviceLocator();
  MsgStream log(msgSvc(),name());

  if ( !sc.isSuccess() ) { // Base class failure
    return sc;
  }
  // Attach data loader facility
  sc = svc_loc->service("PersistencySvc/RunRecordPersistencySvc", m_cnvSvc, true);
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Failed to access RunRecordPersistencySvc." << endmsg;
    return sc;
  }
  SmartIF<IProperty> prp(m_cnvSvc);
  if ( prp ) {
    //prp->setProperty();
  }
  sc = setDataLoader( m_cnvSvc );
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Failed to attach dataloader RunRecordPersistencySvc." << endmsg;
    return sc;
  }

  sc = setRoot(m_rootName, new DataObject());
  if( !sc.isSuccess() )  {
    log << MSG::WARNING << "Error declaring RunRecord root DataObject" << endreq;
    return sc;
  } 

  if( !m_incidentSvc )  {
    log << MSG::FATAL << "IncidentSvc is invalid--base class failed." << endreq;
    return sc;
  }

  /// We need to subscribe to 2 incident:
  /// 1) FILE_OPEN_READ:            fired by conversion service on open file
  /// 2) IncidentType::BeginEvent   fired by event loop BEFORE the event processing starts.
  ///                               Do everything to bootstract access to the old event record.
  m_incidentSvc->addListener(this,IncidentType::BeginEvent);
  m_incidentSvc->addListener(this,"FILE_OPEN_READ");
  m_incidentSvc->addListener(this,"SAVE_RUN_RECORD");

  return sc;
}

/// Service reinitialisation
StatusCode RunRecordDataSvc::reinitialize()    {
  // Do nothing for this service
  return StatusCode::SUCCESS;
}

/// Service finalization
StatusCode RunRecordDataSvc::finalize()    {
  if( m_cnvSvc ) m_cnvSvc->release();
  m_cnvSvc = 0;
  DataSvc::finalize().ignore();
  return StatusCode::SUCCESS ;
}

/// Query interfaces of Interface
StatusCode RunRecordDataSvc::queryInterface(const InterfaceID& riid,void** ppvIf)  { 
  if ( 0 == ppvIf )
    return StatusCode::FAILURE;
  else if ( IIncidentListener::interfaceID().versionMatch(riid) )
    *ppvIf = static_cast<IIncidentListener*>( this );
  else
    return DataSvc::queryInterface(riid,ppvIf);
  // increment reference counter 
  addRef();
  return SUCCESS;
}

/// Inform that a new incident has occured
void RunRecordDataSvc::handle(const Incident& incident) {
  if ( incident.type() == "FILE_OPEN_READ" ) {
    typedef ContextIncident<IOpaqueAddress*> Ctxt;
    const Ctxt* inc = dynamic_cast<const Ctxt*>(&incident);
    if ( inc ) {
      registerRunRecord(inc->source(),inc->tag());
      return;
    }
    MsgStream log(msgSvc(),name());
    log << MSG::ALWAYS << "Received invalid incident of type:" << incident.type() << endmsg;        
  }
  else if ( incident.type() == "SAVE_RUN_RECORD" ) {
    MsgStream log(msgSvc(),name());
    log << MSG::ALWAYS << "Saving run records not implemented." << endmsg;    
  }
}

/// Load new run record into the data store if necessary
void RunRecordDataSvc::registerRunRecord(const std::string& data, IOpaqueAddress* pAddr)   {
  if ( !data.empty() && 0 != pAddr ) {
    MsgStream log(msgSvc(),name());
    std::string fid = data;
    log << MSG::ALWAYS << "Request to load run record for file " << fid << endmsg;
    StatusCode sc = registerAddress(m_root,fid,pAddr);
    if ( !sc.isSuccess() ) {
      log << MSG::ERROR << "Failed to register run record for:" << fid << endmsg;
      pAddr->release();
    }
  }
  else if ( !data.empty() && 0 == pAddr ) {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed to register run record for:" << data << " [Invalid Address]" << endmsg;
  }
}

/// Standard Constructor
RunRecordDataSvc::RunRecordDataSvc(const std::string& name,ISvcLocator* svc) 
: DataSvc(name,svc)   {
  m_cnvSvc = 0;
  m_rootName = "/RunRecords";
}

/// Standard Destructor
RunRecordDataSvc::~RunRecordDataSvc()  {
}
