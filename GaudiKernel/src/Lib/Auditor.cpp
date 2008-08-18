// $Id: Auditor.cpp,v 1.16 2008/04/03 14:40:19 marcocle Exp $
 
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/Auditor.h"


#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/GaudiException.h" 


// Constructor
Auditor::Auditor( const std::string& name, ISvcLocator *pSvcLocator )
: m_refCount(0),
  m_name(name),
  m_MS (0),
  m_pSvcLocator(pSvcLocator),
  m_isEnabled(true),
  m_isInitialized(false),
  m_isFinalized(false)
{
  m_PropertyMgr = new PropertyMgr();
  
  // Declare common Auditor properties with their defaults
  declareProperty( "OutputLevel", m_outputLevel = MSG::NIL);
  declareProperty( "Enable", m_isEnabled = true);
  
}

// Default Destructor
Auditor::~Auditor() {
  delete m_PropertyMgr;
  if( m_MS ) m_MS->release();
}

// IAuditor implementation
StatusCode Auditor::sysInitialize() {
  StatusCode sc;

  // Bypass the initialization if the auditor is disabled or
  // has already been initialized.
  if ( isEnabled( ) && ! m_isInitialized ) {

    // Setup the default service ... this should be upgraded so as to be configurable.
    if( 0 == m_pSvcLocator )
      return StatusCode::FAILURE;

    // Set up message service
    sc = m_pSvcLocator->getService("MessageSvc", IID_IMessageSvc, *pp_cast<IInterface>(&m_MS));
    if( !sc.isSuccess() )  return StatusCode::FAILURE;

    // Set the Auditor's properties
    sc = setProperties();
    if( !sc.isSuccess() )  return StatusCode::FAILURE;

    // Check current outputLevel to evetually inform the MessagsSvc
    if( m_outputLevel != MSG::NIL ) {
      setOutputLevel( m_outputLevel );
    }

    {
      try{
        // Invoke the initialize() method of the derived class
	sc = initialize();
	if( !sc.isSuccess() )  return StatusCode::FAILURE;
        m_isInitialized = true;

        return sc;                                               /// RETURN !!!
      }
      catch ( const GaudiException& Exception )                  /// catch Gaudi Exception
        {
          /// (1) perform the printout of message
	  MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
	  log << MSG::FATAL << " Exception with tag=" << Exception.tag() << " is catched " << endreq;
          /// (2) print  the exception itself (NB!  - GaudiException is a linked list of all "previous exceptions")
	  MsgStream logEx ( msgSvc() , Exception.tag() );
	  logEx << MSG::ERROR << Exception  << endreq;
        }
      catch( const std::exception& Exception )                   /// catch std::exception
        {
	  /// (1) perform the printout of message
	  MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
	  log << MSG::FATAL << " Standard std::exception is catched " << endreq;
	  /// (2) print  the exception itself (NB!  - GaudiException is a linked list of all "previous exceptions")
	  MsgStream logEx ( msgSvc() , name() + "*std::exception*" );
	  logEx << MSG::ERROR << Exception.what()  << endreq;
        }
      catch(...)
        {
	  /// (1) perform the printout
	  MsgStream log ( msgSvc() , name() + ".sysInitialize()" );
	  log << MSG::FATAL << " UNKNOWN Exception is  catched " << endreq;
        }
    }
  }
  ///
  return StatusCode::FAILURE;
}

StatusCode Auditor::initialize() {
  return StatusCode::SUCCESS;
}

// Implemented for backward compatibility 
void Auditor::before(StandardEventType evt, INamedInterface* obj){
  switch (evt) {
  case Initialize:   beforeInitialize(obj);   break;
  case ReInitialize: beforeReinitialize(obj); break;
  case Execute:      beforeExecute(obj);      break;
  case BeginRun:     beforeBeginRun(obj);     break;
  case EndRun:       beforeEndRun(obj);       break;
  case Finalize:     beforeFinalize(obj);     break;
  }
}
void Auditor::before(StandardEventType, const std::string&) {}

void Auditor::before(CustomEventTypeRef, INamedInterface*){}
void Auditor::before(CustomEventTypeRef, const std::string&){}

// Implemented for backward compatibility
void Auditor::after(StandardEventType evt, INamedInterface* obj, const StatusCode& sc){
  switch (evt) {
  case Initialize:   afterInitialize(obj);   break;
  case ReInitialize: afterReinitialize(obj); break;
  case Execute:      afterExecute(obj, sc);  break;
  case BeginRun:     afterBeginRun(obj);     break;
  case EndRun:       afterEndRun(obj);       break;
  case Finalize:     afterFinalize(obj);     break;
  default: ;// do nothing
  }
}
void Auditor::after(StandardEventType, const std::string&, const StatusCode&) {}

void Auditor::after(CustomEventTypeRef, INamedInterface*, const StatusCode&){}
void Auditor::after(CustomEventTypeRef, const std::string&, const StatusCode&){}

void Auditor::beforeInitialize(INamedInterface* ) {}

void Auditor::afterInitialize(INamedInterface* ) {}

void Auditor::beforeReinitialize(INamedInterface* ) {}

void Auditor::afterReinitialize(INamedInterface* ) {}

void Auditor::beforeExecute(INamedInterface* ) {}

void Auditor::afterExecute(INamedInterface*, const StatusCode& ) {}

void Auditor::beforeBeginRun(INamedInterface* ) {}

void Auditor::afterBeginRun(INamedInterface* ) {}

void Auditor::beforeEndRun(INamedInterface* ) {}

void Auditor::afterEndRun(INamedInterface* ) {}

void Auditor::beforeFinalize(INamedInterface* ) {}

void Auditor::afterFinalize(INamedInterface* ) {}

StatusCode Auditor::sysFinalize() {
  StatusCode sc = StatusCode::SUCCESS;
  try{
    // 
    // Invoke the finalize() method of the derived class if
    // it has been initilized.
    if ( m_isInitialized && ! m_isFinalized ) {
      m_isFinalized = true;
      sc = finalize();
      if( !sc.isSuccess() )  return StatusCode::FAILURE;
    }
    return sc;                                               /// RETURN !!!
    //
  } 
  catch( const GaudiException& Exception )                   /// catch GaudiExeption 
    {
      /// (1) perform the printout of message  
      MsgStream log ( msgSvc() , name() + ".sysFinalize()" ); 
      log << MSG::FATAL 
	  << " Exception with tag=" << Exception.tag() << " is catched " << endreq; 
      /// (2) print  the exception itself 
      ///       (NB!  - GaudiException is a linked list of all "previous exceptions") 
      MsgStream logEx ( msgSvc() , Exception.tag() ); 
      logEx << MSG::ERROR 
	    << Exception  << endreq; 
    }
  catch( const std::exception& Exception )                   /// catch std::exception 
    {
      /// (1) perform the printout of message  
      MsgStream log ( msgSvc() , name() + ".sysFinalize()" ); 
      log << MSG::FATAL
	  << " Standard std::exception is catched " << endreq; 
      /// (2) print  the exception itself 
      MsgStream logEx ( msgSvc() , name() + "*std::exception*" ); 
      logEx << MSG::ERROR 
	    << Exception.what()  << endreq; 
    }
  catch( ... )                                                /// catch unknown exception 
    {
      /// (1) perform the printout 
      MsgStream log ( msgSvc() , name() + ".sysFinalize()" ); 
      log << MSG::FATAL 
	  << " UNKNOWN Exception is  catched " << endreq; 
    }
  ///
  return StatusCode::FAILURE ;
}

StatusCode Auditor::finalize() {
  return StatusCode::SUCCESS;
}

const std::string& Auditor::name() const {
  return m_name;
}

const bool Auditor::isEnabled( ) const {
  return m_isEnabled;
}

IMessageSvc* Auditor::msgSvc() const {
  return m_MS;
}

void Auditor::setOutputLevel( int level ) {
  if( m_MS != 0) {
    m_MS->setOutputLevel( name(), level );
  }
}

ISvcLocator * Auditor::serviceLocator() const {
  return m_pSvcLocator;
}


// IInterface implementation
unsigned long Auditor::addRef() {
  return ++m_refCount;
}

unsigned long Auditor::release() {
  long count = --m_refCount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

StatusCode Auditor::queryInterface
( const InterfaceID& riid   , 
  void**             ppISvc ) 
{
  if ( 0 == ppISvc ) { return StatusCode::FAILURE ; } // RETURN 

  if      ( IAuditor        ::interfaceID() . versionMatch ( riid ) ) 
  { *ppISvc = static_cast<IAuditor*>         ( this ) ; }
  else if ( IProperty       ::interfaceID() . versionMatch ( riid ) ) 
  { *ppISvc = static_cast<IProperty*>        ( this ) ; }
  else if ( INamedInterface ::interfaceID() . versionMatch ( riid ) ) 
  { *ppISvc = static_cast<INamedInterface*>  ( this ) ; }
  else if ( IInterface      ::interfaceID() . versionMatch ( riid ) ) 
  { *ppISvc = static_cast<IInterface*>       ( this ) ; }
  else { *ppISvc = 0 ; return StatusCode::FAILURE; } // RETURN 
  // increment the reference counter
  addRef();
  //
  return StatusCode::SUCCESS;
}


// Use the job options service to set declared properties
StatusCode Auditor::setProperties() {
  if( 0 != m_pSvcLocator )    {
    IJobOptionsSvc* jos;
    StatusCode sc = service("JobOptionsSvc", jos);
    if( sc.isSuccess() )    {
      jos->setMyProperties( name(), this ).ignore();
      jos->release();
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

// IProperty implementation
// Delegate to the Property manager
StatusCode Auditor::setProperty(const Property& p) {
	return m_PropertyMgr->setProperty(p);
}
StatusCode Auditor::setProperty(const std::string& s) {
	return m_PropertyMgr->setProperty(s);
}
StatusCode Auditor::setProperty(const std::string& n, const std::string& v) {
	return m_PropertyMgr->setProperty(n,v);
}
StatusCode Auditor::getProperty(Property* p) const {
	return m_PropertyMgr->getProperty(p);
}
const Property& Auditor::getProperty( const std::string& name) const{
	return m_PropertyMgr->getProperty(name);
}
StatusCode Auditor::getProperty(const std::string& n, std::string& v ) const {
	return m_PropertyMgr->getProperty(n,v);
}
const std::vector<Property*>& Auditor::getProperties( ) const {
	return m_PropertyMgr->getProperties();
}
