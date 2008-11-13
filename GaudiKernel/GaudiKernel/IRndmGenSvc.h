// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IRndmGenSvc.h,v 1.4 2006/11/30 14:43:13 mato Exp $
#ifndef GAUDIKERNEL_IRNDMGENSVC_H
#define GAUDIKERNEL_IRNDMGENSVC_H

// STL include files
#include <string>
#include <vector>

// Framework include files
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IRndmGen.h"

// Forward declarations
class IFactory;
class IRndmGen;
class StreamBuffer;
class IRndmEngine;
class IRndmGenFactory;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IRndmGenSvc(141, 1 , 0); 

/** @class IRndmGenSvc IRndmGenSvc.h GaudiKernel/IRndmGenSvc.h

    Random Generator service interface definition
    Definition of a interface for a service to access 
    random generators according to predefined distributions:

    The Random Generator creates proper instances of requested
    random generators and delivers them to the user.

    On request the service dumps the type and state of the random
    number engine to a StreamBuffer object allowing to save and to restore
    the exact state of the random number generator.

    @author  M.Frank
    @version 1.0
*/
class IRndmGenSvc : virtual public IService   {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IRndmGenSvc; }
  /** Retrieve engine.
      @return pointer to random number engine
  */
  virtual IRndmEngine* engine() = 0;
  /** Add a Generator factory.
      @param fac Pointer to factory to be added
      @return StatusCode indicating success or failure.
  */

  virtual StatusCode generator(const IRndmGen::Param& par, IRndmGen*& refpGen) = 0;
};

#endif // GAUDIKERNEL_IRNDMGENSVC_H
