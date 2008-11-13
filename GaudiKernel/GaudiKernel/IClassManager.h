// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IClassManager.h,v 1.6 2006/12/11 13:40:13 leggett Exp $
#ifndef GAUDIKERNEL_ICLASSMANAGER_H
#define GAUDIKERNEL_ICLASSMANAGER_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <string>

// Forward class declaration
class IFactory;
class ISvcLocator;


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IClassManager(16, 1 , 0); 

/** @class IClassManager IClassManager.h GaudiKernel/IClassManager.h

    The IClassManager is the interface implemented by the generic Factory in the
    Application Manager to support class management functions.

    @author Pere Mato
*/
class IClassManager : virtual public  IInterface  {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IClassManager; }

  /// Declare a shareable library to be used for creating instances of a given algorithm type.
  virtual StatusCode loadModule( const std::string& module,
				 bool fireIncident=true ) = 0;  // Shareable library name
};

#endif  // GAUDIKERNEL_ICLASSMANAGER_H
