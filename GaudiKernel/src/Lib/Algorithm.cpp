//$Id: Algorithm.cpp,v 1.40.2.1 2008/08/18 16:31:10 leggett Exp $
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAuditorSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IExceptionSvc.h"
#include "GaudiKernel/IProperty.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Chrono.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/Guards.h"


// Constructor
Algorithm::Algorithm( const std::string& name, ISvcLocator *pSvcLocator,
                      const std::string& version)
  : m_refCount(0),
    m_name(name),
    m_version(version),
    m_MS(0),
    m_EDS(0),
    m_ECS(0),
    m_DDS(0),
    m_DCS(0),
    m_HDS(0),
    m_NTS(0),
    //AIDATupleSvc:
    // m_ATS(0),
    m_CSS(0),
    m_RGS(0),
    m_EXS(0),
    m_pAuditorSvc(0),
    m_ptoolSvc(0),
    m_pMonitorSvc(NULL),
    m_pSvcLocator(pSvcLocator),
    m_filterPassed(true),
    m_isEnabled(true),
    m_isExecuted(false),
    m_isInitialized(false),
    m_isFinalized(false)
{
  m_propertyMgr = new PropertyMgr();
  m_subAlgms = new std::vector<Algorithm *>();

  // Declare common Algorithm properties with their defaults
  declareProperty( "OutputLevel",        m_outputLevel = MSG::NIL);
  declareProperty( "Enable",             m_isEnabled = true);
  declareProperty( "ErrorMax",           m_errorMax  = 1);
  declareProperty( "ErrorCount",         m_errorCount = 0);
  // Auditor monitoring properties

  // Get the default setting for service auditing from the AppMgr
  declareProperty( "AuditAlgorithms", m_auditInit );

  IProperty *appMgr;
  bool audit(false);
  if (serviceLocator()->service("ApplicationMgr", appMgr, false).isSuccess()) {
    const Property& prop = appMgr->getProperty("AuditAlgorithms");
    Property &pr = const_cast<Property&>(prop);
    if (m_name != "IncidentSvc") {
      setProperty( pr ).ignore();
    }
    audit = m_auditInit.value();
  }

  declareProperty( "AuditInitialize"  , m_auditorInitialize   = audit ) ;
  declareProperty( "AuditReinitialize", m_auditorReinitialize = audit ) ;
  declareProperty( "AuditExecute"     , m_auditorExecute      = audit ) ;
  declareProperty( "AuditFinalize"    , m_auditorFinalize     = audit ) ;
  declareProperty( "AuditBeginRun"    , m_auditorBeginRun       = audit ) ;
  declareProperty( "AuditEndRun"      , m_auditorEndRun       = audit ) ;

  declareProperty( "MonitorService"   , m_monitorSvcName      = "MonitorSvc" );
  
  // update handlers.
  m_outputLevel.declareUpdateHandler(&Algorithm::initOutputLevel, this);
 
}

// Default Destructor
Algorithm::~Algorithm() {
  delete m_subAlgms;
  delete m_propertyMgr;
  if ( m_MS )  m_MS->release();
  if ( m_EDS ) m_EDS->release();
  if ( m_ECS ) m_ECS->release();
  if ( m_DDS ) m_DDS->release();
  if ( m_DCS ) m_DCS->release();
  if ( m_HDS ) m_HDS->release();
  if ( m_NTS ) m_NTS->release();
  if ( m_CSS ) m_CSS->release();
  if ( m_RGS ) m_RGS->release();
  if ( m_pAuditorSvc ) m_pAuditorSvc->release();
  if ( m_ptoolSvc ) m_ptoolSvc->release();
  if ( m_pMonitorSvc ) { m_pMonitorSvc->undeclareAll(this); m_pMonitorSvc->release(); }
}

// IAlgorithm implementation
StatusCode Algorithm::sysInitialize() {

  // Bypass the initialization if the algorithm
  // has already been initialized.
  if ( m_isInitialized ) return StatusCode::SUCCESS;

  // Set the Algorithm's properties
  StatusCode sc = setProperties();
  if( sc.isFailure() ) return StatusCode::FAILURE;

  // Bypass the initialization if the algorithm is disabled.
  // Need to do this after setProperties.
  if ( !isEnabled( ) ) return StatusCode::SUCCESS;

  // Check current outputLevel to evetually inform the MessagsSvc
  //if( m_outputLevel != MSG::NIL ) {
  setOutputLevel( m_outputLevel );
  //}

  // Reset Error count
  m_errorCount = 0;

  // Invoke initialize() method of the derived class inside a try/catch clause
  try {
    { // limit the scope of the guard
      Gaudi::Guards::AuditorGuard guard(this,
                                        // check if we want to audit the initialize
                                        (m_auditorInitialize) ? auditorSvc() : 0,
                                        IAuditor::Initialize);
      // Invoke the initialize() method of the derived class
      sc = initialize();
    }
    if( sc.isFailure() ) return StatusCode::FAILURE;

    // Now initialize care of any sub-algorithms
    std::vector<Algorithm *>::iterator it;
    StatusCode result = StatusCode::SUCCESS;
    for (it = m_subAlgms->begin(); it != m_subAlgms->end(); it++) {
      sc = (*it)->sysInitialize();
      if( sc.isFailure() ) result = sc;
    }
    if( result.isFailure() ) {
      MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
      log << MSG::ERROR << " Error initializing one or several sub-algorithms"
          << endreq;
      return result;
    }
    // Indicate that this Algorithm has been initialized to prevent duplicate
    // attempts.
    setInitialized( );
    return StatusCode::SUCCESS;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }

  return StatusCode::FAILURE;
}

// IAlgorithm implementation
StatusCode Algorithm::sysReinitialize() {

  // Bypass the initialization if the algorithm is disabled.
  if ( !isEnabled( ) ) return StatusCode::SUCCESS;

  // Set the Algorithm's properties
  StatusCode sc;
  sc = setProperties();
  if( sc.isFailure() ) return StatusCode::FAILURE;

  // Check current outputLevel to evetually inform the MessagsSvc
  //if( m_outputLevel != MSG::NIL ) {
  setOutputLevel( m_outputLevel );
  //}

  // Reset Error count
  m_errorCount = 0;
  
  // Invoke reinitialize() method of the derived class inside a try/catch clause
  try {
    { // limit the scope of the guard
      Gaudi::Guards::AuditorGuard guard(this,
                                        // check if we want to audit the initialize
                                        (m_auditorReinitialize) ? auditorSvc() : 0,
                                        IAuditor::ReInitialize);
      // Invoke the reinitialize() method of the derived class
      sc = reinitialize();
    }
    if( sc.isFailure() ) return StatusCode::FAILURE;

    // Now initialize care of any sub-algorithms
    std::vector<Algorithm *>::iterator it;
    StatusCode result = StatusCode::SUCCESS;
    for (it = m_subAlgms->begin(); it != m_subAlgms->end(); it++) {
      sc = (*it)->sysReinitialize();
      if( sc.isFailure() ) result = sc;
    }
    if( result.isFailure() ) {
      MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
      log << MSG::ERROR << " Error initializing one or several sub-algorithms"
          << endreq;
      return result;
    }
    // Indicate that this Algorithm has been initialized to prevent duplicate
    // attempts.
    setInitialized( );
    return StatusCode::SUCCESS;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() + ".sysReinitialize()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }

  return StatusCode::FAILURE;
}

StatusCode Algorithm::reinitialize() {
  return StatusCode::SUCCESS;
}

// IAlgorithm implementation
StatusCode Algorithm::sysBeginRun() {

  // Bypass the beginRun if the algorithm is disabled.
  if ( !isEnabled( ) ) return StatusCode::SUCCESS;

  // Check current outputLevel to evetually inform the MessagsSvc
  //if( m_outputLevel != MSG::NIL ) {
  setOutputLevel( m_outputLevel );
  //}

  // Reset Error count
  m_errorCount = 0;

  StatusCode sc;
  // Invoke beginRun() method of the derived class inside a try/catch clause
  try {
    { // limit the scope of the guard
      Gaudi::Guards::AuditorGuard guard(this,
                                        // check if we want to audit the initialize
                                        (m_auditorBeginRun) ? auditorSvc() : 0,
                                        IAuditor::BeginRun);
      // Invoke the beginRun() method of the derived class
      sc = beginRun();
    }
    if( sc.isFailure() ) return StatusCode::FAILURE;

    // Now call beginRun for any sub-algorithms
    std::vector<Algorithm *>::iterator it;
    StatusCode result = StatusCode::SUCCESS;
    for (it = m_subAlgms->begin(); it != m_subAlgms->end(); it++) {
      sc = (*it)->sysBeginRun();
      if( sc.isFailure() ) result = sc;
    }
    if( result.isFailure() ) {
      MsgStream log ( msgSvc() , name() + ".sysBeginRun()" );
      log << MSG::ERROR << " Error executing BeginRun for one or several sub-algorithms"
          << endreq;
      return result;
    }
    return StatusCode::SUCCESS;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() + ".sysBeginRun()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysBeginRun()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() + ".sysBeginRun()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }
  return StatusCode::FAILURE;
}

StatusCode Algorithm::beginRun() {
  return StatusCode::SUCCESS;
}

// IAlgorithm implementation
StatusCode Algorithm::sysEndRun() {

  // Bypass the endRun if the algorithm is disabled.
  if ( !isEnabled( ) ) return StatusCode::SUCCESS;

  // Check current outputLevel to eventually inform the MessagsSvc
  //if( m_outputLevel != MSG::NIL ) {
  setOutputLevel( m_outputLevel );
  //}

  // Reset Error count
  m_errorCount = 0;

  // Invoke endRun() method of the derived class inside a try/catch clause
  StatusCode sc;
  try {
    { // limit the scope of the guard
      Gaudi::Guards::AuditorGuard guard(this,
                                        // check if we want to audit the initialize
                                        (m_auditorEndRun) ? auditorSvc() : 0,
                                        IAuditor::EndRun);
      // Invoke the endRun() method of the derived class
      sc = endRun();
    }
    if( sc.isFailure() ) return StatusCode::FAILURE;

    // Now call endRun for any sub-algorithms
    std::vector<Algorithm *>::iterator it;
    StatusCode result = StatusCode::SUCCESS;
    for (it = m_subAlgms->begin(); it != m_subAlgms->end(); it++) {
      sc = (*it)->sysEndRun();
      if( sc.isFailure() ) result = sc;
    }
    if( result.isFailure() ) {
      MsgStream log ( msgSvc() , name() + ".sysEndRun()" );
      log << MSG::ERROR << " Error calling endRun for one or several sub-algorithms"
          << endreq;
      return result;
    }
    return StatusCode::SUCCESS;
  }
  catch ( const GaudiException& Exception )  {
    MsgStream log ( msgSvc() , name() + ".sysEndRun()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    Stat stat( chronoSvc() , Exception.tag() );
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysEndRun()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    Stat stat( chronoSvc() , "*std::exception*" );
  }
  catch(...) {
    MsgStream log ( msgSvc() , name() + ".sysEndRun()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }
  return StatusCode::FAILURE;
}

StatusCode Algorithm::endRun() {
  return StatusCode::SUCCESS;
}


StatusCode Algorithm::sysExecute() {
  if (!isEnabled()) {
    MsgStream log ( msgSvc() , name() );
    log << MSG::VERBOSE << ".sysExecute(): is not enabled. Skip execution" <<endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode status;

  // Should performance profile be performed ?
  // invoke execute() method of Algorithm class
  //   and catch all uncaught exceptions

  Gaudi::Guards::AuditorGuard guard(this,
                                    // check if we want to audit the initialize
                                    (m_auditorExecute) ? auditorSvc() : 0,
                                    IAuditor::Execute,
                                    status);
  try {
    status = execute();
    setExecuted(true);  // set the executed flag

    if (status.isFailure()) {
      status = exceptionSvc()->handleErr(*this,status);
    }

  }
  catch( const GaudiException& Exception ) {
    setExecuted(true);  // set the executed flag

    MsgStream log ( msgSvc() , name() + ".sysExecute()" );
    if (Exception.code() == StatusCode::FAILURE) {
      log << MSG::FATAL;
    } else {
      log << MSG::ERROR << " Recoverable";
    }

    log << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;

    log << MSG::ERROR << Exception  << endreq;

    Stat stat( chronoSvc() , Exception.tag() ) ;
    status = exceptionSvc()->handle(*this,Exception);
  }
  catch( const std::exception& Exception ) {
    setExecuted(true);  // set the executed flag

    MsgStream log ( msgSvc() , name() + ".sysExecute()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    Stat stat( chronoSvc() , "*std::exception*" ) ;
    status = exceptionSvc()->handle(*this,Exception);
  }
  catch(...) {
    setExecuted(true);  // set the executed flag

    MsgStream log ( msgSvc() , name() + ".sysExecute()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;

    status = exceptionSvc()->handle(*this);
  }

  if( status.isFailure() ) {
    MsgStream log ( msgSvc() , name() );
    // Increment the error count
    m_errorCount++;
    // Check if maximum is exeeded
    if( m_errorCount < m_errorMax ) {
      log << MSG::WARNING << "Continuing from error (cnt=" << m_errorCount
          << ", max=" << m_errorMax << ")" << endreq;
      // convert to success
      status = StatusCode::SUCCESS;
    }
  }
  return status;
}

StatusCode Algorithm::sysFinalize() {

  // Bypass the finalialization if the algorithm hasn't been initialized or
  // has already been finalialized.
  if ( !isInitialized( ) || isFinalized( ) ) return StatusCode::SUCCESS;

  // Invoke finalize() method of the derived class inside a try/catch clause
  StatusCode sc = StatusCode::SUCCESS;
  try {
    // Order changed (bug #3903 overview: finalize and nested algorithms)
    // Finalize first any sub-algoithms (it can be done more than once)
    std::vector<Algorithm *>::iterator it;
    for (it = m_subAlgms->begin(); it != m_subAlgms->end(); it++) {
      (*it)->sysFinalize().ignore();
    }

    { // limit the scope of the guard
      Gaudi::Guards::AuditorGuard guard(this,
                                        // check if we want to audit the initialize
                                        (m_auditorFinalize) ? auditorSvc() : 0,
                                        IAuditor::Finalize);
      // Invoke the finalize() method of the derived class
      sc = finalize();
    }
    if( !sc.isSuccess() )  return StatusCode::FAILURE;

    // Release all sub-algorithms
    for (it = m_subAlgms->begin(); it != m_subAlgms->end(); it++) {
      (*it)->release();
    }
    // Indicate that this Algorithm has been finalized to prevent duplicate attempts
    setFinalized( );
    return sc;
  }
  catch( const GaudiException& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysFinalize()" );
    log << MSG::FATAL << " Exception with tag=" << Exception.tag()
        << " is caught " << endreq;
    log << MSG::ERROR << Exception  << endreq;
    Stat stat( chronoSvc() , Exception.tag() ) ;
  }
  catch( const std::exception& Exception ) {
    MsgStream log ( msgSvc() , name() + ".sysFinalize()" );
    log << MSG::FATAL << " Standard std::exception is caught " << endreq;
    log << MSG::ERROR << Exception.what()  << endreq;
    Stat stat( chronoSvc() , "*std::exception*" ) ;
  }
  catch( ... ) {
    MsgStream log ( msgSvc() , name() + ".sysFinalize()" );
    log << MSG::FATAL << "UNKNOWN Exception is caught " << endreq;
    Stat stat( chronoSvc() , "*UNKNOWN Exception*" ) ;
  }
  return StatusCode::FAILURE ;
}

const std::string& Algorithm::name() const {
  return m_name;
}

const std::string& Algorithm::version() const {
  return m_version;
}

bool Algorithm::isExecuted() const {
  return m_isExecuted;
}

void Algorithm::setExecuted( bool state ) {
  m_isExecuted = state;
}

void Algorithm::resetExecuted() {
  m_isExecuted   = false;
  m_filterPassed = true;
}

bool Algorithm::isEnabled() const {
  return m_isEnabled;
}

bool Algorithm::filterPassed() const {
  return m_filterPassed;
}

void Algorithm::setFilterPassed( bool state ) {
  m_filterPassed = state;
}

std::vector<Algorithm*>* Algorithm::subAlgorithms( ) const {
  return m_subAlgms;
}

void Algorithm::setOutputLevel( int level ) {
  if ( 0 != msgSvc() )
  {
    if ( MSG::NIL != level )
    { msgSvc()->setOutputLevel( name(), level ) ; }
    m_outputLevel = msgSvc()->outputLevel( name() );
  }
}

IAuditorSvc* Algorithm::auditorSvc() const {
  if ( 0 == m_pAuditorSvc ) {
    StatusCode sc = service( "AuditorSvc", m_pAuditorSvc, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [AuditorSvc] not found", name(), sc);
    }
  }
  return m_pAuditorSvc;
}

IChronoStatSvc* Algorithm::chronoSvc() const {
  if ( 0 == m_CSS ) {
    StatusCode sc = service( "ChronoStatSvc", m_CSS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [ChronoStatSvc] not found", name(), sc);
    }
  }
  return m_CSS;
}

// Obsoleted name, kept due to the backwards compatibility
IChronoStatSvc* Algorithm::chronoStatService() const {
  return chronoSvc();
}

IDataProviderSvc* Algorithm::detSvc() const {
  if ( 0 == m_DDS ) {
    StatusCode sc = service( "DetectorDataSvc", m_DDS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [DetectorDataSvc] not found", name(), sc);
    }
  }
  return m_DDS;
}
// Obsoleted name, kept due to the backwards compatibility
IDataProviderSvc* Algorithm::detDataService() const {
  return detSvc();
}

IConversionSvc* Algorithm::detCnvSvc() const {
  if ( 0 == m_DCS ) {
    StatusCode sc = service( "DetectorPersistencySvc", m_DCS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [DetectorPersistencySvc] not found",
                           name(), sc);
    }
  }
  return m_DCS;
}
// Obsoleted name, kept due to the backwards compatibility
IConversionSvc* Algorithm::detDataCnvService() const {
  return detCnvSvc();
}

IDataProviderSvc* Algorithm::eventSvc() const {
  if ( 0 == m_EDS ) {
    StatusCode sc = service( "EventDataSvc", m_EDS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [EventDataSvc] not found", name(), sc);
    }
  }
  return m_EDS;
}
// Obsoleted name, kept due to the backwards compatibility
IDataProviderSvc* Algorithm::eventDataService() const {
  return eventSvc();
}

IConversionSvc* Algorithm::eventCnvSvc() const {
  if ( 0 == m_ECS ) {
    StatusCode sc = service( "EventPersistencySvc", m_ECS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [EventPersistencySvc] not found",
                           name(), sc);
    }
  }
  return m_ECS;
}
// Obsoleted name, kept due to the backwards compatibility
IConversionSvc* Algorithm::eventDataCnvService() const {
  return eventCnvSvc();
}

IHistogramSvc* Algorithm::histoSvc() const {
  if ( 0 == m_HDS ) {
    StatusCode sc = service( "HistogramDataSvc", m_HDS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [HistogramDataSvc] not found", name(), sc);
    }
  }
  return m_HDS;
}
// Obsoleted name, kept due to the backwards compatibility
IHistogramSvc* Algorithm::histogramDataService() const {
  return histoSvc();
}

IExceptionSvc* Algorithm::exceptionSvc() const {
  if ( 0 == m_EXS ) {
    StatusCode sc = service( "ExceptionSvc", m_EXS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [ExceptionSvc] not found", name(), sc);
    }
  }
  return m_EXS;
}

IMessageSvc* Algorithm::msgSvc() const {
  if ( 0 == m_MS ) {
    //can not use service() method (infinite recursion!)
    StatusCode sc = serviceLocator()->service( "MessageSvc", m_MS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [MessageSvc] not found", name(), sc);
    }
  }
  return m_MS;
}

// Obsoleted name, kept due to the backwards compatibility
IMessageSvc* Algorithm::messageService() const {
  return msgSvc();
}

INTupleSvc* Algorithm::ntupleSvc() const {
  if ( 0 == m_NTS ) {
    StatusCode sc = service( "NTupleSvc", m_NTS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [NTupleSvc] not found", name(), sc);
    }
  }
  return m_NTS;
}

// Obsoleted name, kept due to the backwards compatibility
INTupleSvc* Algorithm::ntupleService() const {
  return ntupleSvc();
}

// AIDATupleSvc:
// IAIDATupleSvc* Algorithm::atupleSvc() const {
//   if ( 0 == m_ATS ) {
//     StatusCode sc = service( "AIDATupleSvc", m_ATS, true );
//     if( sc.isFailure() ) {
//       throw GaudiException("Service [AIDATupleSvc] not found", name(), sc);
//     }
//   }
//   return m_ATS;
// }


IRndmGenSvc* Algorithm::randSvc() const {
  if ( 0 == m_RGS ) {
    StatusCode sc = service( "RndmGenSvc", m_RGS, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [RndmGenSvc] not found", name(), sc);
    }
  }
  return m_RGS;
}

IToolSvc* Algorithm::toolSvc() const {
  if ( 0 == m_ptoolSvc ) {
    StatusCode sc = service( "ToolSvc", m_ptoolSvc, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [ToolSvc] not found", name(), sc);
    }
  }
  return m_ptoolSvc;
}

ISvcLocator * Algorithm::serviceLocator() const {
  m_pSvcLocator->setCaller( const_cast<std::string&>(name()) );
  return m_pSvcLocator;
}

// IInterface implementation
unsigned long Algorithm::addRef() {
  return ++m_refCount;
}

unsigned long Algorithm::release() {
  long count = --m_refCount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

// Retrieve the interface requested and check its version compatibility
StatusCode Algorithm::queryInterface
( const InterfaceID& iid    ,
  void**             ppISvc )
{
  if ( 0 == ppISvc ) { return StatusCode::FAILURE ; }   // RETURN
  //
  if      ( IAlgorithm ::interfaceID()      . versionMatch ( iid ) )
  { *ppISvc = static_cast<IAlgorithm*>      ( this ) ; }
  else if ( IProperty  ::interfaceID()      . versionMatch ( iid ) )
  { *ppISvc = static_cast<IProperty*>       ( this ) ; }
  else if ( INamedInterface ::interfaceID() . versionMatch ( iid ) )
  { *ppISvc = static_cast<INamedInterface*> ( this ) ; }
  else if ( IInterface ::interfaceID()      . versionMatch ( iid ) )
  { *ppISvc = static_cast<IInterface*>      ( this ) ; }
  else { *ppISvc = 0 ; return StatusCode::FAILURE; }    // RETURN
  // increment the reference
  addRef();

  return StatusCode::SUCCESS;
}

// Use the job options service to set declared properties
StatusCode Algorithm::setProperties() {
  if( 0 != m_pSvcLocator )    {
    IJobOptionsSvc* jos = 0;
    StatusCode sc = m_pSvcLocator->getService
      ( "JobOptionsSvc", IID_IJobOptionsSvc, *pp_cast<IInterface>(&jos) );
    if( sc.isSuccess() )    {
      // set first generic Properties
      sc = jos->setMyProperties( getGaudiThreadGenericName(name()), this );
      if( sc.isFailure() ) return StatusCode::FAILURE;

      // set specific Properties
      if (isGaudiThreaded(name())) {
        if(jos->setMyProperties( name(), this ).isFailure()) {
          return StatusCode::FAILURE;
        }
      }
      jos->release();
      return sc;
    }
  }
  return StatusCode::FAILURE;
}

StatusCode Algorithm::createSubAlgorithm(const std::string& type,
                                         const std::string& name,
                                         Algorithm*& pSubAlgorithm) {
  if( 0 == m_pSvcLocator ) return StatusCode::FAILURE;

  IAlgManager* am = 0;
  IAlgManager** ptr = &am;
  StatusCode sc = m_pSvcLocator->getService
    ( "", IID_IAlgManager,(IInterface*&)*ptr );
  if( sc.isFailure() ) return StatusCode::FAILURE;

  // Maybe modify the AppMgr interface to return Algorithm* ??
  IAlgorithm *tmp;
  sc = am->createAlgorithm
    (type, name+getGaudiThreadIDfromName(Algorithm::name()), tmp);
  if( sc.isFailure() ) return StatusCode::FAILURE;
  am->release();

  try{
    pSubAlgorithm = dynamic_cast<Algorithm*>(tmp);
    m_subAlgms->push_back(pSubAlgorithm);
  } catch(...){
    sc = StatusCode::FAILURE;
  }
  return sc;
}

// IProperty implementation
// Delegate to the Property manager
StatusCode Algorithm::setProperty(const Property& p) {
  return m_propertyMgr->setProperty(p);
}
StatusCode Algorithm::setProperty(const std::string& s) {
  return m_propertyMgr->setProperty(s);
}
StatusCode Algorithm::setProperty(const std::string& n, const std::string& v) {
  return m_propertyMgr->setProperty(n,v);
}
StatusCode Algorithm::getProperty(Property* p) const {
  return m_propertyMgr->getProperty(p);
}
const Property& Algorithm::getProperty( const std::string& name) const{
  return m_propertyMgr->getProperty(name);
}
StatusCode Algorithm::getProperty(const std::string& n, std::string& v ) const {
  return m_propertyMgr->getProperty(n,v);
}
const std::vector<Property*>& Algorithm::getProperties( ) const {
  return m_propertyMgr->getProperties();
}

/**
 ** Protected Member Functions
 **/

bool Algorithm::isInitialized( ) const {
  return m_isInitialized;
}

void Algorithm::setInitialized( ) {
  m_isInitialized = true;
}

bool
Algorithm::isFinalized( ) const
{
  return m_isFinalized;
}

void
Algorithm::setFinalized( )
{
  m_isFinalized = true;
}


void 
Algorithm::initOutputLevel(Property& /*prop*/) 
{
  // do nothing... yet ?
}




StatusCode
Algorithm::service_i(const std::string& svcName,
                     bool createIf,
                     const InterfaceID& iid,
                     void** ppSvc) const {
  MsgStream log(msgSvc(), name());
  ServiceLocatorHelper helper(*serviceLocator(), log, name());
  return helper.getService(svcName, createIf, iid, ppSvc);
}

StatusCode
Algorithm::service_i(const std::string& svcType,
                     const std::string& svcName,
                     const InterfaceID& iid,
                     void** ppSvc) const {

  MsgStream log(msgSvc(), name());
  ServiceLocatorHelper helper(*serviceLocator(), log, name());
  return  helper.createService(svcType, svcName, iid, ppSvc);
}
