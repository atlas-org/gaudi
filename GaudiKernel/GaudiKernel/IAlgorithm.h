// $Id: IAlgorithm.h,v 1.10 2007/05/23 09:48:50 marcocle Exp $
#ifndef GAUDIKERNEL_IALGORITHM_H
#define GAUDIKERNEL_IALGORITHM_H

// Include files
#include "GaudiKernel/INamedInterface.h"
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IAlgorithm("IAlgorithm", 3 , 0); 

/** @class IAlgorithm IAlgorithm.h GaudiKernel/IAlgorithm.h

    The IAlgorithm is the interface implmented by the Algorithm base class.
    Concrete algorithms, derived from the Algorithm base class are controlled 
    via this interface.

    @author Paul Maley
    @author D.Quarrie
*/
class IAlgorithm : virtual public INamedInterface {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAlgorithm; }

  /** The version of the algorithm
   */
  virtual const std::string& version() const = 0;

  /** Initialization of a concrete algorithm object. Concrete algorithm classes
      should overide this method.
  */
  virtual StatusCode initialize() = 0;

  /** Re-initialization of a concrete algorithm object. Concrete algorithm classes
      should overide this method.
  */
  virtual StatusCode reinitialize() = 0;

  /** The action to be performed by the algorithm on an event. This method is
      invoked once per event for top level algorithms by the application manager.
  */
  virtual StatusCode execute() = 0;

  /** Algorithm finalization. This method should be overridden by concrete algorithm
      classes. It will typically be used for outputting statistical summaries of the
      algorithm's performance over an event sample.
  */
  virtual StatusCode finalize() = 0;

  /// check if the algorithm is initialized properly
  virtual bool isInitialized() const = 0; 
  /// check if the algorithm is finalized properly 
  virtual bool isFinalized() const = 0; 
  /// check if th ealgorithm is already executed for the current event
  virtual bool isExecuted() const = 0; 

  /** Initialization method invoked by the framework. This method is responsible
      for any bookkeeping of initialization required by the framework itself.
      It will in turn invoke the initialize() method of the derived algorithm,
      and of any sub-algorithms which it creates.
  */
  virtual StatusCode sysInitialize() = 0;

  /** Re-initialization method invoked by the framework. This method is responsible
      for any re-initialization required by the framework itself.
      It will in turn invoke the reinitialize() method of the derived algorithm,
      and of any sub-algorithms which it creates.
  */
  virtual StatusCode sysReinitialize() = 0;

  /// System execution. This method invokes the execute() method of a concrete algorithm 
  virtual StatusCode sysExecute() = 0;

  /** System finalization. This method invokes the finalize() method of a concrete
      algorithm and the finalize() methods of all of that algorithm's sub algorithms.
  */
  virtual StatusCode sysFinalize() = 0;

  /** beginRun method invoked by the framework. This method is responsible
      for any beginRun actions required by the framework itself.
      It will in turn invoke the beginRun() method of the derived algorithm,
      and of any sub-algorithms which it creates.
  */
  virtual StatusCode sysBeginRun( ) = 0;

  /** endRun method invoked by the framework. This method is responsible
      for any endRun actions required by the framework itself.
      It will in turn invoke the endRun() method of the derived algorithm,
      and of any sub-algorithms which it creates.
  */
  virtual StatusCode sysEndRun( ) = 0;

  /// Reset the Algorithm executed state for the current event
  virtual void resetExecuted( ) = 0;

  /** Algorithm begin run. This method is called at the beginning of the event loop.
  */
  virtual StatusCode beginRun() = 0;
  /** Algorithm end run. This method is called at the end of the event loop.
  */
  virtual StatusCode endRun() = 0;

};

#endif // GAUDIKERNEL_IALGORITHM_H
