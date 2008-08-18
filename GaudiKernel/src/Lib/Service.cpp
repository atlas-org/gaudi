// $Id: Service.cpp,v 1.32 2008/04/04 15:19:35 marcocle Exp $

// Include Files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/IAuditorSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/Guards.h"

using std::string;

//--- IInterface::addRef
unsigned long Service::addRef()   {
  m_refCount++;
  return m_refCount;
}

//--- IInterface::release
unsigned long Service::release()   {
  unsigned long count = --m_refCount;
  if( count == 0) {
    if (m_svcManager!=0) {
      m_svcManager->removeService(this).ignore();
    }
    delete this;
  }
  return count;
}

//--- IInterface::queryInterface
StatusCode Service::queryInterface
( const InterfaceID& riid, 
  void**             ppvi )  
{
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; } // RETURN 
  //
  if      ( IService        ::interfaceID() . versionMatch ( riid ) )  
  { *ppvi = static_cast<IService*>        ( this ) ; }
  else if ( IProperty       ::interfaceID() . versionMatch ( riid ) )  
  { *ppvi = static_cast<IProperty*>       ( this ) ; }
  else if ( INamedInterface ::interfaceID() . versionMatch ( riid ) )   
  { *ppvi = static_cast<INamedInterface*> ( this ) ; }
  else if ( IInterface      ::interfaceID() . versionMatch ( riid ) )   
  { *ppvi = static_cast<IInterface*>      ( this ) ; }
  else { *ppvi = 0 ; return NO_INTERFACE ; } // RETURN 
  // increment reference counter 
  addRef();
  //
  return SUCCESS;
}

// IService::sysInitialize
StatusCode Service::sysInitialize() {
  StatusCode sc;

  try {
    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorInitialize) ? auditorSvc() : 0,
                                      IAuditor::Initialize);
    sc = initialize();
    if (sc.isSuccess())
      m_state = INITIALIZED;
    return sc;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //	  Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //	  Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    //	  Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }
  
  return StatusCode::FAILURE;
}


//--- IService::initialize
StatusCode Service::initialize() {
  MsgStream log(msgSvc(),name());
  // Initialize it only if was not done
  if( m_state != OFFLINE ) {
    log << MSG::WARNING <<  "Service already initialized" << endmsg;
    return StatusCode::FAILURE;
  }

  // Set the Algorithm's properties
  StatusCode sc = setProperties();
  m_state = CONFIGURED;
  
  log << MSG::DEBUG <<  "Service base class initialized successfully" << endmsg;
  return sc ;
}

//--- IService::sysFinalize
StatusCode Service::sysFinalize() {
  
  StatusCode sc(StatusCode::FAILURE);

  try {
    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorFinalize) ? auditorSvc() : 0,
                                      IAuditor::Finalize);
    sc = finalize();
  }
  catch( const GaudiException& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysFinalize()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //    Stat stat( chronoSvc() , Exception.tag() ) ;
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysFinalize()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //    Stat stat( chronoSvc() , "*std::exception*" ) ;
  }
  catch( ... ) {
    MsgStream log ( msgSvc() , name() + ".sysFinalize()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    //    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }

  if( m_messageSvc ) {
    m_messageSvc->release();
    m_messageSvc = 0;
  }
  if( m_pAuditorSvc ) {
    m_pAuditorSvc->release();
    m_pAuditorSvc = 0;
  }
  
  return sc ;
  
}

//--- IService::finalize
StatusCode Service::finalize() {
  MsgStream log(msgSvc(),name());
  if( m_state != INITIALIZED ) {
    log << MSG::WARNING << "Service already offline" << endmsg;
    return StatusCode::FAILURE;
  }
  m_state = OFFLINE;
  return StatusCode::SUCCESS;
}

//--- IService::sysReinitialize
StatusCode Service::sysReinitialize() {
  
  StatusCode sc;

  try {

    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorInitialize) ? auditorSvc() : 0,
                                      IAuditor::ReInitialize);
    sc = reinitialize();
    return sc;
  }
  catch( const GaudiException& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //    Stat stat( chronoSvc() , Exception.tag() ) ;
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //    Stat stat( chronoSvc() , "*std::exception*" ) ;
  }
  catch( ... ) {
    MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    //    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }
  return StatusCode::FAILURE ;
  
}

//--- IService::reinitialize 
StatusCode Service::reinitialize() {
  return StatusCode::SUCCESS;
}

//--- IService::getServiceName
const std::string& Service::name()   const {
  return m_name;
}

//--- IService::getSeriviceType
const InterfaceID& Service::type()  const {
  return IID_IService;
}

//--- Retrieve pointer to service locator
ISvcLocator* Service::serviceLocator() const {
  return m_svcLocator;
}

//--- Retrieve pointer to message service
IMessageSvc* Service::msgSvc()    {
  if ( 0 == m_messageSvc ) {
    StatusCode sc = serviceLocator()->service( "MessageSvc", m_messageSvc, true ); 
    if( sc.isFailure() ) {
      throw GaudiException("Service [MessageSvc] not found", name(), sc);
    }
  }
  return m_messageSvc;
}

IMessageSvc* Service::msgSvc()  const  {
  if ( 0 == m_messageSvc ) {
    StatusCode sc = serviceLocator()->service( "MessageSvc", m_messageSvc, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [MessageSvc] not found", name(), sc);
    }
  }
  return m_messageSvc;
}
// Obsoleted name, kept due to the backwards compatibility
IMessageSvc* Service::messageService()    {
  return msgSvc();
}
IMessageSvc* Service::messageService()  const  {
  return msgSvc();
}

// IProperty implementation
// Delegate to the Property manager
StatusCode Service::setProperty(const Property& p) {
  return m_propertyMgr->setProperty(p);
}

StatusCode Service::setProperty(const std::string& s) {
  return m_propertyMgr->setProperty(s);
}

StatusCode Service::setProperty(const std::string& n, const std::string& v) {
  return m_propertyMgr->setProperty(n,v);
}

StatusCode Service::getProperty(Property* p) const {
  return m_propertyMgr->getProperty(p);
}

const Property& Service::getProperty(const std::string& n) const {
  return m_propertyMgr->getProperty(n);
}

StatusCode Service::getProperty(const std::string& n, std::string& v ) const {
  return m_propertyMgr->getProperty(n,v);
}

const std::vector<Property*>& Service::getProperties() const {
  return m_propertyMgr->getProperties();
}

// Use the job options service to set declared properties
StatusCode Service::setProperties() {
  IJobOptionsSvc* jos;
  const bool CREATEIF(true);
  StatusCode sc = serviceLocator()->service( "JobOptionsSvc", jos, CREATEIF );
  if( sc.isFailure() ) {
    throw GaudiException("Service [JobOptionsSvc] not found", name(), sc);
  }
  // set first generic Properties
  sc = jos->setMyProperties( getGaudiThreadGenericName(name()), this );
  if( sc.isFailure() ) return StatusCode::FAILURE;
 
  // set specific Properties
  if (isGaudiThreaded(name())) {
    if (jos->setMyProperties( name(), this ).isFailure()) {
      return StatusCode::FAILURE;
    }
  }
  jos->release();
  return StatusCode::SUCCESS;
}


//--- Local methods
// Standard Constructor
Service::Service( const std::string& name, ISvcLocator* svcloc) {
  m_name       = name;
  m_refCount   = 0;
  m_svcLocator = svcloc;
  m_svcManager = 0;
  m_messageSvc = 0;
  m_state      = OFFLINE;
  m_propertyMgr = new PropertyMgr();
  m_pAuditorSvc = 0;
  m_outputLevel = MSG::NIL;
  // Declare common Service properties with their defaults
  if ( name != "MessageSvc" )  {
    m_outputLevel = msgSvc()->outputLevel();
  }
  declareProperty( "OutputLevel", m_outputLevel);
  m_outputLevel.declareUpdateHandler(&Service::initOutputLevel, this);

  // Get the default setting for service auditing from the AppMgr
  declareProperty( "AuditServices", m_auditInit=true );

  IProperty *appMgr;
  bool audit(false);
  if (m_svcLocator->service("ApplicationMgr", appMgr, false).isSuccess()) {
    const Property& prop = appMgr->getProperty("AuditServices");
    Property &pr = const_cast<Property&>(prop);
    if (m_name != "IncidentSvc") {
      setProperty( pr ).ignore();
      audit = m_auditInit.value();
    } else {
      audit = false;
    }
  }

  declareProperty( "AuditInitialize" , m_auditorInitialize = audit );
  declareProperty( "AuditFinalize"   , m_auditorFinalize   = audit );

}

// Callback to set output level
void Service::initOutputLevel(Property& /*prop*/) {
  if ( name() != "MessageSvc" ) {
    msgSvc()->setOutputLevel( name(), m_outputLevel );
  }
}

// Standard Destructor
Service::~Service() {
  delete m_propertyMgr;
}


StatusCode 
Service::service_i(const std::string& svcName, bool createIf, 
		   const InterfaceID& iid, void** ppSvc) const {
  MsgStream log(msgSvc(), name());
  ServiceLocatorHelper helper(*serviceLocator(), log, name());
  return helper.getService(svcName, createIf, iid, ppSvc);
}

StatusCode 
Service::service_i(const std::string& svcType,
		   const std::string& svcName,
		   const InterfaceID& iid,
		   void** ppSvc) const {

  MsgStream log(msgSvc(), name());
  ServiceLocatorHelper helper(*serviceLocator(), log, name());
  return  helper.createService(svcType, svcName, iid, ppSvc);
}

IAuditorSvc* Service::auditorSvc() const {
  if ( 0 == m_pAuditorSvc ) {
    StatusCode sc = service( "AuditorSvc", m_pAuditorSvc, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [AuditorSvc] not found", name(), sc);
    }
  }
  return m_pAuditorSvc;
}

void
Service::setServiceManager(ISvcManager *ism) {
  m_svcManager = ism;
}
