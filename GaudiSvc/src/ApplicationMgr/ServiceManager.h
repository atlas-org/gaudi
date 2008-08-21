// $Id: ServiceManager.h,v 1.7.4.3 2008/08/21 00:09:08 leggett Exp $
#ifndef GAUDISVC_ServiceManager_H
#define GAUDISVC_ServiceManager_H

// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Property.h"
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <map>

// Forward declarations
class IService;
class ISvcFactory;
class IMessageSvc;

/** @class ServiceManager ServiceManager.h

    The ServiceManager class is in charge of the cretion of concrete
    instances of Services. The ApplicationMgr delegates the creation and 
    bookkeeping of services to the ServiceManager. In order to 
    be able to create services from which it is not know the concrete
    type it requires that the services has been declared in one of 
    3 possible ways: an abstract static creator function, a dynamic
    link library or an abstract factory reference. 

    @author Pere Mato
*/
class ServiceManager : virtual public ISvcManager,
                       virtual public ISvcLocator {
  public:
  // typedefs and classes
  typedef std::list<IService*> ListSvc;
  typedef std::list<std::pair<IService*,int> > PListSvc;
  typedef std::map<std::string, std::string > MapType;
  typedef std::map<std::string, ISvcFactory*> MapFactory;

  /// default creator
  ServiceManager(IInterface* iface);
  /// virtual destructor
  virtual ~ServiceManager();

  /// implmentation of IInterface::addRef
  virtual unsigned long addRef();
  /// implmentation of IInterface::release
  virtual unsigned long release();
  /// implementation of IInterface::queryInterface
  virtual StatusCode queryInterface(const InterfaceID& iid, void** pinterface);

  /// implementation of ISvcLocation::getService
  virtual StatusCode getService( const std::string& name, IService*& svc);
  /// implementation of ISvcLocation::getService
  virtual StatusCode getService( const std::string& name, const InterfaceID& iid, IInterface*& pinterface);
  /// implementation of ISvcLocation::getService
  virtual StatusCode getService( const std::string& name, IService*& svc, bool createIf);
  /// Return the list of Services
  virtual const std::list<IService*>& getServices( ) const;
  virtual std::list<IService*> getActiveServices( ) const;
  /// implementation of ISvcLocation::existsService
  virtual bool existsService( const std::string& name) const;

  /// implementation of ISvcManager::addService
  virtual StatusCode addService( IService* svc, int prio );
  /// implementation of ISvcManager::addService
  virtual StatusCode addService( const std::string& typ, const std::string& nam, int prio );
  /// implementation of ISvcManager::removeService
  virtual StatusCode removeService( IService* svc);
  /// implementation of ISvcManager::removeService
  virtual StatusCode removeService( const std::string& nam );
  /// implementation of ISvcManager::declareSvcFactory
  virtual StatusCode declareSvcFactory( const ISvcFactory& factory, const std::string& svctype );
  /// implementation of ISvcManager::declareSvcType
  virtual StatusCode declareSvcType( const std::string& svcname, const std::string& svctype );
  /// implementation of ISvcManager::createService
  virtual StatusCode createService( const std::string& svctype, const std::string& svcname, IService*& svc);
  /// implementation of IAlgManager::getFactory
  virtual StatusCode getFactory(const std::string& svctype, const ISvcFactory*& fac) const;
  /// implementation of ISvcManager::initializeServices
  virtual StatusCode initializeServices();
  /// implementation of ISvcManager::finalizeServices
  virtual StatusCode finalizeServices();
  /// implementation of ISvcManager::reinitializeServices
  virtual StatusCode reinitializeServices();

  /// manage priorties of services
  virtual int getPriority(const std::string& name) const;
  virtual StatusCode setPriority(const std::string& name, int pri);

  virtual void setCaller( std::string& name ) { m_caller = name; }

  void loopCheckHandler(Property&);

private:
  /// access the message service
  IMessageSvc* msgSvc();
  /// create a service and initialize it
  StatusCode makeService( const std::string& name, IService*& svc);
  StatusCode removeActiveService( IService* svc);
  StatusCode removeListService( IService* svc);
  void logCreation(const std::string&, const std::string&);
  void logGet(const std::string&);

private:
  unsigned long m_refcount;    ///< Reference counter
  ListSvc       m_listsvc;     ///< List of service maintained by ServiceManager
  PListSvc      m_activesvc;   ///< List of "active" service maintained by ServiceManager
  MapType       m_maptype;     ///< Map of service name and service type
  MapFactory    m_mapfactory;  ///< Map of pairs of factory reference and service name
  IInterface*   m_pOuter;      ///< Interface hub reference (ApplicationMgr)
  ISvcLocator*  m_svclocator;  ///< Service locator
  IMessageSvc*  m_msgsvc;      ///< Pointer to the message service if it exists

  bool          m_loopCheck;   ///< Check for service init loops
  std::vector<std::string> m_loopIgnore;  ///< list of services to ignore
  std::string&  m_caller;      ///< name of caller
  bool          m_savelog;     ///< save log of caller tree
  std::ostringstream m_tmpLog; ///< temporary stream for initial service calls
  bool          m_init;
  std::ofstream m_ofs;         ///< where to write caller tree

};
#endif  // GAUDISVC_ServiceManager_H

