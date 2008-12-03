// $Id: ApplicationMgr.cpp,v 1.77 2008/11/10 15:29:09 marcocle Exp $

// Include files
#include "ApplicationMgr.h"
#include "ServiceManager.h"
#include "AlgorithmManager.h"
#include "DLLClassManager.h"

#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ObjectFactory.h"

#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "GaudiKernel/StatusCode.h"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <limits>

DECLARE_OBJECT_FACTORY(ApplicationMgr)

static const char* s_eventloop = "EventLoop";
static const char* s_runable   = "Runable";


// Implementation class for the Application Manager. In this way the
// ApplicationMgr class is a fully insulated concrete class. Clients
// (main programs) will not need to re-compile if there are changes
// in the implementation

//=======================================================================
// Constructor
//=======================================================================
ApplicationMgr::ApplicationMgr(IInterface*) {
  // IInterface initialization
  m_refcount = 1;

  // Initialize two basic services: messagesvc & joboptions
  m_messageSvc        = 0;
  m_jobOptionsSvc     = 0;
  // Service Locator/Factory has to be instantiated FIRST!!!!!
  // as subsequent internal services need access to SvcLocator/Factory obj
  m_svcManager        = new ServiceManager(this);
  m_svcLocator        = m_svcManager; // alias to distinguish dual function

  // Instanciate internal services
  // SvcLocator/Factory HAS to be already instantiated
  m_algManager   = new AlgorithmManager(this);
  m_classManager = new DLLClassManager(this);
  m_propertyMgr  = new PropertyMgr(this);

  m_name  = "ApplicationMgr";
  m_state = Gaudi::StateMachine::OFFLINE;
  m_targetState = Gaudi::StateMachine::OFFLINE;

  m_propertyMgr->declareProperty("Go",            m_SIGo = 0 );
  m_propertyMgr->declareProperty("Exit",          m_SIExit = 0 );
  m_propertyMgr->declareProperty("Dlls",          m_dllNameList );
  m_propertyMgr->declareProperty("ExtSvc",        m_extSvcNameList );
  m_propertyMgr->declareProperty("CreateSvc",     m_createSvcNameList );
  m_propertyMgr->declareProperty("ExtSvcCreates", m_extSvcCreates=true );

  m_propertyMgr->declareProperty("SvcMapping",    m_svcMapping );
  m_propertyMgr->declareProperty("SvcOptMapping", m_svcOptMapping );

  m_propertyMgr->declareProperty("TopAlg",        m_topAlgNameList );
  m_propertyMgr->declareProperty("OutStream",     m_outStreamNameList );
  m_propertyMgr->declareProperty("OutStreamType", m_outStreamType = "OutputStream" );
  m_propertyMgr->declareProperty("MessageSvcType",m_messageSvcType= "MessageSvc" );
  m_propertyMgr->declareProperty("JobOptionsSvcType",
                                 m_jobOptionsSvcType = "JobOptionsSvc" );
  m_propertyMgr->declareProperty( s_runable,      m_runableType   = "AppMgrRunable");
  m_propertyMgr->declareProperty( s_eventloop,    m_eventLoopMgr  = "EventLoopMgr");

  m_propertyMgr->declareProperty("HistogramPersistency", m_histPersName="NONE");

  // Declare Job Options Service properties and set default
  m_propertyMgr->declareProperty("JobOptionsType", m_jobOptionsType = "FILE");
  m_propertyMgr->declareProperty("JobOptionsPath", m_jobOptionsPath = "");
  m_propertyMgr->declareProperty("EvtMax",         m_evtMax = -1);
  m_propertyMgr->declareProperty("EvtSel",         m_evtsel );
  m_propertyMgr->declareProperty("OutputLevel",    m_outputLevel = MSG::INFO);

  m_propertyMgr->declareProperty("MultiThreadExtSvc", m_multiThreadSvcNameList);
  m_propertyMgr->declareProperty("NoOfThreads",    m_noOfEvtThreads = 0);
  m_propertyMgr->declareProperty("AppName",        m_appName = "ApplicationMgr");
  m_propertyMgr->declareProperty("AppVersion",     m_appVersion = "");

  m_propertyMgr->declareProperty("AuditTools",      m_auditTools = false);
  m_propertyMgr->declareProperty("AuditServices",   m_auditSvcs = false);
  m_propertyMgr->declareProperty("AuditAlgorithms", m_auditAlgs = false);

  m_propertyMgr->declareProperty("ActivateHistory", m_actHistory = false);
  m_propertyMgr->declareProperty("StatusCodeCheck", m_codeCheck = false);

  m_propertyMgr->declareProperty("Environment",    m_environment);

  // ServiceMgr Initialization loop checking
  m_propertyMgr->declareProperty("InitializationLoopCheck", m_loopCheck = true)
    ->declareUpdateHandler(&ApplicationMgr::initLoopCheckHndlr, this);
  m_propertyMgr->declareProperty("LoopCheckIgnore", m_loopIgnore)
    ->declareUpdateHandler(&ApplicationMgr::initLoopCheckHndlr, this);
  m_svcManager->setLoopCheckEnabled(m_loopCheck);
  m_svcManager->setLoopCheckIgnore(m_loopIgnore);

  // Flag to activate the printout of properties
  m_propertyMgr->declareProperty
    ( "PropertiesPrint",
      m_propertiesPrint = false,
      "Flag to activate the printout of properties" );

  m_propertyMgr->declareProperty("ReflexPluginDebugLevel", m_reflexDebugLevel = 0 );

  // Add action handlers to the appropriate properties
  m_SIGo.declareUpdateHandler  ( &ApplicationMgr::SIGoHandler         , this );
  m_SIExit.declareUpdateHandler( &ApplicationMgr::SIExitHandler       , this );
  m_topAlgNameList.declareUpdateHandler(&ApplicationMgr::evtLoopPropertyHandler, this);
  m_outStreamNameList.declareUpdateHandler(&ApplicationMgr::evtLoopPropertyHandler, this);
  m_outStreamType.declareUpdateHandler(&ApplicationMgr::evtLoopPropertyHandler, this);
  m_reflexDebugLevel.declareUpdateHandler(&ApplicationMgr::reflexDebugPropertyHandler, this);
  m_processingMgr = 0;
  m_runable = 0;
  m_svcMapping.push_back("EvtDataSvc/EventDataSvc");
  m_svcMapping.push_back("DetDataSvc/DetectorDataSvc");
  m_svcMapping.push_back("HistogramSvc/HistogramDataSvc");
  m_svcMapping.push_back("HbookCnv::PersSvc/HbookHistSvc");
  m_svcMapping.push_back("RootHistCnv::PersSvc/RootHistSvc");
  m_svcMapping.push_back("EvtPersistencySvc/EventPersistencySvc");
  m_svcMapping.push_back("DetPersistencySvc/DetectorPersistencySvc");
  m_svcMapping.push_back("HistogramPersistencySvc/HistogramPersistencySvc");
}

//============================================================================
// destructor
//============================================================================
ApplicationMgr::~ApplicationMgr() {
  if( m_algManager ) m_algManager->release();
  if( m_classManager ) m_classManager->release();
  if( m_propertyMgr ) m_propertyMgr->release();
  if( m_messageSvc ) m_messageSvc->release();
  if( m_jobOptionsSvc ) m_jobOptionsSvc->release();
  if( m_svcManager ) m_svcManager->release();  // Need to be the last one!!
}

//============================================================================
// IInterface implementation: ApplicationMgr::addRef()
//============================================================================
unsigned long ApplicationMgr::addRef() {
  m_refcount++;
  return m_refcount;
}

//============================================================================
// IInterface implementation: ApplicationMgr::release()
//============================================================================
unsigned long ApplicationMgr::release() {
  long count = --m_refcount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

//============================================================================
// IInterface implementation: queryInterface::addRef()
//============================================================================
StatusCode ApplicationMgr::queryInterface
( const InterfaceID& iid  ,
  void**             ppvi )
{
  if ( 0 == ppvi ) { return StatusCode::FAILURE ; }

  // try to find own/direct interfaces:
  if      ( IAppMgrUI       ::interfaceID() . versionMatch ( iid ) )
  { *ppvi = static_cast<IAppMgrUI*>        ( this ) ; }
  else if ( IEventProcessor ::interfaceID() .versionMatch  ( iid ) )
  { *ppvi = static_cast<IEventProcessor*>  ( this ) ; }
  else if ( IService        ::interfaceID() . versionMatch ( iid ) )
  { *ppvi = static_cast<IService*>         ( this ) ; }
  else if ( INamedInterface ::interfaceID() . versionMatch ( iid ) )
  { *ppvi = static_cast<INamedInterface*>  ( this ) ; }
  else if ( IStateful       ::interfaceID() . versionMatch ( iid ) )
  { *ppvi = static_cast<IStateful*>        ( this ) ; }
  else if ( IInterface      ::interfaceID() . versionMatch ( iid ) )
  { *ppvi = static_cast<IInterface*>       ( this ) ; }
  // find indirect interfaces :
  else if ( ISvcLocator     ::interfaceID() . versionMatch ( iid ) )
  { return m_svcLocator   -> queryInterface ( iid , ppvi ) ; }
  else if ( ISvcManager     ::interfaceID() . versionMatch ( iid ) )
  { return m_svcManager   -> queryInterface ( iid , ppvi ) ; }
  else if ( IAlgManager     ::interfaceID() . versionMatch ( iid ) )
  { return m_algManager   -> queryInterface ( iid , ppvi ) ; }
  else if ( IClassManager   ::interfaceID() . versionMatch ( iid ) )
  { return m_classManager -> queryInterface ( iid , ppvi ) ; }
  else if ( IProperty       ::interfaceID() . versionMatch ( iid ) )
  { return m_propertyMgr  -> queryInterface ( iid , ppvi ) ; }
  else
  { *ppvi = 0 ; return StatusCode::FAILURE; }              // RETURN
  // increment the reference counter:
  addRef ();
  //
  return StatusCode::SUCCESS ;
}

//============================================================================
// ApplicationMgr::i_startup()
//============================================================================
StatusCode ApplicationMgr::i_startup() {
  MsgStream tlog( m_messageSvc, name() );
  IService   *msgsvc = 0, *jobsvc = 0;
  IProperty* jobOptsIProp = 0;
  IProperty* msgSvcIProp = 0;
  StatusCode  sc;

  // declare factories in current module
  m_classManager->loadModule("").ignore();

  // Create the Message service
  sc = m_svcManager->createService( m_messageSvcType, "MessageSvc", msgsvc );
  if( !sc.isSuccess() )  {
    tlog << MSG::FATAL << "Error creating MessageSvc of type "
         << m_messageSvcType << endreq;
    return sc;
  }
  // Create the Job Options service
  sc = m_svcManager->createService( m_jobOptionsSvcType, "JobOptionsSvc", jobsvc );
  if( !sc.isSuccess() )   {
    tlog << MSG::FATAL << "Error creating JobOptionsSvc" << endreq;
    return sc;
  }

  sc = m_svcLocator->service( "JobOptionsSvc", jobOptsIProp);
  if (  !sc.isSuccess() )   {
    tlog << MSG::FATAL << "Error locating JobOptionsSvc" << endreq;
    return sc;
  }
  sc = jobOptsIProp->setProperty( StringProperty("TYPE", m_jobOptionsType) );
  if( !sc.isSuccess() )   {
    tlog << MSG::FATAL << "Error setting TYPE option in JobOptionsSvc"
         << endreq;
    return sc;
  }

  if ( m_jobOptionsPath != "") {         // The command line takes precedence
    sc = jobOptsIProp->setProperty( StringProperty("PATH", m_jobOptionsPath) );
    if( !sc.isSuccess() )   {
      tlog << MSG::FATAL << "Error setting PATH option in JobOptionsSvc"
           << endreq;
      return sc;
    }
  }
  else if ( 0 != getenv("JOBOPTPATH") ) {// Otherwise the Environment JOBOPTPATH
    sc = jobOptsIProp->setProperty (StringProperty("PATH",
                                                   getenv("JOBOPTPATH")));
    if( !sc.isSuccess() )   {
      tlog << MSG::FATAL
           << "Error setting PATH option in JobOptionsSvc from env"
           << endreq;
      return sc;
    }
  }
  else {                                   // Otherwise the default
    sc = jobOptsIProp->setProperty (StringProperty("PATH",
                                                   "../options/job.opts"));
    if( !sc.isSuccess() )   {
      tlog << MSG::FATAL
           << "Error setting PATH option in JobOptionsSvc to default"
           << endreq;
      return sc;
    }
  }
  jobOptsIProp->release();

  // Sets my default the Output Level of the Message service to be
  // the same as this
  m_svcLocator->service( "MessageSvc", msgSvcIProp).ignore();
  msgSvcIProp->setProperty( IntegerProperty("OutputLevel", m_outputLevel)).ignore();
  msgSvcIProp->release();

  sc = jobsvc->sysInitialize();
  if( !sc.isSuccess() )   {
    tlog << MSG::FATAL << "Error initializing JobOptionsSvc" << endreq;
    return sc;
  }
  sc = msgsvc->sysInitialize();
  if( !sc.isSuccess() )  {
    tlog << MSG::FATAL << "Error initializing MessageSvc" << endreq;
    return sc;
  }

  // Get the useful interface from Message anf JobOptions services
  sc = m_svcLocator->service( "MessageSvc", m_messageSvc);
  if( !sc.isSuccess() )  {
    tlog << MSG::FATAL << "Error retrieving MessageSvc." << endreq;
    return sc;
  }
  sc = m_svcLocator->service( "JobOptionsSvc", m_jobOptionsSvc);
  if( !sc.isSuccess() )  {
    tlog << MSG::FATAL << "Error retrieving JobOptionsSvc." << endreq;
    return sc;
  }
  jobsvc->release();
  msgsvc->release();

  return sc;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::configure()
//============================================================================
StatusCode ApplicationMgr::configure() {
  // Check if the state is compatible with the transition
  MsgStream tlog( m_messageSvc, name() );
  if( Gaudi::StateMachine::CONFIGURED == m_state ) {
    tlog << MSG::INFO << "Already Configured" << endreq;
    return StatusCode::SUCCESS;
  }
  else if( Gaudi::StateMachine::OFFLINE != m_state ) {
    tlog << MSG::FATAL
         << "configure: Invalid state \""  << m_state << "\"" << endreq;
    return StatusCode::FAILURE;
  }
  m_targetState = Gaudi::StateMachine::CONFIGURED;

  StatusCode  sc;
  sc = i_startup();
  if ( !sc.isSuccess() )    {
    return sc;
  }

  MsgStream log( m_messageSvc, name() );

  // Get my own options using the Job options service
  log << MSG::DEBUG << "Getting my own properties" << endreq;
  sc = m_jobOptionsSvc->setMyProperties( name(), m_propertyMgr );
  if( !sc.isSuccess() ) {
    log << MSG::WARNING << "Problems getting my properties from JobOptionsSvc"
        << endreq;
    return sc;
  }

  // Check current outputLevel to eventually inform the MessageSvc
  if( m_outputLevel != MSG::NIL && !m_appName.empty() ) {
    assert(0 != m_messageSvc);
    m_messageSvc->setOutputLevel( name(), m_outputLevel );
    // Print a welcome message
    log << MSG::ALWAYS
        << std::endl
        << "=================================================================="
        << "=================================================================="
        << std::endl
        << "                                "
        << "                   Welcome to " << m_appName;

    if( "" != m_appVersion ) {
      log << MSG::ALWAYS << " version " << m_appVersion;
    }
    else {
      log << MSG::ALWAYS << " $Revision: 1.77 $";
    }

    // Add the host name and current time to the message
    time_t t;
    std::time( &t );
    tm* localt = std::localtime( &t );

    log << MSG::ALWAYS
        << std::endl
        << "                                "
        << "          running on " << System::hostName()
        << " on " << std::asctime( localt )
        << "=================================================================="
        << "=================================================================="
        << endmsg;
  }

  // print all own properties if the options "PropertiesPrint" is set to true
  if ( m_propertiesPrint )
  {
    typedef std::vector<Property*> Properties;
    const Properties& properties = m_propertyMgr->getProperties() ;
    log << MSG::ALWAYS
        << "List of ALL properties of "
        << System::typeinfoName ( typeid( *this ) ) << "/" << this->name()
        << "  #properties = " << properties.size() << endreq ;
    for ( Properties::const_iterator property = properties.begin() ;
          properties.end() != property ; ++property )
    { log << "Property ['Name': Value] = " << ( **property) << endreq ; }
  }

  // Check if StatusCode need to be checked
  if (m_codeCheck) {
    StatusCode::enableChecking();
    sc = addMultiSvc("StatusCodeSvc", "StatusCodeSvc", -9999);
    if ( sc.isFailure() ) {
      log << MSG::FATAL << "Error adding StatusCodeSvc for multiple threads" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    StatusCode::disableChecking();
  }

  // set the requested environment variables
  std::map<std::string,std::string>::iterator var;
  for ( var = m_environment.begin(); var != m_environment.end(); ++var ) {
    const std::string &name  = var->first;
    const std::string &value = var->second;
    std::string old = System::getEnv(name.c_str());
    if ( !old.empty() && (old != "UNKNOWN" )) {
      log << MSG::WARNING;
    }
    else {
      log << MSG::DEBUG;
    }
    log << "Setting " << name << " = " << value << endmsg;
    System::setEnv(name,value);
  }

  //Declare Service Types
  VectorName::const_iterator j;
  for(j=m_svcMapping.begin(); j != m_svcMapping.end(); ++j)  {
    ListItem itm(*j);
    if ( declareMultiSvcType(itm.name(), itm.type()).isFailure() )  {
      log << MSG::ERROR << "configure: declaring svc type:'" << *j << "' failed." << endmsg;
      return StatusCode::FAILURE;
    }
  }
  for(j=m_svcOptMapping.begin(); j != m_svcOptMapping.end(); ++j)  {
    ListItem itm(*j);
    if ( declareMultiSvcType(itm.name(), itm.type()).isFailure() )  {
      log << MSG::ERROR << "configure: declaring svc type:'" << *j << "' failed." << endmsg;
      return StatusCode::FAILURE;
    }
  }

  //--------------------------------------------------------------------------
  // Declare other Services and Algorithms by loading DLL's
  sc = decodeDllNameList( );
  if ( sc.isFailure( ) ) {
    log << MSG::ERROR << "Failure loading declared DLL's" << endreq;
    return sc;
  }

  //--------------------------------------------------------------------------
  // Deal with the services explicitely declared by the user.
  sc = decodeExtSvcNameList();
  if ( sc.isFailure( ) ) {
    log << MSG::ERROR << "Failure during external service association" << endreq;
    return sc;
  }

  sc = decodeMultiThreadSvcNameList( );
  if ( sc.isFailure( ) ) {
    log << MSG::ERROR << "Failure during multi thread service creation"
        << endreq;
    return sc;
  }

  sc = decodeCreateSvcNameList();
  if ( sc.isFailure( ) ) {
    log << MSG::ERROR << "Failure during external service creation" << endreq;
    return sc;
  }


  //--------------------------------------------------------------------------
  // Retrieve intrinsic services. If needed configure them.
  //--------------------------------------------------------------------------
  ListItem evtloop_item(m_eventLoopMgr);
  sc = addMultiSvc(evtloop_item.type(), evtloop_item.name(), 100);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error adding :" << m_eventLoopMgr << endreq;
    return sc;
  }

  if (m_noOfEvtThreads == 0) {
    sc = m_svcLocator->service(m_runableType, m_runable);
    if( !sc.isSuccess() )  {
      log << MSG::FATAL
          << "Error retrieving Runable:" << m_runableType
          << "\n Check option ApplicationMgr." << s_runable << endreq;
      return sc;
    }
    sc = m_svcLocator->service( evtloop_item.name(), m_processingMgr);
    if( !sc.isSuccess() )  {
      log << MSG::FATAL
          << "Error retrieving Processing manager:" << m_eventLoopMgr
          << "\n Check option ApplicationMgr." << s_eventloop
          << "\n No events will be processed." << endreq;
      return sc;
    }
  }

  // Establish Update Handlers for ExtSvc and DLLs Properties
  m_extSvcNameList.declareUpdateHandler (&ApplicationMgr::extSvcNameListHandler,
                                         this);
  m_createSvcNameList.declareUpdateHandler (&ApplicationMgr::createSvcNameListHandler,
                                         this);
  m_multiThreadSvcNameList.declareUpdateHandler
    (&ApplicationMgr::multiThreadSvcNameListHandler, this);
  m_dllNameList.declareUpdateHandler (&ApplicationMgr::dllNameListHandler,
                                      this );

  if (m_actHistory) {
    // Create HistorySvc with a priority to ensure it's initialized last, finalized first
    sc = m_svcManager->addService("HistorySvc","HistorySvc",std::numeric_limits<int>::max());
    if ( sc.isFailure() ) {
      log << MSG::FATAL << "Error adding HistorySvc" << endreq;
      return StatusCode::FAILURE;
    }

    if (m_noOfEvtThreads > 0) {
      sc = addMultiSvc("HistorySvc","HistorySvc",std::numeric_limits<int>::max());
      if ( sc.isFailure() ) {
        log << MSG::FATAL << "Error adding HistorySvc for multiple threads"
            << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  log << MSG::INFO << "Application Manager Configured successfully" << endreq;
  m_state = m_targetState;
  return StatusCode::SUCCESS;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::initialize()
//============================================================================
StatusCode ApplicationMgr::initialize() {

  MsgStream log( m_messageSvc, name() );
  StatusCode sc;

  if( m_state == Gaudi::StateMachine::INITIALIZED ) {
    log << MSG::INFO << "Already Initialized!" << endreq;
    return StatusCode::SUCCESS;
  }
  else if( m_state != Gaudi::StateMachine::CONFIGURED ) {
    log << MSG::FATAL
         << "initialize: Invalid state \""  << m_state << "\"" << endreq;
    return StatusCode::FAILURE;
  }
  m_targetState = Gaudi::StateMachine::INITIALIZED;

  //--------------------------------------------------------------------------
  // Initialize the list of top Services
  //--------------------------------------------------------------------------
  sc = m_svcManager->initializeServices();
  if( !sc.isSuccess() ) return sc;

  //--------------------------------------------------------------------------
  // Final steps: Inform user and change internal state
  //--------------------------------------------------------------------------
  log << MSG::INFO << "Application Manager Initialized successfully"  << endreq;
  m_state = m_targetState;

  return sc;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::start()
//============================================================================
StatusCode ApplicationMgr::start() {

  MsgStream log( m_messageSvc, name() );
  StatusCode sc;

  if( m_state == Gaudi::StateMachine::RUNNING ) {
    log << MSG::INFO << "Already Initialized!" << endreq;
    return StatusCode::SUCCESS;
  }
  else if( m_state != Gaudi::StateMachine::INITIALIZED ) {
    log << MSG::FATAL
         << "start: Invalid state \""  << m_state << "\"" << endreq;
    return StatusCode::FAILURE;
  }
  m_targetState = Gaudi::StateMachine::RUNNING;

  //--------------------------------------------------------------------------
  // Initialize the list of top Services
  //--------------------------------------------------------------------------
  sc = m_svcManager->startServices();
  if( !sc.isSuccess() ) return sc;

  //--------------------------------------------------------------------------
  // Final steps: Inform user and change internal state
  //--------------------------------------------------------------------------
  log << MSG::INFO << "Application Manager Started successfully"  << endreq;
  m_state = m_targetState;

  return sc;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::nextEvent(int)
//============================================================================
StatusCode ApplicationMgr::nextEvent(int maxevt)    {
  if( m_state != Gaudi::StateMachine::RUNNING ) {
    MsgStream log( m_messageSvc, name() );
    log << MSG::FATAL << "nextEvent: Invalid state \"" << m_state << "\""
        << endreq;
    return StatusCode::FAILURE;
  }
  if ( 0 == m_processingMgr )   {
    MsgStream log( m_messageSvc, name() );
    log << MSG::FATAL << "No event processing manager specified. Check option:"
        << s_eventloop << endreq;
    return StatusCode::FAILURE;
  }
  return m_processingMgr->nextEvent(maxevt);
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::stop()
//============================================================================
StatusCode ApplicationMgr::stop() {

  MsgStream log( m_messageSvc, name() );
  StatusCode sc;

  if( m_state == Gaudi::StateMachine::INITIALIZED ) {
    log << MSG::INFO << "Already Initialized!" << endreq;
    return StatusCode::SUCCESS;
  }
  else if( m_state != Gaudi::StateMachine::RUNNING ) {
    log << MSG::FATAL
         << "stop: Invalid state \""  << m_state << "\"" << endreq;
    return StatusCode::FAILURE;
  }
  m_targetState = Gaudi::StateMachine::INITIALIZED;

  // Stop independently managed Algorithms
  sc = m_algManager->stopAlgorithms();
  if( !sc.isSuccess() ) return sc;

  //--------------------------------------------------------------------------
  // Stop the list of top Services
  //--------------------------------------------------------------------------
  sc = m_svcManager->stopServices();
  if( !sc.isSuccess() ) return sc;

  //--------------------------------------------------------------------------
  // Final steps: Inform user and change internal state
  //--------------------------------------------------------------------------
  log << MSG::INFO << "Application Manager Stopped successfully"  << endreq;
  m_state = m_targetState;

  return sc;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::finalize()
//============================================================================
StatusCode ApplicationMgr::finalize() {
  MsgStream log( m_messageSvc, name() );
  if( m_state == Gaudi::StateMachine::CONFIGURED ) {
    log << MSG::INFO << "Already Finalized" << endreq;
    return StatusCode::SUCCESS;
  }
  else if( m_state != Gaudi::StateMachine::INITIALIZED ) {
    log << MSG::FATAL << "finalize: Invalid state \"" << m_state << "\""
	<< endreq;
    return StatusCode::FAILURE;
  }
  m_targetState = Gaudi::StateMachine::CONFIGURED;

  IProperty* msgSvcIProp = 0;
  // disable message suppression in finalize
  m_svcLocator->service( "MessageSvc", msgSvcIProp).ignore();
  msgSvcIProp->setProperty( BooleanProperty("enableSuppression", false)).ignore();
  msgSvcIProp->release();

  // Finalize independently managed Algorithms
  StatusCode sc = m_algManager->finalizeAlgorithms();


  // Finalize all Services
  sc = m_svcManager->finalizeServices();

  m_svcManager->removeService( (IService*) m_processingMgr );
  m_svcManager->removeService( (IService*) m_runable );

  if (m_codeCheck) {
    StatusCode::disableChecking();
  }

  log << MSG::INFO << "Application Manager Finalized successfully" << endreq;

  m_state = m_targetState;
  return sc;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::terminate()
//============================================================================
StatusCode ApplicationMgr::terminate() {
  MsgStream log( m_messageSvc, name() );

  if( m_state == Gaudi::StateMachine::OFFLINE ) {
    log << MSG::INFO << "Already Offline" << endreq;
    return StatusCode::SUCCESS;
  }
  else if( m_state != Gaudi::StateMachine::CONFIGURED ) {
    log << MSG::FATAL << "terminate: Invalid state \"" << m_state << "\""
	<< endreq;
    return StatusCode::FAILURE;
  }
  // release all Services
  m_targetState = Gaudi::StateMachine::OFFLINE;


  log << MSG::INFO << "Application Manager Terminated successfully" << endreq;

  // finalize MessageSvc
  IService *svc = 0;
  StatusCode sc = m_messageSvc->queryInterface(IService::interfaceID(),pp_cast<void>(&svc));
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not get the IService interface of the MessageSvc" << endreq;
  } else {
    svc->sysFinalize().ignore();
    svc->release();
  }

  // finalize JobOptionsSvc
  svc = 0;
  sc = m_jobOptionsSvc->queryInterface(IService::interfaceID(),pp_cast<void>(&svc));
  if ( !sc.isSuccess() ) {
    log << MSG::ERROR << "Could not get the IService interface of the JobOptionsSvc" << endreq;
  } else {
    svc->sysFinalize().ignore();
    svc->release();
  }

  m_state = m_targetState;
  return StatusCode::SUCCESS;
}

//============================================================================
// Reach the required state going through all the needed transitions
//============================================================================
StatusCode ApplicationMgr::GoToState(Gaudi::StateMachine::State state, bool ignoreFailures) {
  StatusCode sc = StatusCode(StatusCode::SUCCESS,true);

  switch (state) {

  case Gaudi::StateMachine::OFFLINE:
    switch (m_state) {
    case Gaudi::StateMachine::OFFLINE    : return StatusCode::SUCCESS; break;
    case Gaudi::StateMachine::CONFIGURED : return terminate(); break;
    default: // Gaudi::StateMachine::INITIALIZED or Gaudi::StateMachine::RUNNING
      sc = GoToState(Gaudi::StateMachine::CONFIGURED);
      if (sc.isSuccess()) {
        return terminate();
      } break;
    } break;

  case Gaudi::StateMachine::CONFIGURED:
    switch (m_state) {
    case Gaudi::StateMachine::CONFIGURED  : return StatusCode::SUCCESS; break;
    case Gaudi::StateMachine::OFFLINE     : return configure(); break;
    case Gaudi::StateMachine::INITIALIZED : return finalize(); break;
    default: // Gaudi::StateMachine::RUNNING
      sc = GoToState(Gaudi::StateMachine::INITIALIZED);
      if (sc.isSuccess()) {
        return finalize();
      } break;
    } break;

  case Gaudi::StateMachine::INITIALIZED:
    switch (m_state) {
    case Gaudi::StateMachine::INITIALIZED : return StatusCode::SUCCESS; break;
    case Gaudi::StateMachine::CONFIGURED  : return initialize(); break;
    case Gaudi::StateMachine::RUNNING     : return stop(); break;
    default: // Gaudi::StateMachine::OFFLINE
      sc = GoToState(Gaudi::StateMachine::CONFIGURED);
      if (sc.isSuccess()) {
        return initialize();
      } break;
    } break;

  case Gaudi::StateMachine::RUNNING:
    switch (m_state) {
    case Gaudi::StateMachine::RUNNING     : return StatusCode::SUCCESS; break;
    case Gaudi::StateMachine::INITIALIZED : return start(); break;
    default: // Gaudi::StateMachine::OFFLINE or Gaudi::StateMachine::CONFIGURED
      sc = GoToState(Gaudi::StateMachine::INITIALIZED);
      if (sc.isSuccess()) {
        return start();
      } break;
    } break;

  }

  // If I get here, there has been a problem in the recursion

  if (ignoreFailures){
    // force the new state
    m_state = state;
    return StatusCode::SUCCESS;
  }

  return sc;
}

//============================================================================
// IAppMgrUI implementation: ApplicationMgr::run()
//============================================================================
StatusCode ApplicationMgr::run() {
  StatusCode sc = StatusCode::SUCCESS;

  sc = GoToState(Gaudi::StateMachine::RUNNING);
  if ( sc.isSuccess() ) {
    MsgStream log(m_messageSvc, name());
    if ( 0 != m_runable ) { // loop over the events
      sc = m_runable->run();
      if ( !sc.isSuccess() ) {
        log << MSG::FATAL << "Application execution failed. Ending the job."
            << endreq;
      }
    } else {
      log << MSG::FATAL << "Application has no runable object. Check option:"
          << s_runable << endreq;
    }
  }
  if (sc.isSuccess()) { // try to close cleanly
    sc = GoToState(Gaudi::StateMachine::OFFLINE);
  }
  // either the runable failed of the stut-down
  if (sc.isFailure()) { // try to close anyway (but keep the StatusCode unchanged)
    GoToState(Gaudi::StateMachine::OFFLINE,true).ignore();
  }
  return sc;
}

//============================================================================
// IEventProcessor implementation: executeEvent(void* par)
//============================================================================
StatusCode ApplicationMgr::executeEvent(void* par)    {
  MsgStream log( m_messageSvc, name() );
  if( m_state == Gaudi::StateMachine::RUNNING ) {
    SmartIF<IEventProcessor> processor(IID_IEventProcessor, m_processingMgr);
    if ( processor.isValid() )    {
      return processor->executeEvent(par);
    }
  }
  log << MSG::FATAL << "executeEvent: Invalid state \"" << FSMState() << "\""
      <<endreq;
  return StatusCode::FAILURE;
}

//============================================================================
// IEventProcessor implementation: executeRun(int)
//============================================================================
StatusCode ApplicationMgr::executeRun(int evtmax)    {
  MsgStream log( m_messageSvc, name() );
  if( m_state == Gaudi::StateMachine::RUNNING ) {
    SmartIF<IEventProcessor> processor(IID_IEventProcessor, m_processingMgr);
    if ( processor.isValid() )    {
      return processor->executeRun(evtmax);
    }
    log << MSG::WARNING << "No EventLoop Manager specified " << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::FATAL << "executeRun: Invalid state \"" << FSMState() << "\""
      << endreq;
  return StatusCode::FAILURE;
}

//============================================================================
// IEventProcessor implementation: stopRun(int)
//============================================================================
StatusCode ApplicationMgr::stopRun()    {
  MsgStream log( m_messageSvc, name() );
  if( m_state == Gaudi::StateMachine::RUNNING ) {
    SmartIF<IEventProcessor> processor(IID_IEventProcessor, m_processingMgr);
    if ( processor.isValid() )    {
      return processor->stopRun();
    }
    log << MSG::WARNING << "No EventLoop Manager specified " << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::FATAL << "stopRun: Invalid state \"" << FSMState() << "\""
      << endreq;
  return StatusCode::FAILURE;
}
// Implementation of IAppMgrUI::name
const std::string& ApplicationMgr::name() const {
  return m_name;
}

// implementation of IService::type
const InterfaceID& ApplicationMgr::type() const {
  return IID_IService;
}
// implementation of IService::state
Gaudi::StateMachine::State ApplicationMgr::FSMState( ) const {
  return m_state;
}
// implementation of IService::state
Gaudi::StateMachine::State ApplicationMgr::targetFSMState( ) const {
  return m_targetState;
}


//============================================================================
// implementation of IService::reinitilaize
//============================================================================
StatusCode ApplicationMgr::reinitialize() {
  StatusCode retval = StatusCode::SUCCESS;
  StatusCode sc;
  if ( m_state < Gaudi::StateMachine::INITIALIZED ) {
    throw GaudiException("Cannot reinitialize application if not INITIALIZED or RUNNING",
                         "ApplicationMgr::reinitialize", StatusCode::FAILURE);
  }
  if ( m_state == Gaudi::StateMachine::RUNNING ) {
    retval = GoToState(Gaudi::StateMachine::INITIALIZED);
  }
  sc = m_svcManager->reinitializeServices();
  if (sc.isFailure()) retval = sc;
  sc = m_algManager->reinitializeAlgorithms();
  if (sc.isFailure()) retval = sc;
  return retval;
}

//============================================================================
// implementation of IService::reinitilaize
//============================================================================
StatusCode ApplicationMgr::restart() {
  StatusCode retval = StatusCode::SUCCESS;
  StatusCode sc;
  if ( m_state != Gaudi::StateMachine::RUNNING ) {
    throw GaudiException("Cannot restart application if not RUNNING",
                         "ApplicationMgr::restart", StatusCode::FAILURE);
  }
  sc = m_svcManager->restartServices();
  if (sc.isFailure()) retval = sc;
  sc = m_algManager->restartAlgorithms();
  if (sc.isFailure()) retval = sc;
  return retval;
}

//============================================================================
// SI Go Handler
//============================================================================
void ApplicationMgr::SIGoHandler( Property& ) {

  MsgStream log (m_messageSvc, name());
  StatusCode sc;

  // Re-initialize everything
  sc = reinitialize();
  // Execute a number of events
  executeRun(m_evtMax);

  return;
}

//============================================================================
// SI Exit Handler
//============================================================================
void ApplicationMgr::SIExitHandler( Property& ) {
  StatusCode status;
  status = finalize();
  status = terminate();
  ::exit( 0 );
}

//============================================================================
// Handle properties of the event loop manager (Top alg/Output stream list)
//============================================================================
void ApplicationMgr::evtLoopPropertyHandler( Property& p ) {
  if ( m_processingMgr )    {
    SmartIF<IProperty> props(IID_IProperty, m_processingMgr);
    if ( props.isValid() )    {
      props->setProperty( p ).ignore();
    }
  }
}

//============================================================================
// External Service List handler
//============================================================================
void ApplicationMgr::createSvcNameListHandler( Property& /* theProp */ ) {
  if ( !(decodeCreateSvcNameList()).isSuccess() ) {
    throw GaudiException("Failed to create ext services",
 			 "MinimalEventLoopMgr::createSvcNameListHandler",
 			 StatusCode::FAILURE);
  }
}
//============================================================================
//  decodeCreateSvcNameList
//============================================================================
StatusCode ApplicationMgr::decodeCreateSvcNameList() {
  StatusCode result = StatusCode::SUCCESS;
  const std::vector<std::string>& theNames = m_createSvcNameList.value( );
  VectorName::const_iterator it(theNames.begin());
  VectorName::const_iterator et(theNames.end());
  while(result.isSuccess() && it != et) {
    ListItem item(*it++);
    if( (result = m_svcManager->addService(item.type(), item.name(), 10) ).isFailure()) {
      MsgStream log( m_messageSvc, m_name );
      log << MSG::ERROR << "decodeCreateSvcNameList: Cannot create service "
          << item.type() << "/" << item.name() << endmsg;
    } else {
      MsgStream log( m_messageSvc, m_name );
      log << MSG::DEBUG << "decodeCreateSvcNameList: Created service "
          << item.type() << "/" << item.name() << endmsg;
    }
  }
  return result;
}

//============================================================================
// External Service List handler
//============================================================================
void ApplicationMgr::extSvcNameListHandler( Property& /* theProp */ ) {
  if ( !(decodeExtSvcNameList( )).isSuccess() ) {
    throw GaudiException("Failed to declare ext services",
                         "MinimalEventLoopMgr::extSvcNameListHandler",
                         StatusCode::FAILURE);
  }
}

//============================================================================
//  decodeExtSvcNameList
//============================================================================
StatusCode ApplicationMgr::decodeExtSvcNameList( ) {
  StatusCode result = StatusCode::SUCCESS;

  std::vector<std::string> theNames = m_extSvcNameList.value( );

  VectorName::const_iterator it(theNames.begin());
  VectorName::const_iterator et(theNames.end());
  while(result.isSuccess() && it != et) {
    ListItem item(*it++);
    if (m_extSvcCreates == true) {
      if ( (result = m_svcManager->addService(item.type(),
					      item.name(), 10)).isFailure()) {
        MsgStream log( m_messageSvc, m_name );
        log << MSG::ERROR << "decodeExtSvcNameList: Cannot create service "
            << item.type() << "/" << item.name() << endmsg;
      }
    } else {
      if( ( result = m_svcManager->declareSvcType(item.name(),
						  item.type()) ).isFailure()) {
        MsgStream log( m_messageSvc, m_name );
        log << MSG::ERROR << "decodeExtSvcNameList: Cannot declare service "
            << item.type() << "/" << item.name() << endmsg;
      }
    }
  }
  return result;
}

//============================================================================
// External Service List handler
//============================================================================
void ApplicationMgr::multiThreadSvcNameListHandler( Property& /* theProp */ ) {
  if ( !(decodeMultiThreadSvcNameList( )).isSuccess() ) {
    throw GaudiException("Failed to create copies of mt services",
                         "MinimalEventLoopMgr::multiThreadSvcNameListHandler",
                         StatusCode::FAILURE);
  }

}

//============================================================================
//  decodeMultiExtSvcNameList
//============================================================================
StatusCode ApplicationMgr::decodeMultiThreadSvcNameList( ) {
  StatusCode result = StatusCode::SUCCESS;
  const std::vector<std::string>& theNames = m_multiThreadSvcNameList.value( );
  for(int iCopy=0; iCopy<m_noOfEvtThreads; ++iCopy) {
    for (VectorName::const_iterator it = theNames.begin();
         it != theNames.end();
         ++it) {
      ListItem item(*it);
      result = addMultiSvc(item.type(), item.name(), 10);
      //FIXME SHOULD CLONE?
      if( result.isFailure() ) {
        MsgStream log( m_messageSvc, m_name );
        log << MSG::ERROR
            << "decodeMultiThreadSvcNameList: Cannot create service "
            << item.type() << "/" << item.name() << endmsg;
      } else {
        MsgStream log( m_messageSvc, m_name );
        log << MSG::VERBOSE
            << "decodeMultiThreadSvcNameList: created service "
            << item.type() << "/" << item.name() << endmsg;
      }
    }
  }
  return result;
}
//=============================================================================
//  declareMultiSvcType
//=============================================================================
StatusCode ApplicationMgr::declareMultiSvcType(const std::string& name,
                                               const std::string& type) {
  StatusCode result = StatusCode::SUCCESS;
  MsgStream log( m_messageSvc, m_name );
  if (0 == m_noOfEvtThreads) {
    result = m_svcManager->declareSvcType(name, type);
    if( result.isFailure() ) {
      log << MSG::ERROR << "declareMultiSvcType: Cannot declare service "
          << type << "/" << name << endmsg;
    } else {
      log << MSG::VERBOSE << "declareMultiSvcType: declared service "
          << type << "/" << name << endmsg;
    }
  } else {
    for(int iCopy=0; iCopy<m_noOfEvtThreads; ++iCopy) {
      std::string thrName(name + getGaudiThreadIDfromID(iCopy));
      result = m_svcManager->declareSvcType(thrName, type);
      if( result.isFailure() ) {
        log << MSG::ERROR << "declareMultiSvcType: Cannot declare service "
            << type << "/" << thrName << endmsg;
      } else {
        log << MSG::VERBOSE << "declareMultiSvcType: declared service "
            << type << "/" << thrName << endmsg;
      }
    }
  }
  return result;
}
//=============================================================================
//  addMultiSvc
//=============================================================================
StatusCode ApplicationMgr::addMultiSvc(const std::string& type,
                                       const std::string& name,
                                       int prio) {
  StatusCode result = StatusCode::SUCCESS;
  MsgStream log( m_messageSvc, m_name );
  if (0 == m_noOfEvtThreads) {
    result = m_svcManager->addService(type, name, prio);
    // result = m_svcManager->addService(name, type, prio); // CHECKME???
    if( result.isFailure() ) {
      log << MSG::ERROR << "addMultiSvc: Cannot add service "
          << type << "/" << name << endmsg;
    } else {
      log << MSG::VERBOSE << "addMultiSvc: added service "
          << type << "/" << name << endmsg;
    }
  } else {
    for(int iCopy=0; iCopy<m_noOfEvtThreads; ++iCopy) {
      std::string thrName(name + getGaudiThreadIDfromID(iCopy));
      result = m_svcManager->addService(type, thrName, prio);
      if( result.isFailure() ) {
        log << MSG::ERROR << "addMultiSvc: Cannot add service "
            << type << "/" << thrName << endmsg;
      } else {
        log << MSG::VERBOSE << "addMultiSvc: added service "
            << type << "/" << thrName << endmsg;
      }
    }
  }
  return result;
}

//============================================================================
// Dll List handler
//============================================================================
void ApplicationMgr::dllNameListHandler( Property& /* theProp */ ) {
  if ( !(decodeDllNameList( )).isSuccess() ) {
    throw GaudiException("Failed to load DLLs.",
                         "MinimalEventLoopMgr::dllNameListHandler",
                         StatusCode::FAILURE);
  }
}

//============================================================================
//  decodeDllNameList
//============================================================================
StatusCode ApplicationMgr::decodeDllNameList() {

  MsgStream log( m_messageSvc, m_name );
  StatusCode result = StatusCode::SUCCESS;

  // Clean up multiple entries from DLL list
  // -------------------------------------------------------------------------
  std::vector<std::string> newList;
  std::map<std::string,unsigned int> dllInList, duplicateList;
  {for ( std::vector<std::string>::const_iterator it = m_dllNameList.value().begin();
        it != m_dllNameList.value().end(); ++it ) {
    if ( 0 == dllInList[*it] ) {
      newList.push_back(*it);        // first instance of this module
    } else { ++duplicateList[*it]; } // module listed multiple times
    ++dllInList[*it];                // increment count for this module
  }}
  //m_dllNameList = newList; // update primary list to new, filtered list (do not use the
                             // property itself otherwise we get called again infinitely)
  // List modules that were in there twice..
  if ( !duplicateList.empty() ) {
    log << MSG::DEBUG << "Removed duplicate entries for modules : ";
    for ( std::map<std::string,unsigned int>::const_iterator it = duplicateList.begin();
          it != duplicateList.end(); ++it ) {
      log << it->first << "(" << 1+it->second << ")";
      if ( it != --duplicateList.end() ) log << ", ";
    }
    log << endreq;
  }
  // -------------------------------------------------------------------------

  const std::vector<std::string>& theNames = newList;

  // only load the new dlls or previously failed dlls
  log << MSG::DEBUG << "Loading declared DLL's" << endreq;

  std::vector<std::string> successNames, failNames;
  std::vector<std::string>::const_iterator it;

  for (it = theNames.begin(); it != theNames.end(); it++) {
    if (std::find (m_okDlls.rbegin(), m_okDlls.rend(), *it) == m_okDlls.rend()){
      // found a new module name
      StatusCode status = m_classManager->loadModule( (*it) );
      if( status.isFailure() ) {
        failNames.push_back(*it);
        result = StatusCode::FAILURE;
      }
      else {
        successNames.push_back(*it);
      }
    }
  }

  // report back to the user and store the names of the succesfully loaded dlls
  if ( !successNames.empty() ) {
    log << MSG::INFO << "Successfully loaded modules : ";
    for (it = successNames.begin(); it != successNames.end(); it++) {
      log<< (*it);
      if( (it+1) != successNames.end())  log << ", ";
      // save name
      m_okDlls.push_back( *it );
    }
    log << endmsg;
  }

  if ( result == StatusCode::FAILURE ) {
    log << MSG::WARNING << "Failed to load modules: ";
    for (it = failNames.begin(); it != failNames.end(); it++) {
      log<< (*it);
      if( (it+1) != failNames.end())  log << ", ";
    }
    log << endmsg;
  }
  return result;
}

//============================================================================
// Reflex debug level handler
//============================================================================
void ApplicationMgr::reflexDebugPropertyHandler( Property& )
{
  // Setup debug level for Reflex plugin system
  MsgStream log (m_messageSvc, name());
  log << MSG::INFO
      << "Updating ROOT::Reflex::PluginService::SetDebug(level) to level="
      << (int)m_reflexDebugLevel
      << endreq;
  ROOT::Reflex::PluginService::SetDebug(m_reflexDebugLevel);
}

//============================================================================
// Reflex debug level handler
//============================================================================
void ApplicationMgr::initLoopCheckHndlr(Property&) {
  m_svcManager->setLoopCheckEnabled(m_loopCheck);
}
