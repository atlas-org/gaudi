// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IAppMgrUI.h,v 1.5 2001/06/29 13:47:41 mato Exp $
#ifndef GAUDIKERNEL_IAPPMGRUI_H
#define GAUDIKERNEL_IAPPMGRUI_H 1

// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IInterface.h"

#include <string>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IAppMgrUI(12, 2 , 0);

/** @class IAppMgrUI IAppMgrUI.h GaudiKernel/IAppMgrUI.h

    Application Manager User Interface. This is the interface 
    offered to the UI to control the JOB or Application.

    @author Pere Mato
    @date   30/10/98
*/

class IAppMgrUI : virtual public  IInterface  {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAppMgrUI; }
  /// Run the complete job (from intialize to terminate)
  virtual StatusCode run() = 0;
  /// Configure the job
  virtual StatusCode configure() = 0;
  /// Terminate the job
  virtual StatusCode terminate() = 0;
  /// Initialize the job
  virtual StatusCode initialize() = 0;
  /// Finalize the job
  virtual StatusCode finalize() = 0;
  /// Process the next maxevt events
  virtual StatusCode nextEvent(int maxevt) = 0;

  /// The identifying name of the AppMgrUI object.
  virtual const std::string& name() const = 0;
  // current state of the AppMgrUI object
  virtual const std::string& stateName()  const = 0;
};
#endif  // KERNEL_IAPPMGRUI_H
