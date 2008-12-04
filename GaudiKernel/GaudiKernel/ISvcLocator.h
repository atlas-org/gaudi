// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/ISvcLocator.h,v 1.15 2006/12/14 12:55:44 hmd Exp $
#ifndef GAUDIKERNEL_ISVCLOCATOR_H
#define GAUDIKERNEL_ISVCLOCATOR_H 1

// Include files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcManager.h"
#include <string>
#include <list>

// Forward class declaration
class IService;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ISvcLocator(11, 2 , 1);

/** @class ISvcLocator ISvcLocator.h GaudiKernel/ISvcLocator.h
    The ISvcLocator is the interface implemented by the Service Factory in the
    Application Manager to locate services in the framework. Clients use this
    interface to locate references to interfaces of services existing in the
    application. This operation needs to be done before the service can be used
    by the client. Typically "locating the services" is done at the initialization
    phase of the clients.

    @author Pere Mato
*/
class ISvcLocator : virtual public  IInterface  {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISvcLocator; }
  /** Get a reference to the service given a service name
      @param name Service name
      @param svc Returned service pointer
  */
  virtual StatusCode getService( const std::string& name,
                                 IService*&  svc ) = 0;
  /** Get a specific interface pointer given a service name and interface id
      @param name Service name
      @param iid Interface ID
      @param pinterface Returned pointer to the requested interface
  */
  virtual StatusCode getService( const std::string& name,
                                 const InterfaceID& iid,
                                 IInterface*& pinterface ) = 0;

  /** Get a reference to a service and create it if it does not exists
      @param name Service name
      @param svc Returned service pointer
      @param createIf flag to control the creation
  */
  virtual StatusCode getService( const std::string& name,
                                 IService*& svc,
                                 bool createIf ) = 0;

  /// Erase service from the list of known services
  //virtual void eraseService(const IService*) = 0;

  /// Return the list of Services
  virtual const std::list<IService*>& getServices( ) const = 0;

  /// Check the existance of a service given a service name
  virtual bool existsService( const std::string& name ) const = 0;

  /// Templated method to access a service by name.
  template <class T>
  StatusCode service( const std::string& name, T*& svc, bool createIf = true ) {
    if( createIf ) {
      IService* s;
      StatusCode sc = getService( name, s, true);
      if ( !sc.isSuccess() ) return sc;  // Must check if initialization was OK!
    }
    return getService( name, T::interfaceID(), (IInterface*&)svc );
  }

  /// Templated method to access a service by type and name.
  template <class T>
  StatusCode service( const std::string& type, const std::string& name,
		      T*& svc, bool createIf = true ) {
    try {
      ISvcManager& theManager = dynamic_cast<ISvcManager&>(*this);
      theManager.declareSvcType(name, type).ignore();
    } catch(...) {}
    //even if the above fails we may still find the service, so keep going
    return service(name, svc, createIf);
  }
};


#endif  // GAUDI_ISVCLOCATOR_H
