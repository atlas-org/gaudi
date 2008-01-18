// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiMonitor/src/Dll/GaudiMonitor_load.cpp,v 1.1.1.1 2007/01/15 13:56:38 hmd Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(GaudiMonitor) {
  DECLARE_SERVICE( DataListenerSvc );
}
