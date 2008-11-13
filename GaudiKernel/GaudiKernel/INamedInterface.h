#ifndef GAUDIKERNEL_INAMEDINTERFACE_H
#define GAUDIKERNEL_INAMEDINTERFACE_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <string>

// Interface ID
static const InterfaceID IID_INamedInterface("INamedInterface",0,0);

/**
 * @class INamedInterface INamedInterface.h GaudiKernel/INamedInterface.h
 *
 * IInterface compliant class extending IInterface with the name() method.
 *
 * @author Charles Leggett 
 * @author Marco Clemencic
 */
class INamedInterface : virtual public IInterface {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_INamedInterface; }

  /// Retrieve the name of the instance.
  virtual const std::string& name() const = 0;

  /// Virtual destructor (always needed for abstract classes).
  virtual ~INamedInterface() {}
  
};

#endif
