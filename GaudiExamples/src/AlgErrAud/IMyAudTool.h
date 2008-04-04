// $Id: IMyAudTool.h,v 1.1 2007/01/22 16:06:11 hmd Exp $
#ifndef GAUDIEXAMPLE_IMYAUDTOOL_H
#define GAUDIEXAMPLE_IMYAUDTOOL_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMyAudTool("IMyAudTool", 1 , 0); 

/** @class IMyAudTool IMyAudTool.h 
 *  Example of an Interface of a Algorithm Tool
 *
 *  @author Pere Mato
*/
class IMyAudTool : virtual public IAlgTool {
 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMyAudTool; }
  /// Get a message
  virtual const std::string&  message() const = 0;
  /// Do something
  virtual void  doErr() = 0;
  virtual void  doFatal() = 0;
};

#endif // GAUDIEXAMPLE_IMYAUDTOOL_H
