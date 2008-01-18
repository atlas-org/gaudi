// $Id: HbookCnv_load.cpp,v 1.7 2006/01/19 08:41:14 mato Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"


// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(HbookCnv) {
  DECLARE_SERVICE( HbookCnvPersSvc );
  DECLARE_SERVICE( HbookCnvConvSvc );
  // Histogram related stuff
  DECLARE_CONVERTER( HbookCnvDirectoryCnv );
  DECLARE_CONVERTER( HbookCnvH1DCnv );
  DECLARE_CONVERTER( HbookCnvH2DCnv );
  // NTuple related stuff
  DECLARE_CONVERTER( HbookCnvHFileCnv );
  DECLARE_CONVERTER( HbookCnvHDirectoryCnv );
  DECLARE_CONVERTER( HbookCnvHCWNTupleCnv );
  DECLARE_CONVERTER( HbookCnvHRWNTupleCnv );
  DECLARE_TOOL  ( HbookTool  ) ;
};

