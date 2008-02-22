// $Id: testGaudiPoolDb_dll.cpp,v 1.1 2006/08/21 14:19:24 hmd Exp $
//====================================================================
//  PoolDb_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : Gaudi
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : M.Frank
//  Created    : 13/1/99
//  Changes    : 
//
//====================================================================
// Include Files
#include "GaudiKernel/LoadFactoryEntries.h"

#ifdef __SCHEMA_CHANGE__
LOAD_FACTORY_ENTRIES(schemaGaudiPoolDb)
#else
LOAD_FACTORY_ENTRIES(testGaudiPoolDb)
#endif
