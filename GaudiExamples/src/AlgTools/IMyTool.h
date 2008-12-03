// $Id: IMyTool.h,v 1.4 2008/10/10 15:18:56 marcocle Exp $
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

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IMyOtherTool("IMyOtherTool", 1 , 0);

/** @class IMyOtherTool IMyTool.h
 *
 *  Second interface, to test multiple interface tools in GaudiAlg
 *
 *  @author Chris Jones
 */
class IMyOtherTool : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMyOtherTool; }
  /// Do something else
  virtual void doItAgain() = 0;
};

#endif // GAUDIEXAMPLE_IMYTOOL_H
