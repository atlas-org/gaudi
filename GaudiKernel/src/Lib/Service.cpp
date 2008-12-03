// $Id: Service.cpp,v 1.35 2008/10/23 15:57:37 marcocle Exp $

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
  else if ( IStateful       ::interfaceID() . versionMatch ( riid ) )
  { *ppvi = static_cast<IStateful*>       ( this ) ; }
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
    m_targetState = Gaudi::StateMachine::INITIALIZED;
    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorInitialize) ? auditorSvc() : 0,
                                      IAuditor::Initialize);
    sc = initialize(); // This should change the state to Gaudi::StateMachine::CONFIGURED
    if (sc.isSuccess())
      m_state = m_targetState;
    return sc;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysInitialize(): exception with tag=" << Exception.tag()
        << " is caught" << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //	  Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysInitialize(): standard std::exception is caught" << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //	  Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysInitialize(): UNKNOWN Exception is caught" << endreq;
    //	  Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }

  return StatusCode::FAILURE;
}


//--- IService::initialize
StatusCode Service::initialize() {
  MsgStream log(msgSvc(),name());

  // Set the Algorithm's properties
  StatusCode sc = setProperties();

  log << MSG::DEBUG <<  "Service base class initialized successfully" << endmsg;
  m_state = Gaudi::StateMachine::ChangeState(Gaudi::StateMachine::CONFIGURE,m_state);
  return sc ;
}

// IService::sysStart
StatusCode Service::sysStart() {
  StatusCode sc;

  try {
    m_targetState = Gaudi::StateMachine::ChangeState(Gaudi::StateMachine::START,m_state);
    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorStart) ? auditorSvc() : 0,
                                      IAuditor::Start);
    sc = start();
    if (sc.isSuccess())
      m_state = m_targetState;
    return sc;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysStart(): exception with tag=" << Exception.tag()
        << " is caught" << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //    Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysStart(): standard std::exception is caught" << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //    Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysStart(): UNKNOWN Exception is caught" << endreq;
    //    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }

  return StatusCode::FAILURE;
}

// IService::sysStop
StatusCode Service::sysStop() {
  StatusCode sc;

  try {
    m_targetState = Gaudi::StateMachine::ChangeState(Gaudi::StateMachine::STOP,m_state);
    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorStop) ? auditorSvc() : 0,
                                      IAuditor::Stop);
    sc = stop();
    if (sc.isSuccess())
      m_state = m_targetState;
    return sc;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysStop(): exception with tag=" << Exception.tag()
        << " is caught" << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //    Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysStop(): standard std::exception is caught" << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //    Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "in sysStop(): UNKNOWN Exception is caught" << endreq;
    //    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }

  return StatusCode::FAILURE;
}


//--- IService::stop
StatusCode Service::stop() {
  // stub implementation
  return StatusCode::SUCCESS;
}

//--- IService::start
StatusCode Service::start() {
  // stub implementation
  return StatusCode::SUCCESS;
}

//--- IService::sysFinalize
StatusCode Service::sysFinalize() {

  StatusCode sc(StatusCode::FAILURE);

  try {
    m_targetState = Gaudi::StateMachine::OFFLINE;
    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorFinalize) ? auditorSvc() : 0,
                                      IAuditor::Finalize);
    sc = finalize();
    if (sc.isSuccess())
      m_state = m_targetState;
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
  //MsgStream log(msgSvc(),name());
  //m_state = Gaudi::StateMachine::ChangeState(Gaudi::StateMachine::TERMINATE,m_state);
  return StatusCode::SUCCESS;
}

//--- IService::sysReinitialize
StatusCode Service::sysReinitialize() {

  StatusCode sc;

  // Check that the current status is the correct one.
  if ( Gaudi::StateMachine::INITIALIZED != FSMState() ) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR
        << "sysReinitialize(): cannot reinitialize service not initialized"
        << endreq;
    return StatusCode::FAILURE;
  }

  try {

    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorReinitialize) ? auditorSvc() : 0,
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

//--- IService::sysRestart
StatusCode Service::sysRestart() {

  StatusCode sc;

  // Check that the current status is the correct one.
  if ( Gaudi::StateMachine::RUNNING != FSMState() ) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR
        << "sysRestart(): cannot restart service in state " << FSMState()
        << " -- must be RUNNING "
        << endreq;
    return StatusCode::FAILURE;
  }

  try {

    Gaudi::Guards::AuditorGuard guard(this,
                                      // check if we want to audit the initialize
                                      (m_auditorRestart) ? auditorSvc() : 0,
                                      IAuditor::ReStart);
    sc = restart();
    return sc;
  }
  catch( const GaudiException& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysRestart()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    //    Stat stat( chronoSvc() , Exception.tag() ) ;
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysRestart()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    //    Stat stat( chronoSvc() , "*std::exception*" ) ;
  }
  catch( ... ) {
    MsgStream log ( msgSvc() , name() + ".sysRestart()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    //    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }
  return StatusCode::FAILURE ;

}

//--- IService::reinitialize
StatusCode Service::reinitialize() {
  /* @TODO
   * MCl 2008-10-23: the implementation of reinitialize as finalize+initialize
   *                 is causing too many problems
   *
  // Default implementation is finalize+initialize
  StatusCode sc = finalize();
  if (sc.isFailure()) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR << "reinitialize(): cannot be finalized" << endreq;
    return sc;
  }
  sc = initialize();
  if (sc.isFailure()) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR << "reinitialize(): cannot be initialized" << endreq;
    return sc;
  }
  */
  return StatusCode::SUCCESS;
}

//--- IService::restart
StatusCode Service::restart() {
  // Default implementation is stop+start
  StatusCode sc = stop();
  if (sc.isFailure()) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR << "restart(): cannot be stopped" << endreq;
    return sc;
  }
  sc = start();
  if (sc.isFailure()) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::ERROR << "restart(): cannot be started" << endreq;
    return sc;
  }
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
  m_state      = Gaudi::StateMachine::OFFLINE;
  m_targetState = Gaudi::StateMachine::OFFLINE;
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

  declareProperty( "AuditInitialize"   , m_auditorInitialize   = audit );
  declareProperty( "AuditStart"        , m_auditorStart        = audit );
  declareProperty( "AuditStop"         , m_auditorStop         = audit );
  declareProperty( "AuditFinalize"     , m_auditorFinalize     = audit );
  declareProperty( "AuditReInitialize" , m_auditorReinitialize = audit );
  declareProperty( "AuditReStart"      , m_auditorRestart      = audit );

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
