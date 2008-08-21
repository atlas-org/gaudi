// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiSvc/tests/src/component/GaudiSvcTest_load.cpp,v 1.1 2007/10/24 13:25:40 marcocle Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(GaudiSvcTest) {
  DECLARE_NAMESPACE_ALGORITHM(GaudiSvcTest, CounterTestAlg);
}
