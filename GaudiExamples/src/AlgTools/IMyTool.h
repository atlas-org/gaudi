// $Id: IMyTool.h,v 1.3 2002/11/06 08:50:55 mato Exp $
#ifndef GAUDIEXAMPLE_IMYTOOL_H
#define GAUDIEXAMPLE_IMYTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMyTool("IMyTool", 1 , 0); 

/** @class IMyTool IMyTool.h 
 *  Example of an Interface of a Algorithm Tool
 *
 *  @author Pere Mato
*/
class IMyTool : virtual public IAlgTool {
 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMyTool; }
  /// Get a message
  virtual const std::string&  message() const = 0;
  /// Do something
  virtual void  doIt() = 0;
};

#endif // GAUDIEXAMPLE_IMYTOOL_H
