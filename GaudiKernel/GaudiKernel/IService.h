// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IService.h,v 1.7 2006/12/10 20:29:17 leggett Exp $
#ifndef GAUDIKERNEL_ISERVICE_H
#define GAUDIKERNEL_ISERVICE_H

// Include files
#include "GaudiKernel/INamedInterface.h"
#include <string>

// Declaration of the interface ID. 
static const InterfaceID IID_IService(2, 2, 0); 

/** @class IService IService.h GaudiKernel/IService.h

    General service interface definition

    @author Pere Mato
*/
class ISvcManager;
class ServiceManager;

class IService : virtual public INamedInterface  {
  friend class ServiceManager;
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IService; }
  /// Define Service states                     
  enum State { OFFLINE, CONFIGURED, FINALIZED, INITIALIZED };

  /// Retrieve ID of the Service. Not really used.
  virtual const InterfaceID&  type() const = 0;
  /// Initilize Service 
  virtual StatusCode initialize() = 0;
  virtual StatusCode sysInitialize() = 0;
  virtual StatusCode sysReinitialize() = 0;
  /// Finalize Service
  virtual StatusCode finalize() = 0;
  virtual StatusCode sysFinalize() = 0;
  /// Retrieve state of the Service 
  virtual State state( ) const = 0;
  /// Re-initialize the Service 
  virtual StatusCode reinitialize() = 0;
protected:
  virtual void setServiceManager(ISvcManager*) = 0;
};

#endif  // GAUDIKERNEL_ISERVICE_H
