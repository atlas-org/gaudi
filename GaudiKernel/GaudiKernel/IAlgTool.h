// $Id: IAlgTool.h,v 1.6 2006/12/10 20:29:16 leggett Exp $
#ifndef GAUDIKERNEL_IALGTOOL_H
#define GAUDIKERNEL_IALGTOOL_H

// Include files
#include "GaudiKernel/INamedInterface.h"
#include <string>

// Forward declarations
class IAlgorithm;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IAlgTool("IAlgTool", 2 , 0);

/** @class IAlgTool IAlgTool.h GaudiKernel/IAlgTool.h

    The interface implemented by the AlgTool base class.
    Concrete tools, derived from the AlgTool based class are 
    controlled via this interface.

    @author Gloria Corti
    @author Pere Mato
    @date 15/11/01 version 2 introduced
*/ 
class IAlgTool : virtual public INamedInterface {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAlgTool; }

  /// The type of an AlgTool, meaning the concrete AlgTool class.
  virtual const std::string&  type() const = 0;

  /** The parent of the concrete AlgTool. It can be an Algorithm
      or a Service. A common AlgTool has the ToolSvc as parent.
  */
  virtual const IInterface*   parent() const = 0;

  /** Initialization of the Tool. This method is called typically
   *  by the ToolSvc. It allows to complete  the initialization that
   *  could not be done with a simply a constructor.
   */
  virtual StatusCode initialize() = 0;
  virtual StatusCode sysInitialize() = 0;

  /** Finalization of the Tool. This method is called typically
   *  by the ToolSvc. For completeless this method is called before
   *  the tool is destructed.
   */
  virtual StatusCode finalize() = 0;
  virtual StatusCode sysFinalize() = 0;

  /** Current number of reference counts. 
      Avoids having to call addRef() + release() to get current refCount.
  */
  virtual unsigned long refCount() const = 0;
};

#endif  // GAUDIKERNEL_IALGTOOL_H
