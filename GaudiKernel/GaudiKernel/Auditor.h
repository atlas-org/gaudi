// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/Auditor.h,v 1.16 2008/04/03 17:27:01 marcocle Exp $
#ifndef GAUDIKERNEL_AUDITOR_H
#define GAUDIKERNEL_AUDITOR_H

// Include files
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"  /*used by service(..)*/
#include "GaudiKernel/PropertyMgr.h"
#include <string>
#include <vector>

// Forward declarations
class IService;
class IMessageSvc;
class Property;
class Algorithm;

/** @class Auditor Auditor.h GaudiKernel/Auditor.h

    Base class from which all concrete auditor classes should be derived. 
    The only base class functionality which may be used in the
    constructor of a concrete auditor is the declaration of 
    member variables as properties. All other functionality, 
    i.e. the use of services, may be used only in 
    initialise() and afterwards.

    @author David Quarrie
    @date   2000
    @author Marco Clemencic
    @date   2008-03
*/ 
class Auditor : virtual public IAuditor,
                virtual public IProperty {
public:

  /** Constructor
      @param name    The algorithm object's name
      @param svcloc  A pointer to a service location service */
  Auditor( const std::string& name, ISvcLocator *svcloc );
  /// Destructor
  virtual ~Auditor();

  /** Initialization method invoked by the framework. This method is responsible
      for any bookkeeping of initialization required by the framework itself.
  */
  StatusCode sysInitialize();
  /** Finalization method invoked by the framework. This method is responsible
      for any bookkeeping of initialization required by the framework itself.
  */
  StatusCode sysFinalize();

  /// The following methods are meant to be implemented by the child class...

  virtual void before(StandardEventType, INamedInterface*);
  virtual void before(StandardEventType, const std::string&);

  virtual void before(CustomEventTypeRef, INamedInterface*);
  virtual void before(CustomEventTypeRef, const std::string&);

  virtual void after(StandardEventType, INamedInterface*, const StatusCode&);
  virtual void after(StandardEventType, const std::string&, const StatusCode&);

  virtual void after(CustomEventTypeRef, INamedInterface*, const StatusCode&);
  virtual void after(CustomEventTypeRef, const std::string&, const StatusCode&);

  // Obsolete methods
  
  virtual void beforeInitialize(INamedInterface* ) ;
  virtual void afterInitialize(INamedInterface* ) ;

  virtual void beforeReinitialize(INamedInterface* ) ;
  virtual void afterReinitialize(INamedInterface* ) ;

  virtual void beforeExecute(INamedInterface* );
  virtual void afterExecute(INamedInterface*, const StatusCode& );

  virtual void beforeFinalize(INamedInterface* ) ;
  virtual void afterFinalize(INamedInterface* ) ;

  virtual void beforeBeginRun(INamedInterface* );
  virtual void afterBeginRun(INamedInterface* );

  virtual void beforeEndRun(INamedInterface* );
  virtual void afterEndRun(INamedInterface* );

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual const std::string&  name() const ;

  virtual const bool isEnabled() const ;

  /** The standard message service. Returns a pointer to the standard message
      service. May not be invoked before sysInitialize() has been invoked.
  */
  IMessageSvc*  msgSvc() const;

  /// Retrieve the outputlevel of current auditor
  int outputLevel() const { return m_outputLevel; }
   
  /// Set the outputlevel for current auditor
  void setOutputLevel( int level );
   
  /** The standard service locator. Returns a pointer to the service locator service.
      This service may be used by an auditor to request any services it requires in
      addition to those provided by default.
  */
  ISvcLocator* serviceLocator() const;

  /** Access a service by name, creating it if it doesn't already exist.
  */
  template <class T>
  StatusCode service( const std::string& name, T*& svc, bool createIf = false ) const {
    IService* theSvc;
    StatusCode sc = serviceLocator()->getService( name, theSvc, createIf );
    if ( sc.isSuccess() ) {
      return theSvc->queryInterface(T::interfaceID(), (void**)&svc);
    }
    svc = 0;
    return sc;
  }

  /// Set a value of a property of an auditor. 
  virtual StatusCode setProperty(const Property& p);

  /// Implementation of IProperty::setProperty
  virtual StatusCode setProperty( const std::string& s );

  /// Implementation of IProperty::setProperty
  virtual StatusCode setProperty( const std::string& n, const std::string& v);

  /// Get the value of a property.
  virtual StatusCode getProperty(Property* p) const;

  /// Get the property by name.
  virtual const Property& getProperty( const std::string& name) const;

  /// Implementation of IProperty::getProperty
  virtual StatusCode getProperty( const std::string& n, std::string& v ) const;

  /// Get all properties.
  const std::vector<Property*>& getProperties( ) const;


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
  { return Gaudi::Utils::setProperty ( m_PropertyMgr , name , value ) ; }
  
  /** Set the auditor's properties. This method requests the job options service
      to set the values of any declared properties. The method is invoked from 
      within sysInitialize() by the framework and does not need to be explicitly
      called by a concrete auditor.
  */
  StatusCode setProperties();

  // ==========================================================================
  /** Declare the named property
   *
   *  @code
   *
   *  MyAuditor( ... ) 
   *     : Auditor ( ...  ) 
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
  Property* declareProperty( const std::string& name, T& property,
			     const std::string& doc = "none") const {
	return m_PropertyMgr->declareProperty(name, property, doc);
  }

  /// Methods for IInterface
  unsigned long addRef();
  unsigned long release();
  StatusCode queryInterface(const InterfaceID& riid, void**);

 private:

  long m_refCount;              ///< Counter for references to Auditor
  std::string m_name;	          ///< Auditor's name for identification

  IMessageSvc* m_MS;            ///< Message service
  ISvcLocator* m_pSvcLocator;   ///< Pointer to service locator service
  PropertyMgr* m_PropertyMgr;   ///< For management of properties
  int          m_outputLevel;   ///< Auditor output level
  bool         m_isEnabled;     ///< Auditor is enabled flag
  bool         m_isInitialized; ///< Auditor has been initialized flag
  bool         m_isFinalized;   ///< Auditor has been finalized flag

  // Private Copy constructor: NO COPY ALLOWED
  Auditor(const Auditor& a);

  // Private asignment operator: NO ASSIGNMENT ALLOWED
  Auditor& operator=(const Auditor& rhs);
};

#endif //GAUDIKERNEL_AUDITOR_H

