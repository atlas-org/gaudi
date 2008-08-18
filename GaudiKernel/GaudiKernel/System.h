// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/System.h,v 1.13 2006/11/29 18:46:30 hmd Exp $
#ifndef GAUDIKERNEL_SYSTEM_H
#define GAUDIKERNEL_SYSTEM_H

// Framework include files
#include "GaudiKernel/Kernel.h"
// STL include files
#include <string>
#include <vector>
#include <typeinfo>

#include "GaudiKernel/Timing.h"
#include "GaudiKernel/ModuleInfo.h"

#ifdef __linux
# include <pthread.h>
#ifndef __APPLE__
# include <execinfo.h>
#endif
#endif

/** Note: OS specific details as well as Gaudi details may not occur
    in this definition file, because this header is the used by both, the 
    OS specific implementations and the gaudi specific implementation.
    Since e.g. IID is defined in both, this would lead automatically to
    complete comilation failures.....

    @author M.Frank
*/
namespace System  {
  /// Definition of an image handle
  typedef void* ImageHandle;
  /// Definition of the process handle
  typedef void* ProcessHandle;
  /// Definition of the "generic" DLL entry point function
  typedef unsigned long (*EntryPoint)(const unsigned long iid, void** ppvObject);
  /// Definition of the "generic" DLL entry point function
  typedef void* (*Creator)();
  /// Load dynamic link library
  unsigned long loadDynamicLib(const std::string& name, ImageHandle* handle);
  /// unload dynamic link library
  unsigned long unloadDynamicLib(ImageHandle handle);
  /// Get a specific function defined in the DLL
  unsigned long getProcedureByName(ImageHandle handle, const std::string& name, EntryPoint* pFunction);
  /// Get a specific function defined in the DLL
  unsigned long getProcedureByName(ImageHandle handle, const std::string& name, Creator* pFunction);
  /// Get last system known error
  unsigned long getLastError();
  /// Get last system error as string
  const std::string getLastErrorString();
  /// Retrieve error code as string for a given error
  const std::string getErrorString(unsigned long error);
  /// Get platform independent information about the class type
  const std::string typeinfoName( const std::type_info& );
  const std::string typeinfoName( const char* );
  /// Host name
  const std::string& hostName();
  /// OS name
  const std::string& osName();
  /// OS version
  const std::string& osVersion();
  /// Machine type
  const std::string& machineType();
  /// User login name
  const std::string& accountName();
  /// Number of arguments passed to the commandline
  long numCmdLineArgs();
  /// Number of arguments passed to the commandline (==numCmdLineArgs()); just to match argv call...
  long argc();
  /// Command line arguments including executable name as arg[0] as vector of strings
  const std::vector<std::string> cmdLineArgs();
  ///char** command line arguments including executable name as arg[0]; You may not modify them!
  char** argv();
  ///get a particular environment variable
  const std::string getEnv(const char* var);
  ///get all environment variables
  const std::vector<std::string> getEnv();
  ///Set an environment variables.
  ///If value is empty, the variable is removed from the environment.
  ///When overwrite is 0, the variable is not set if already present.
  ///Returns 0 on success, -1 on failure.
  ///See man 3 setenv.
  int setEnv(const std::string &name, const std::string &value, int overwrite = 1);
#ifdef __linux
  ///A Thread handle
  typedef pthread_t ThreadHandle;
  ///thread handle "accessor"
  inline ThreadHandle threadSelf() { return pthread_self(); }
#else
  ///A Thread handle
  typedef void* ThreadHandle;
  ///thread handle "accessor"
  inline ThreadHandle threadSelf() { return (void*)0; }
#endif
  bool backTrace(void** addresses, const size_t depth);
  bool getStackLevel(void* addresses, void*& addr, std::string& fnc, std::string& lib) ;
}
#endif    // SYSTEM_SYSTEM_H
