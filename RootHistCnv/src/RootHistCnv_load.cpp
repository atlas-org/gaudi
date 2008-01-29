// $Id: RootHistCnv_load.cpp,v 1.8 2006/01/19 08:34:48 mato Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"


// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(RootHistCnv) {
  DECLARE_SERVICE( RootHistCnvPersSvc );
  // Histogram related stuff
  DECLARE_CONVERTER( RootHistCnvDirectoryCnv );
  DECLARE_CONVERTER( RootHistCnvH1DCnv );
  DECLARE_CONVERTER( RootHistCnvH2DCnv );
  DECLARE_CONVERTER( RootHistCnvH3DCnv );
  DECLARE_CONVERTER( RootHistCnvP1DCnv );
  DECLARE_CONVERTER( RootHistCnvP2DCnv );
  // NTuple related stuff
  DECLARE_CONVERTER( RootHistCnvRFileCnv );
  DECLARE_CONVERTER( RootHistCnvRDirectoryCnv );
  DECLARE_CONVERTER( RootHistCnvRCWNTupleCnv );
  DECLARE_CONVERTER( RootHistCnvRRWNTupleCnv );
}
