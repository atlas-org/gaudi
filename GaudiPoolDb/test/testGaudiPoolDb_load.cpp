// $Id: testGaudiPoolDb_load.cpp,v 1.3 2007/02/08 10:12:14 hmd Exp $
//====================================================================
//  PoolDb_load.cpp
//--------------------------------------------------------------------
//
//  Package    : PoolDb
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
void* __init_TGFALFile ();
#ifdef __SCHEMA_CHANGE__
DECLARE_FACTORY_ENTRIES(schemaGaudiPoolDb)
#else
DECLARE_FACTORY_ENTRIES(testGaudiPoolDb)
#endif
{
  DECLARE_ALGORITHM(ReadAlg);
  DECLARE_ALGORITHM(WriteAlg);
  DECLARE_ALGORITHM(EvtCollectionWrite);
  DECLARE_OBJECT(EvtCollectionSelector);
  DECLARE_CONVERTER(MyTrackContainerCnv);
  __init_TGFALFile();
} 
