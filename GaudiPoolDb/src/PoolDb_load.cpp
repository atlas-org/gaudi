// $Id: PoolDb_load.cpp,v 1.2 2005/09/15 14:48:34 hmd Exp $
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

DECLARE_FACTORY_ENTRIES(GaudiPoolDb) 
{
  DECLARE_OBJECT( PoolDbCacheSvc     );
  DECLARE_OBJECT( PoolDbCnvSvc       );
  DECLARE_OBJECT( PoolDbBaseCnv      );
  DECLARE_OBJECT( PoolDbListCnv      );
  DECLARE_OBJECT( PoolDbVectorCnv    );
  DECLARE_OBJECT( PoolDbKeyedMapCnv  );
  DECLARE_OBJECT( PoolDbKeyedArrayCnv);
  DECLARE_OBJECT( PoolDbKeyedHashMapCnv );
  DECLARE_OBJECT( PoolDbUpdateableCnv);
  DECLARE_OBJECT( PoolDbEvtSelector  );
  DECLARE_OBJECT( PoolDbDatabaseCnv  );
  DECLARE_OBJECT( PoolDbDirectoryCnv );
  DECLARE_OBJECT( PoolDbRWNTupleCnv  );
  DECLARE_OBJECT( PoolDbCWNTupleCnv  );
} 
