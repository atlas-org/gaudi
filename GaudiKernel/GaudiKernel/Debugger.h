//====================================================================
//	Debugger.h
//--------------------------------------------------------------------
//
//	Package    : System (The LHCb System service)
//
//  Description: Invoke interactively the debugger from a 
//               running application
//
//	Author     : M.Frank
//  Created    : 13/1/99
//	Changes    : 
//====================================================================
#ifndef GAUDI_SYSTEM_DEBUGGER_H
#define GAUDI_SYSTEM_DEBUGGER_H

namespace System  {
  /// Break the execution of the application and invoke the debugger
  long breakExecution();
  /// Break the execution of the application and invoke the debugger in a remote process
  long breakExecution(long pid);
}
#endif    // GAUDI_SYSTEM_DEBUGGER_H
