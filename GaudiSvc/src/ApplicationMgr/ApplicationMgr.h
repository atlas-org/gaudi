// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiSvc/src/ApplicationMgr/ApplicationMgr.h,v 1.34 2008/11/10 15:29:09 marcocle Exp $	//
#ifndef GAUDI_APPLICATIONMGR_H
#define GAUDI_APPLICATIONMGR_H

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IStateful.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/PropertyMgr.h"

// STL include files
#include <list>
#include <vector>

// Forward declarations
class ISvcLocator;
class ServiceManager;
class AlgorithmManager;
class DLLClassManager;

class IMessageSvc;
class IIncidentSvc;
class IRunable;
class IDataManagerSvc;
class IConversionSvc;
class IJobOptionsSvc;

/** @class ApplicationMgr

    The Application Manager class. The main purpose of the Application
    Manager is to steer any data processing application. This includes
    all data processing applications for LHCb data in all stages:
    simulation, reconstruction, analysis, high level triggers, etc.
    Specific implementations of the ApplicationMgr will be developed
    to cope with the different environments (on-line, off-line,
    interactive, batch, etc.).

    @author Pere Mato
*/
class ApplicationMgr : virtual public IAppMgrUI,
                       virtual public IEventProcessor,
                       virtual public IService,
                       virtual public IStateful {
public:
  typedef std::list<std::pair<IService*,int> >  ListSvc;
  typedef std::list<std::string>   ListName;
  typedef std::vector<std::string> VectorName;

public:

  // default creator
  ApplicationMgr(IInterface* = 0);
  // virtual destructor
  virtual ~ApplicationMgr();

  // implmentation of IInterface::addRef
  virtual unsigned long addRef();
  // implmentation of IInterface::release
  virtual unsigned long release();
  // implementation of IInterface::queryInterface
  virtual StatusCode queryInterface(const InterfaceID& iid, void** pinterface);

  // implementation of IAppMgrUI::run
  virtual StatusCode run();
  // implementation of IAppMgrUI::configure
  virtual StatusCode configure();
  // implementation of IAppMgrUI::terminate
  virtual StatusCode terminate();
  // implementation of IAppMgrUI::initalize
  virtual StatusCode initialize();
  // implementation of IAppMgrUI::start
  virtual StatusCode start();
  // implementation of IAppMgrUI::stop
  virtual StatusCode stop();
  // implementation of IAppMgrUI::finalize
  virtual StatusCode finalize();
  // implementation of IAppMgrUI::nextEvent
  virtual StatusCode nextEvent(int maxevt);
  // implementation of IAppMgrUI::name
  virtual const std::string&  name() const;
  /// implementation of IEventProcessor::executeEvent(void*)
  virtual StatusCode executeEvent(void* par);
  /// implementation of IEventProcessor::executeRun(int)
  virtual StatusCode executeRun(int evtmax);
  /// implementation of IEventProcessor::stopRun()
  virtual StatusCode stopRun();

  // implementation of IService::type
  virtual const InterfaceID& type() const;
  // implementation of IStateful::state
  virtual Gaudi::StateMachine::State FSMState() const;
  // implementation of IStateful::targetState
  virtual Gaudi::StateMachine::State targetFSMState() const;
  // implementation of IStateful::reinitilaize
  virtual StatusCode reinitialize();
  // implementation of IStateful::reinitilaize
  virtual StatusCode restart();
  // implementation of IService::sysItinitilaize
  virtual StatusCode sysInitialize() { return StatusCode::SUCCESS; }
  // implementation of IService::sysStart
  virtual StatusCode sysStart() { return StatusCode::SUCCESS; }
  // implementation of IService::sysStop
  virtual StatusCode sysStop() { return StatusCode::SUCCESS; }
  // implementation of IService::sysFinalize
  virtual StatusCode sysFinalize() { return StatusCode::SUCCESS; }
  // implementation of IService::sysReinitialize
  virtual StatusCode sysReinitialize() { return StatusCode::SUCCESS; }
  // implementation of IService::sysRestart
  virtual StatusCode sysRestart() { return StatusCode::SUCCESS; }

  // SI Go Handler
  void       SIGoHandler             ( Property& theProp );
  // SI Exit handler
  void       SIExitHandler           ( Property& theProp );

  /// @name Property handlers
  //@{
  void       evtLoopPropertyHandler  ( Property& theProp );
  StatusCode decodeExtSvcNameList    ( );
  StatusCode decodeCreateSvcNameList ( );
  void createSvcNameListHandler(Property &);
  void       extSvcNameListHandler   ( Property& theProp );
  StatusCode decodeMultiThreadSvcNameList( );
  void  multiThreadSvcNameListHandler( Property& theProp );
  StatusCode decodeDllNameList       ( );
  void       dllNameListHandler      ( Property& theProp );
  void       reflexDebugPropertyHandler ( Property& theProp );
  void       initLoopCheckHndlr      ( Property& );
  //@}

protected:
  /// declare one or more copies of svc type/name as determined by NoOfThreads
  StatusCode declareMultiSvcType(const std::string& name,
				 const std::string& type);
  /// add one or more copies of svc type/name as determined by NoOfThreads
  StatusCode addMultiSvc(const std::string& name, const std::string& type,
			 int prio);

  // implementation of IService::setServiceManager
  void setServiceManager(ISvcManager*) {}

  /// Internal startup routine
  StatusCode i_startup();

  /// Reach a state from current state (whichever it is) going through the
  /// correct transitions.
  /// By default, if a transition fails, the chain is interrupted, but the
  /// behavior can be changed with the paramenter "gnoreFailures"
  StatusCode GoToState(Gaudi::StateMachine::State state, bool ignoreFailures = false);

  // data members
  int                 m_refcount;           ///< Reference count
  ServiceManager*     m_svcManager;         ///< Reference to the service factory
  AlgorithmManager*   m_algManager;         ///< Reference to the algorimth factory
  DLLClassManager*    m_classManager;       ///< Reference to the class manager
  ISvcLocator*        m_svcLocator;         ///< Reference to its own service locator

  IntegerProperty     m_SIGo;               ///< For SI's "Go" command via callback
  IntegerProperty     m_SIExit;             ///< For SI's "Exit" command via callback
  StringArrayProperty m_topAlgNameList;     ///< List of top level algorithms names
  StringArrayProperty m_outStreamNameList;  ///< List of output stream names
  StringProperty      m_outStreamType;      ///< Output stream type (obsolete?)
  StringProperty      m_messageSvcType;     ///< MessageSvc type
  StringProperty      m_jobOptionsSvcType;  ///< JobOptionsSvc type


  ListSvc             m_topSvcList;         ///< List of top level services
  ListName            m_topSvcNameList;     ///< List of top level services names

  std::string         m_name;               ///< Name
  Gaudi::StateMachine::State m_state;              ///< Internal State
  Gaudi::StateMachine::State m_targetState;              ///< Internal State

  VectorName          m_defServices;        ///< Vector default services names
  VectorName          m_svcMapping;         ///< Default mapping of services
  VectorName          m_svcOptMapping;      ///< Default mapping of services

  PropertyMgr*        m_propertyMgr;        ///< Reference to Property Manager
  IMessageSvc*        m_messageSvc;         ///< Reference to the message service
  IRunable*           m_runable;            ///< Reference to the runable object
  IEventProcessor*    m_processingMgr;      ///< Reference to processing manager object
  IJobOptionsSvc*     m_jobOptionsSvc;      ///< Reference to JobOption service

  //
  // The public ApplicationMgr properties
  //

  int                  m_evtMax;            ///< Number of events to be processed
  StringArrayProperty  m_extSvcNameList;    ///< List of external services names
  BooleanProperty      m_extSvcCreates;     ///< LHCb or ATLAS defn of "ExtSvc"

  /// List of external services names for which we want a copy per evt thread
  StringArrayProperty  m_multiThreadSvcNameList;
  int                  m_noOfEvtThreads;    ///< no of multiThreadSvc copies

  StringArrayProperty  m_dllNameList;       ///< List of DDL's names
  std::string          m_jobOptionsType;    ///< Source type (e.g. dbase, file...)
  std::string          m_jobOptionsPath;    ///< The "file" to look for properties
  std::string          m_runableType;       ///< Runable type
  std::string          m_eventLoopMgr;      ///< Processing manager type
  std::string          m_evtsel;            ///< Event selection
  std::string          m_histPersName;      ///< CGL: Name of the Hist Pers Svc
  int                  m_outputLevel;       ///< Message output level
  std::string          m_appName;           ///< The name of the application
  std::string          m_appVersion;        ///< The version of the application
  bool                 m_actHistory;        ///< Activate HistorySvc
  bool                 m_codeCheck;         ///< Activate StatusCode checking
  IntegerProperty      m_reflexDebugLevel;  ///< Debug level for the reflex plugin system

  StringArrayProperty  m_createSvcNameList;

  /// Defaults for auditors.
  BooleanProperty      m_auditTools;
  BooleanProperty      m_auditSvcs;
  BooleanProperty      m_auditAlgs;

  std::map<std::string,std::string> m_environment; ///< Environment variables to set

  /// For ServiceMgr initialization loop checking
  BooleanProperty      m_loopCheck;

  // Flag to activate the printout of properties
  bool                 m_propertiesPrint ; ///< flag to actiavte the printout of properties

private:
   std::vector<std::string> m_okDlls;       ///< names of successfully loaded dlls
};
#endif  // GAUDI_APPLICATIONMGR_H
