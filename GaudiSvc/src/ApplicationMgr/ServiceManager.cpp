// $Id: ServiceManager.cpp,v 1.27 2008/11/10 15:29:09 marcocle Exp $

// Include files
#include "ServiceManager.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/Service.h"

#include <iostream>
#include <cassert>

using ROOT::Reflex::PluginService;
// constructor
ServiceManager::ServiceManager(IInterface* iface):
  m_refcount(1), m_pOuter(iface), m_msgsvc(0),  m_statemgr(iface),
  m_loopCheck(true) {
  m_svclocator = (ISvcLocator*)this;

}

// destructor
ServiceManager::~ServiceManager() {
  if( m_msgsvc ) m_msgsvc->release();
  //-- inform the orphan services that I am gone....
  for (ListSvc::const_iterator it = m_listsvc.begin(); it != m_listsvc.end(); it++ ) {
    (*it)->setServiceManager(0);
  }
}

// addRef
unsigned long ServiceManager::addRef() {
  m_refcount++;
  return m_refcount;
}

// release
unsigned long ServiceManager::release() {
  unsigned long count = --m_refcount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::queryInterface(const InterfaceID& iid, void** pinterface)
//------------------------------------------------------------------------------
{
  if( iid == IID_IInterface ) {
    *pinterface = (IInterface*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  else if ( iid == IID_ISvcLocator ) {
    *pinterface = (ISvcLocator*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  else if ( iid == IID_ISvcManager ) {
    *pinterface = (ISvcManager*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  else {
    return m_pOuter->queryInterface(iid, pinterface);
  }
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::getService( const std::string& name, IService*& svc)
//------------------------------------------------------------------------------
{
  return getService(name, svc, true);
}
//------------------------------------------------------------------------------
StatusCode ServiceManager::getService( const std::string& name, const InterfaceID& iid,
                                       IInterface*& pinterface)
//------------------------------------------------------------------------------
{
  IService*  svc;
  StatusCode status = getService( name, svc, false );
  if( status.isSuccess() ) {
    // Service found. So now get the right interface
    status = svc->queryInterface( iid, (void**)&pinterface );
  }
  return status;
}
//------------------------------------------------------------------------------
StatusCode ServiceManager::makeService( const std::string& nam,
                                        IService*& svc )
//------------------------------------------------------------------------------
{
  // find out if name is registered with a type
  std::string name = nam;
  std::string type;
  MapType::iterator it = m_maptype.find( nam );
  if( it != m_maptype.end() ) {
    type = (*it).second;
  } else {
    // otherwise parse the name to see if it is of the format "type/name"
    ListItem item(name);
    type = item.type();
    name = item.name();
  }
  std::string::size_type ip;
  if ( (ip = type.find("__")) != std::string::npos) {
    type.erase(ip,type.length());
  }
  StatusCode sc = createService( type, name, svc );
  if( sc.isSuccess() ) {
    // Bring the created service to the same state in which the ApplicationMgr
    // will be.
    if (m_statemgr->targetFSMState() >= Gaudi::StateMachine::INITIALIZED) {
      sc = svc->sysInitialize();
      if (sc.isSuccess() && m_statemgr->targetFSMState() >= Gaudi::StateMachine::RUNNING) {
        sc = svc->sysStart();
      }
    }

    if( sc.isSuccess() ) {
      sc = addService( svc, 10);
    } else {
      MsgStream log(msgSvc(), "ServiceManager");
      log << MSG::ERROR << "Unable to initialize service \"" << name << "\""
          << endreq;
      removeService(svc).ignore(); //if init fails remove it from the list
      delete svc;
      svc = 0;
    }
    if (svc!= 0) svc->setServiceManager( this );
  }
  return sc;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::getService( const std::string& nam, IService*& svc,
                                       bool createIf)
//------------------------------------------------------------------------------
{
  ListItem item(nam);
  const std::string& name = item.name();

  StatusCode sc(StatusCode::FAILURE);
  ListSvc::iterator it(m_listsvc.begin());
  for (; it != m_listsvc.end(); ++it ) {
    if( (*it)->name() == name ) {
      svc = *it;
      break;
    }
  }

  if (it !=  m_listsvc.end()) {
    if (m_loopCheck &&
        (createIf && (*it)->FSMState() == Gaudi::StateMachine::CONFIGURED)) {
      MsgStream log(msgSvc(), "ServiceManager");
      std::ostringstream ost;
      ost << "Initialization loop detected when creating service \"" << name
	  << "\"" << std::endl;

      const size_t depth(99);
      void* addresses[depth];
      std::string lib,rlib,fnc;
      void* addr(0);
      
      ost << "Printing stack trace:" << std::endl;
      if ( addresses && System::backTrace(addresses, depth)) {      
	int i=1;
	while (System::getStackLevel(addresses[i], addr, fnc, lib)) {
	  if ( (lib.find("libGaudiSvc.so") == std::string::npos &&
		lib.find("libGaudiKernel.so") == std::string::npos &&
		fnc.find("::service<IService>(") == std::string::npos &&
		fnc.find("__") != 0) || log.level() <= MSG::DEBUG )
	    {
	      int i1 = lib.rfind("/",lib.length());
	      rlib = lib.substr(i1+1,lib.length()-i1-1);
	      ost << "  " << fnc << "  [" << rlib << "]" << std::endl;	    
	    }
	  ++i;
	}
      }


      if ( find(m_loopIgnore.begin(), m_loopIgnore.end(), name) == m_loopIgnore.end() ) {
	log << MSG::ERROR << ost.str() << endreq;

	sc = StatusCode::FAILURE;
      } else {
	log << MSG::DEBUG << ost.str() << endreq;

	sc = StatusCode::SUCCESS;
      }
    } else {      
      sc = StatusCode::SUCCESS;
    }
  } else {
    // Service not found. The user may be interested in one of the interfaces
    // of the application manager itself
    if( name == "ApplicationMgr" ||
        name == "APPMGR" ||
        name == "" ) {
      sc = m_svclocator->queryInterface( IService::interfaceID(), (void**)&svc );
      if( svc ) svc->release(); // Do not increase the reference count
    } else if ( createIf ){
      //last resort: we try to create the service
      if ( item.name() != item.type() ) {
        // if we were asked for a "ServiceType/ServiceName", we pass the string
        // directly to makeService
        sc = makeService(nam, svc);
      } else {
        // the user did not specified both type and name
        sc = makeService(name, svc);
      }
    }
  }

  return sc;
}

//------------------------------------------------------------------------------
const std::list<IService*>& ServiceManager::getServices( ) const
//------------------------------------------------------------------------------
{
  return m_listsvc;
}

//------------------------------------------------------------------------------
std::list<IService*> ServiceManager::getActiveServices( ) const
//------------------------------------------------------------------------------
{
  std::list<IService*> asvcs;
  for (PListSvc::const_iterator itr = m_activesvc.begin();
       itr != m_activesvc.end(); ++itr) {
    asvcs.push_back( itr->first );
  }
  return asvcs;
}


//------------------------------------------------------------------------------
bool ServiceManager::existsService( const std::string& name) const
//------------------------------------------------------------------------------
{
  ListSvc::const_iterator it;
  for (it = m_listsvc.begin(); it != m_listsvc.end(); it++ ) {
    if( name == (*it)->name() ) {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::addService( IService* svc, int prio )
//------------------------------------------------------------------------------
{
  PListSvc::iterator it;
  // remove it if already in the list
  removeActiveService( svc ).ignore();
  // insert in the right place in the list
  for( it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    if( (*it).second > prio )  break;
  }
  m_activesvc.insert(it, std::make_pair(svc, prio));
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::addService( const std::string& type,
                                       const std::string& name, int prio )
//------------------------------------------------------------------------------
{
  StatusCode sc;
  IService* svc;
  if( existsService(name) ) {
    sc = getService(name, svc);
  } else {
    if( type.length() == 0 ) {
      // find out if name is registered with a type
      MapType::iterator it = m_maptype.find( name );
      if( it != m_maptype.end() ) {
        sc = createService( (*it).second, name, svc );
      } else {
        sc = createService( name, name, svc );
      }
    } else {
      sc = createService( type, name, svc );
    }

    if (sc.isSuccess() && m_statemgr->targetFSMState() >= Gaudi::StateMachine::INITIALIZED) {
      sc = svc->sysInitialize();
      if (sc.isSuccess() && m_statemgr->targetFSMState() >= Gaudi::StateMachine::RUNNING) {
        sc = svc->sysStart();
      }
    }

  }
  if( sc.isSuccess() ) {
    return addService( svc, prio );
  }
  else {
    return sc;
  }
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::removeService( IService* svc)
//------------------------------------------------------------------------------
{

  removeActiveService(svc).ignore();
  removeListService(svc).ignore();

  return StatusCode(StatusCode::SUCCESS,true);
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::removeActiveService( IService* svc)
//------------------------------------------------------------------------------
{
  PListSvc::iterator it;
  for (it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    if( (*it).first == svc ) {
      m_activesvc.erase(it);
      break;
    }
  }

  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::removeListService( IService* svc)
//------------------------------------------------------------------------------
{

  ListSvc::iterator ll;
  for ( ll = m_listsvc.begin(); ll != m_listsvc.end(); ++ll ) {
    if ( (*ll) == svc  ) {
      m_listsvc.erase( ll );
      break;
    }
  }

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode ServiceManager::removeService( const std::string& nam )
//------------------------------------------------------------------------------
{
  IService* svc;
  if( getService(nam, svc).isSuccess() ) return removeService(svc);
  else return StatusCode::FAILURE;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::declareSvcFactory( const ISvcFactory& factory,
                                              const std::string& svctype )
//------------------------------------------------------------------------------
{

  std::pair<MapFactory::iterator, bool> p;
  p = m_mapfactory.insert(MapFactory::value_type( svctype, (ISvcFactory*)&factory) );
  if( p.second == false) {
    MsgStream log(msgSvc(), "ServiceManager");
    log << MSG::WARNING << "Service factory for type " << svctype << " already declared" << endreq;
    m_mapfactory.erase ( p.first );
    p = m_mapfactory.insert(MapFactory::value_type( svctype, (ISvcFactory*)&factory) );
    if( p.second == false) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::declareSvcType( const std::string& svcname,
                                           const std::string& svctype )
//------------------------------------------------------------------------------
{
  std::pair<MapType::iterator, bool> p = m_maptype.insert(std::make_pair(svcname, svctype));
  if( p.second == false) {
    m_maptype.erase ( p.first );
    p = m_maptype.insert(std::make_pair(svcname, svctype) );
    if( p.second == false) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::initializeServices()
//------------------------------------------------------------------------------
{
  StatusCode sc;
  MsgStream log(msgSvc(), "ServiceManager");

  // call initialize() for all services
  for (PListSvc::iterator it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    std::string name = (*it).first->name();
    switch ((*it).first->FSMState()) {
    case Gaudi::StateMachine::INITIALIZED:
      log << MSG::DEBUG << "Service " << name
          << " already initialized" << endreq;
      break;
    case Gaudi::StateMachine::OFFLINE:
      log << MSG::DEBUG << "Initializing service " << name << endreq;
      sc = (*it).first->sysInitialize();
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Unable to initialize Service: " << (*it).first->name() << endreq;
        return sc;
      } break;
    default:
      log << MSG::ERROR << "Service " << name
          << " not in the correct state to be initialized ("
          << (*it).first->FSMState() << ")" << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::startServices()
//------------------------------------------------------------------------------
{
  StatusCode sc = StatusCode(StatusCode::FAILURE,true);
  MsgStream log(msgSvc(), "ServiceManager");

  // call initialize() for all services
  for (PListSvc::iterator it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    std::string name = (*it).first->name();
    switch ((*it).first->FSMState()) {
    case Gaudi::StateMachine::RUNNING:
      log << MSG::DEBUG << "Service " << name
          << " already started" << endreq;
      break;
    case Gaudi::StateMachine::INITIALIZED:
      log << MSG::DEBUG << "Starting service " << name << endreq;
      sc = (*it).first->sysStart();
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Unable to start Service: " << (*it).first->name() << endreq;
        return sc;
      } break;
    default:
      log << MSG::ERROR << "Service " << name
          << " not in the correct state to be started ("
          << (*it).first->FSMState() << ")" << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode ServiceManager::stopServices()
//------------------------------------------------------------------------------
{
  StatusCode sc;
  MsgStream log(msgSvc(), "ServiceManager");

  PListSvc::reverse_iterator it;
  // call stop() for all services
  for ( it = m_activesvc.rbegin(); it != m_activesvc.rend(); it++ ) {
    std::string name = (*it).first->name();
    switch ((*it).first->FSMState()) {
    case Gaudi::StateMachine::INITIALIZED:
      log << MSG::DEBUG << "Service " << name
          << " already stopped" << endreq;
      break;
    case Gaudi::StateMachine::RUNNING:
      log << MSG::DEBUG << "Stopping service " << name << endreq;
      sc = (*it).first->sysStop();
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Unable to stop Service: " << (*it).first->name() << endreq;
        return sc;
      } break;
    default:
      log << MSG::ERROR << "Service " << name
          << " not in the correct state to be stopped ("
          << (*it).first->FSMState() << ")" << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::reinitializeServices()
//------------------------------------------------------------------------------
{
  StatusCode sc;

  PListSvc::iterator it;
  // Re-Initialize all services
  for ( it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    sc = (*it).first->sysReinitialize();
    if( !sc.isSuccess() ) {
      MsgStream log(msgSvc(), "ServiceManager");
      log << MSG::ERROR << "Unable to re-initialize Service: " << (*it).first->name() << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::restartServices()
//------------------------------------------------------------------------------
{
  StatusCode sc;

  PListSvc::iterator it;
  // Re-Start all services
  for ( it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    sc = (*it).first->sysRestart();
    if( !sc.isSuccess() ) {
      MsgStream log(msgSvc(), "ServiceManager");
      log << MSG::ERROR << "Unable to re-start Service: " << (*it).first->name() << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
StatusCode ServiceManager::finalizeServices()
//------------------------------------------------------------------------------
{
  MsgStream log(msgSvc(), "ServiceManager");

  PListSvc::reverse_iterator its;

  StatusCode sc(StatusCode::SUCCESS);

  // make sure that HistogramDataSvc and THistSvc get finalized after the
  // ToolSvc, and StatusCodeSvc after that
  int pri_tool = getPriority("ToolSvc");
  if (pri_tool != 0) {
    setPriority("THistSvc",pri_tool-1).ignore();
    setPriority("ChronoStatSvc",pri_tool-2).ignore();
    setPriority("AuditorSvc",pri_tool-3).ignore();
    setPriority("HistogramDataSvc",pri_tool-1).ignore();
    // Preserve the relative ordering between HistogramDataSvc and HistogramPersistencySvc
    setPriority("HistogramPersistencySvc",pri_tool-2).ignore();
  }

  // make sure the StatusCodeSvc gets finalized really late:
  setPriority("StatusCodeSvc",-9999).ignore();

  // call finalize() for all services in reverse order
  for ( its = m_activesvc.rbegin(); its != m_activesvc.rend(); its++ ) {
    // ignore the current state for the moment
    // if( Gaudi::StateMachine::INITIALIZED == (*it).first->state() ) {
    log << MSG::DEBUG << "Finalizing service " << (*its).first->name() << endreq;
    if ( !((*its).first->sysFinalize()).isSuccess() ) {
      log << MSG::WARNING << "Finalization of service " << (*its).first->name()
      << " failed" << endreq;
      sc = StatusCode::FAILURE;
    }
  }

  // loop over all Active Services, removing them one by one.
  while (m_activesvc.size() != 0) {
    PListSvc::iterator its = m_activesvc.end();
    its --;

    //// release for all services
    //   for ( its = m_activesvc.rbegin(); its != m_activesvc.rend(); its++ ) {
    // erase from m_listsvc before releasing
    for ( ListSvc::iterator ll = m_listsvc.begin(); ll != m_listsvc.end(); ++ll ) {
      if ( (*ll)->name() == its->first->name() ) {
        m_listsvc.erase( ll );
        break;
      }
    }

    // this may destroy the service
    while ((*its).first->release()) {}
  }

  // clear the list of active services
  m_activesvc.erase(m_activesvc.begin(), m_activesvc.end() );

  return sc ;
}


//------------------------------------------------------------------------------
StatusCode ServiceManager::createService( const std::string& svctype,
                                          const std::string& svcname,
                                          IService*& service )
//------------------------------------------------------------------------------
{
  // Check is the service is already existing
  if( existsService( svcname ) ) {
    // return an error because a service with that name already exists
    return StatusCode::FAILURE;
  }

  StatusCode rc = StatusCode::FAILURE;
  rc.setChecked(); //hack to avoid going into infinite recursion on ~StatusCode

  MsgStream log(msgSvc(), "ServiceManager");

  service = PluginService::Create<IService*>(svctype, svcname, m_svclocator);
  if ( !service ) {
    service = PluginService::CreateWithId<IService*>(svctype, svcname, m_svclocator);
  }

  if ( service ) {
    m_listsvc.push_back( service );
    // Check the compatibility of the version of the interface obtained
    if( !isValidInterface(service) ) {
      log << MSG::FATAL << "Incompatible interface IService version for " << svctype << endreq;
      return StatusCode::FAILURE;
    }
    service->setServiceManager(this);
    return StatusCode(StatusCode::SUCCESS,true);
  }
  log << MSG::FATAL << "No Service factory for " << svctype << " available." << endreq;
  return StatusCode::FAILURE;
}

/// Access to factory by name
StatusCode
ServiceManager::getFactory(const std::string& svctype,const ISvcFactory*& factory) const
{
  MapFactory::const_iterator itf = m_mapfactory.find( svctype );
  factory = (itf == m_mapfactory.end()) ? 0 : (*itf).second;
  return (factory==0) ? StatusCode::FAILURE : StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
IMessageSvc* ServiceManager::msgSvc()
//------------------------------------------------------------------------------
{
  if( m_msgsvc == 0 ) {
    IService* iSvc(0);
    if ( (getService( "MessageSvc", iSvc, false)).isSuccess() )
      m_msgsvc = dynamic_cast<IMessageSvc*>(iSvc);
  }
  return m_msgsvc;
}

//------------------------------------------------------------------------------
int
ServiceManager::getPriority(const std::string& name) const {
//------------------------------------------------------------------------------

  PListSvc::const_iterator it;
  for (it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    if ((*it).first->name() == name) {
      return (*it).second;
    }
  }

  return 0;

}

//------------------------------------------------------------------------------
StatusCode
ServiceManager::setPriority(const std::string& name, int prio) {
//------------------------------------------------------------------------------

  PListSvc::const_iterator it;
  for (it = m_activesvc.begin(); it != m_activesvc.end(); it++ ) {
    if ((*it).first->name() == name) {
      IService *svc = (*it).first;
      removeActiveService( svc ).ignore();
      return addService(svc,prio);
    }
  }

  return StatusCode::FAILURE;

}

//------------------------------------------------------------------------------
// Get the value of the initialization loop check flag.
//------------------------------------------------------------------------------
bool ServiceManager::loopCheckEnabled() const {
  return m_loopCheck;
}
//------------------------------------------------------------------------------
// Set the value of the initialization loop check flag.
//------------------------------------------------------------------------------
void ServiceManager::setLoopCheckEnabled(bool en) {
  m_loopCheck = en;
}
//------------------------------------------------------------------------------
// Set the value of the initialization loop ignore list.
//------------------------------------------------------------------------------
void ServiceManager::setLoopCheckIgnore(const std::vector<std::string>& vec) {
  m_loopIgnore = vec;
}
