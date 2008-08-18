// $Id: ISvcManager.h,v 1.5 2006/12/10 20:29:17 leggett Exp $
#ifndef GAUDIKERNEL_ISVCMANAGER_H
#define GAUDIKERNEL_ISVCMANAGER_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <string>

// Forward class declaration
class IService;
class ISvcLocator;
class ISvcFactory;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISvcManager(13, 3 , 0); 

/** @class ISvcManager ISvcManager.h GaudiKernel/ISvcManager.h
    
    The ISvcManager is the interface implemented by the Service Factory in the 
    Application Manager to support management functions. Clients use this 
    interface to declare abstract service factories, and to create concrete
    instances of services. 

    @author Pere Mato
*/
class ISvcManager : virtual public  IInterface  {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISvcManager; }

  /** Add a service to the "active" list of services of the factory
    * @param svc Pointer to the service
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode addService( IService* svc, int prio ) = 0;

  /** Add a service to the "active" list of services of the factory
    * @param svc Pointer to the service
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode addService( const std::string& typ, const std::string& nam, int prio ) = 0;

  /** Remove a service from the "active" list of services of the factory
    * @param svc Pointer to the service
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode removeService( IService* svc ) = 0;

  /** Remove a service from the "active" list of services of the factory
    * @param svc Pointer to the service
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode removeService( const std::string& nam ) = 0;

  /** Declare an abstract factory for a given service type
    * @param factory Abstract factory reference
    * @param svctype Service type name
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode declareSvcFactory( const ISvcFactory& factory, 
                                        const std::string& svctype ) = 0;

  /** Declare the type of the service to be used when crating a given service name
    * @param svcname Service name 
    * @param svctype Service type name
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode declareSvcType( const std::string& svcname,
                                     const std::string& svctype ) = 0;

  /** Creates and instance of a service type that has been declared beforehand and 
    * assigns it a name. It returns a pointer to an IService.
    * @param svctype Service type name
    * @param svcname Service name to be set
    * @param svc Returned service pointer
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode createService( const std::string& svctype,
                                    const std::string& svcname,
                                    IService*& svc ) = 0;

  /** Access to service factory by name to create unmanaged services
    * @param  svc_type    [IN]      Name of the service type
    * @param  fac         [OUT]     Reference to store pointer to service factory
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode getFactory(    const std::string& svc_type, 
                                    const ISvcFactory*& fac) const = 0;

  /** Initializes the list of "active" services
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode initializeServices() = 0;

  /** Finalizes the list of "active" services
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode finalizeServices() = 0;

  /** Reinitializes the list of "active" services
    *
    * @return StatusCode indicating success or failure.
    */
  virtual StatusCode reinitializeServices() = 0;

  virtual int getPriority(const std::string& name) const = 0;
  virtual StatusCode setPriority(const std::string& name, int pri) = 0;
};

#endif  // GAUDIKERNEL_ISVCMANAGER_H

