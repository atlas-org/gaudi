// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/LoadFactoryEntries.h,v 1.4 2006/11/30 14:43:13 mato Exp $
#ifndef GAUDIKERNEL_LOADFACTORYENTRIES_H
#define GAUDIKERNEL_LOADFACTORYENTRIES_H 1
//====================================================================
//  LoadFactoryEntries.h
//--------------------------------------------------------------------
//
//  Package    : GaudiKernel
//
//  Description: Implementation of getFactoryEntries() routine.
//               This routine is needed for forcing the linker
//               to load all the components of the library. 
//
//====================================================================

// DllMain entry point
#include "GaudiKernel/DllMain.icpp"

#if !defined(__APPLE__)
  void GaudiDll::initialize(void*) {}
  void GaudiDll::finalize(void*) {}
#endif

#define LOAD_FACTORY_ENTRIES(x) \
extern "C" FACTORYTABLE_API void* x##_getFactoryEntries() {  \
  return 0; \
}

#endif // GAUDIKERNEL_LOADFACTORYENTRIES_H
