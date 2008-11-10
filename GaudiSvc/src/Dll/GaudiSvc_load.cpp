// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiSvc/src/Dll/GaudiSvc_load.cpp,v 1.33 2007/02/23 10:42:42 hmd Exp $

// Include Files
#include "GaudiKernel/DeclareFactoryEntries.h"

// Using the MACROS in the previous include file declare all "components" of
// the library.
DECLARE_FACTORY_ENTRIES(GaudiSvc) {
  DECLARE_OBJECT( ApplicationMgr );
  DECLARE_SERVICE( AlgContextSvc ); 
  DECLARE_SERVICE( ChronoStatSvc );
  DECLARE_SERVICE( CounterSvc );
  DECLARE_SERVICE( DetDataSvc );
  DECLARE_SERVICE( EvtDataSvc );
  DECLARE_SERVICE( ExceptionSvc );
  DECLARE_SERVICE( FastContainersSvc );
  DECLARE_SERVICE( MultiStoreSvc );
  DECLARE_ALGORITHM( StoreExplorerAlg );
  DECLARE_ALGORITHM( CollectionCloneAlg );
  DECLARE_ALGORITHM( PartitionSwitchAlg );
  DECLARE_TOOL( PartitionSwitchTool );
  DECLARE_SERVICE( HistogramSvc );
  DECLARE_SERVICE( HistorySvc );
  DECLARE_SERVICE( JobOptionsSvc );
  DECLARE_SERVICE( MessageSvc );
  DECLARE_SERVICE( NTupleSvc );
  DECLARE_SERVICE( IssueLogger );
  DECLARE_SERVICE( TagCollectionSvc );
  DECLARE_SERVICE( THistSvc );
  DECLARE_SERVICE( RndmGenSvc );
  DECLARE_SERVICE( ParticlePropertySvc );
  DECLARE_SERVICE( HistogramPersistencySvc );
  DECLARE_SERVICE( EvtPersistencySvc );
  DECLARE_SERVICE( DetPersistencySvc );
  DECLARE_SERVICE( EventSelector );
  DECLARE_SERVICE( EventCollectionSelector );
  DECLARE_SERVICE( IncidentSvc );
  DECLARE_SERVICE( DataOnDemandSvc );
  DECLARE_SERVICE( AuditorSvc );
  DECLARE_SERVICE( ToolSvc );
  DECLARE_SERVICE( StatusCodeSvc );
  DECLARE_SERVICE( MinimalEventLoopMgr );
  DECLARE_SERVICE( EventLoopMgr );
  DECLARE_SERVICE( AppMgrRunable );
  DECLARE_ALGORITHM( EvtCollectionStream );
  DECLARE_ALGORITHM( OutputStream );
  DECLARE_ALGORITHM( InputCopyStream );
  DECLARE_ALGORITHM( TagCollectionStream );
//   DECLARE_OBJECT( AIDATupleSvc);
  // Random number stuff
  DECLARE_SERVICE( DualRandEngine );
  DECLARE_SERVICE( TripleRandEngine );
  DECLARE_SERVICE( DRand48Engine );
  DECLARE_SERVICE( Hurd160Engine );
  DECLARE_SERVICE( Hurd288Engine );
  DECLARE_SERVICE( HepJamesRandom );
  DECLARE_SERVICE( MTwistEngine );
  DECLARE_SERVICE( RanecuEngine );
  DECLARE_SERVICE( Ranlux64Engine );
  DECLARE_SERVICE( RanluxEngine );
  DECLARE_SERVICE( RanshiEngine );
  DECLARE_OBJECT( RndmBit );
  DECLARE_OBJECT( RndmFlat );
  DECLARE_OBJECT( RndmChi2 );
  DECLARE_OBJECT( RndmGamma );
  DECLARE_OBJECT( RndmGauss );
  DECLARE_OBJECT( RndmLandau );
  DECLARE_OBJECT( RndmPoisson );
  DECLARE_OBJECT( RndmStudentT );
  DECLARE_OBJECT( RndmBinomial );
  DECLARE_OBJECT( RndmExponential );
  DECLARE_OBJECT( RndmBreitWigner );
  DECLARE_OBJECT( RndmBreitWignerCutOff );
  DECLARE_OBJECT( RndmDefinedPdf );
  DECLARE_OBJECT( RndmGaussianTail );
  DECLARE_OBJECT( NTupleSelector );
  // Histogram factories
  DECLARE_DATAOBJECT( H1D );
  DECLARE_DATAOBJECT( H2D );
  DECLARE_DATAOBJECT( H3D );
  DECLARE_DATAOBJECT( P1D );
  DECLARE_DATAOBJECT( P2D );
}

#include "GaudiKernel/DataStreamTool.h"

DECLARE_TOOL_FACTORY( DataStreamTool )
