// $Id: Service.h,v 1.23 2008/06/02 14:20:37 marcocle Exp $
// ============================================================================
#ifndef GAUDIKERNEL_SERVICE_H
#define GAUDIKERNEL_SERVICE_H
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IStateful.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IAuditorSvc.h"
// ============================================================================
#include <vector>
// ============================================================================
// Forward delarations
// ============================================================================
class IMessageSvc;
class ISvcManager;
class ServiceManager;
// ============================================================================
/** @class Service GaudiKernel/Service.h
 *
 *  Base class for all services. It implements the IService and IProperty interfaces.
 *
 *  @author Pere Mato
 */
class Service : public virtual IService,
                public virtual IProperty,
                public virtual IStateful {
public:
  friend class ServiceManager;
  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /** Reference Interface instance               */
  virtual unsigned long addRef();

  /** Release Interface instance                 */
  virtual unsigned long release();

  /** Retrieve name of the service               */
  virtual const std::string& name() const;
  /** Retrieve ID of the Service                 */
  virtual const InterfaceID& type() const;

  // State machine implementation
  virtual StatusCode configure() { return StatusCode::SUCCESS; }
  virtual StatusCode initialize();
  virtual StatusCode start();
  virtual StatusCode stop();
  virtual StatusCode finalize();
  virtual StatusCode terminate() { return StatusCode::SUCCESS; }
  virtual Gaudi::StateMachine::State FSMState() const { return m_state; }
  virtual Gaudi::StateMachine::State targetFSMState() const { return m_targetState; }
  virtual StatusCode reinitialize();
  virtual StatusCode restart();
  
  /** Initilize Service                          */
  virtual StatusCode sysInitialize();
  /** Initilize Service                          */
  virtual StatusCode sysStart();
  /** Initilize Service                          */
  virtual StatusCode sysStop();
  /** Finalize Service                           */
  virtual StatusCode sysFinalize();
  /// Re-initialize the Service 
  virtual StatusCode sysReinitialize();
  /// Re-initialize the Service 
  virtual StatusCode sysRestart();

  // Default implementations for ISetProperty
  virtual StatusCode setProperty(const Property& p);
  virtual StatusCode setProperty( const std::string& s );
  virtual StatusCode setProperty( const std::string& n, const std::string& v);
  virtual StatusCode getProperty(Property* p) const;
  virtual const Property& getProperty( const std::string& name) const;
  virtual StatusCode getProperty( const std::string& n, std::string& v ) const;
  virtual const std::vector<Property*>& getProperties( ) const;

  /** set the property form the value 
   *  
   *  @code 
   *
   *  std::vector<double> data = ... ;
   *  setProperty( "Data" , data ) ;
   *  
   *  std::map<std::string,double> cuts = ... ;
   *  setProperty( "Cuts" , cuts ) ;
   *
   *  std::map<std::string,std::string> dict = ... ;
   *  setProperty( "Dictionary" , dict ) ;
   * 
   *  @endcode 
   *
   *  Note: the interface IProperty allows setting of the properties either 
   *        directly from other properties or from strings only
   *
   *  This is very convinient in resetting of the default 
   *  properties in the derived classes.
   *  E.g. without this method one needs to convert 
   *  everything into strings to use IProperty::setProperty
   *
   *  @code 
   *  
   *    setProperty ( "OutputLevel" , "1"    ) ;
   *    setProperty ( "Enable"      , "True" ) ;
   *    setProperty ( "ErrorMax"    , "10"   ) ;
   *
   *  @endcode 
   *
   *  For simple cases it is more or less ok, but for complicated properties 
   *  it is just ugly..
   *
   *  @param name      name of the property 
   *  @param value     value of the property
   *  @see Gaudi::Utils::setProperty
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-05-13 
   */ 
  template <class TYPE>
  StatusCode setProperty 
  ( const std::string& name  , 
    const TYPE&        value ) 
  { return Gaudi::Utils::setProperty ( m_propertyMgr , name , value ) ; }

  /** Standard Constructor                       */
  Service( const std::string& name, ISvcLocator* svcloc);
  /** Retrieve pointer to service locator        */
  ISvcLocator* serviceLocator() const;
  /** Retrieve pointer to message service        */
  IMessageSvc* msgSvc();
  IMessageSvc* msgSvc() const;
  // Obsoleted name, kept due to the backwards compatibility
  IMessageSvc* messageService();
  IMessageSvc* messageService() const;


  /** Method for setting declared properties to the values
      specifed for the job.
  */
  StatusCode setProperties();

  /** Access a service by name, creating it if it doesn't already exist.
  */
  template <class T>
  StatusCode service( const std::string& name, T*& psvc, bool createIf = true ) const {
    return service_i(name, createIf, T::interfaceID(), (void**)&psvc);
  }

  /** Access a service by name and type, creating it if it doesn't already exist.
  */
  template <class T>
  StatusCode service( const std::string& svcType, const std::string& svcName,
		      T*& psvc) const {
    return service_i(svcType, svcName, T::interfaceID(), (void**)&psvc);
  }
  // ==========================================================================
  /** Declare the named property
   *
   *  @code
   *
   *  MySvc ( const std::string& name , 
   *          ISvcLocator*       pSvc ) 
   *     : Service ( name , pSvc ) 
   *     , m_property1   ( ... )
   *     , m_property2   ( ... )
   *   {
   *     // declare the property 
   *     declareProperty( "Property1" , m_property1 , "Doc fro property #1" ) ;
   *
   *     // declare the property and attach the handler  to it
   *     declareProperty( "Property2" , m_property2 , "Doc for property #2" ) 
   *        -> declareUpdateHandler( &MyAlg::handler_2 ) ;
   *  
   *   }
   *  @endcode
   *  
   *  @see PropertyMgr 
   *  @see PropertyMgr::declareProperty 
   *  
   *  @param name the property name 
   *  @param proeprty the property itself, 
   *  @param doc      the documentation string
   *  @return the actual property objects 
   */
  template <class T>
  Property* declareProperty
  ( const std::string& name              , 
    T&                 property          ,
    const std::string& doc      = "none" ) const 
  { 
    return m_propertyMgr -> declareProperty ( name , property , doc ) ;
  }
  // ==========================================================================
  /** Declare remote named properties. This is used to declare as a local property
      a property of another services or algorithm. The only needed information is
      IProperty interface of the remote service and the its property name if different
      of the current one.
      @param name       Local property name
      @param rsvc       Remote IProperty interface
      @param rname      Name of the property at remote service
  */
  Property* declareRemoteProperty
  ( const std::string& name       , 
    IProperty*         rsvc       , 
    const std::string& rname = "" ) const 
  {
  	return m_propertyMgr -> declareRemoteProperty ( name , rsvc , rname ) ;
  }
  // ==========================================================================
  /** The standard auditor service.May not be invoked before sysInitialize()
   *  has been invoked.
   */
  IAuditorSvc* auditorSvc() const;

protected:
  /** Standard Destructor                        */
  virtual ~Service();
  /** Service output level                       */
  IntegerProperty m_outputLevel;
  /** Service state                              */
  Gaudi::StateMachine::State    m_state;
  /** Service state                              */
  Gaudi::StateMachine::State    m_targetState;
  /** MessageSvc reference                       */
  mutable IMessageSvc*  m_messageSvc;

  /// get the @c Service's output level
  int  outputLevel() const { return m_outputLevel.value(); }

private:
  /** Service Name  */
  std::string   m_name;
  /** Reference counter                          */
  unsigned long m_refCount;
  /** Service Locator reference                  */
  ISvcLocator*  m_svcLocator;
  ISvcManager*  m_svcManager;
  /** Property Manager                           */
  PropertyMgr*  m_propertyMgr;
  /** implementation of service method */
  StatusCode service_i(const std::string& svcName,
		       bool createIf,
		       const InterfaceID& iid,
		       void** ppSvc) const;
  StatusCode service_i(const std::string& svcType,
		       const std::string& svcName,
		       const InterfaceID& iid,
		       void** ppSvc) const;
  void setServiceManager(ISvcManager*);

  /** Auditor Service                            */
  mutable IAuditorSvc*  m_pAuditorSvc;
  BooleanProperty       m_auditInit;
  bool                  m_auditorInitialize;
  bool                  m_auditorStart;
  bool                  m_auditorStop;
  bool                  m_auditorFinalize;
  bool                  m_auditorReinitialize;
  bool                  m_auditorRestart;

  /** callback for output level property */
  void initOutputLevel(Property& prop);
};

#endif // GAUDIKERNEL_SERVICE_H
