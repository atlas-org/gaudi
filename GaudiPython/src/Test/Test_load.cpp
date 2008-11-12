// $Id: Test_load.cpp,v 1.3 2005/10/27 16:57:57 mato Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES(GPyTest) {
  DECLARE_ALGORITHM(HelloWorld)
  DECLARE_ALGORITHM(RandomNumberAlg)
  DECLARE_ALGORITHM(WriteAlg)
  DECLARE_ALGORITHM(PropertyAlg)
}
