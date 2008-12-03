///////////////////////// -*- C++ -*- /////////////////////////////
// IIoComponent.h 
// Header file for class IIoComponent
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef GAUDIKERNEL_IIOCOMPONENT_H 
#define GAUDIKERNEL_IIOCOMPONENT_H 1 

/** @class IIoComponent
 */

// STL includes
#include <string>

// GaudiKernel includes
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/StatusCode.h"

class IIoComponent
  : virtual public ::INamedInterface
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Destructor: 
   */
  virtual ~IIoComponent();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /// Declaration of the interface ID
  static const InterfaceID& interfaceID();
  
  /** @brief callback method to reinitialize the internal state of
   *         the component for I/O purposes (e.g. upon @c fork(2))
   */
  virtual
  StatusCode io_reinit () = 0;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline 
const InterfaceID& 
IIoComponent::interfaceID() 
{ 
  // Declaration of the interface ID
  static const InterfaceID IID_IIoComponent("IIoComponent", 1, 0);
  return IID_IIoComponent; 
}


#endif //> !GAUDIKERNEL_IIOCOMPONENT_H
