// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IRunable.h,v 1.1 2001/03/14 15:30:13 mato Exp $
#ifndef GAUDIKERNEL_IRUNNABLE_H
#define GAUDIKERNEL_IRUNNABLE_H

#include "GaudiKernel/IInterface.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IRunable(501, 1 , 0); 

/** @class IRunable IRunable.h GaudiKernel/IRunable.h

    Runable interface definition. Package GaudiKernel

    Abstract interface of an object to be "run".
    Best analogy: java.thread.Thread.run(). 
    A known user is the application manager, which
    gives control to a runable for execution of the event loop.

   @author Markus Frank
   @version 1.0
*/
class IRunable : virtual public IInterface   {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRunable; }

  /// Run the class implementation
  virtual StatusCode run() = 0;
};

#endif // GAUDIKERNEL_IRUNNABLE_H
