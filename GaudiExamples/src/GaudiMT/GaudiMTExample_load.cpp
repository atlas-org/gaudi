// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiExamples/src/GaudiMT/GaudiMTExample_load.cpp,v 1.1 2003/02/26 01:52:55 calaf Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(GaudiMTExample) {
  DECLARE_OBJECT( MTEventLoopMgr );
  DECLARE_OBJECT( MTMessageSvc ); 
  DECLARE_ALGORITHM( MTHelloWorld );
}

