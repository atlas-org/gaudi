# $Id: POOLIORead.py,v 1.5 2008/02/27 20:02:27 marcocle Exp $ 
# ============================================================================
################################
# Example options file.
################################
#
from Gaudi.Configuration import *

importOptions("GaudiPoolDbRoot.opts")

from Configurables import ReadAlg

ApplicationMgr( TopAlg      = [ ReadAlg( OutputLevel = DEBUG ) ],
                HistogramPersistency = 'NONE',
                EvtMax      = 2000
              )
                
EventSelector( OutputLevel  = DEBUG, 
               PrintFreq    = 50,
               FirstEvent   = 1,
               Input        = [ "DATAFILE='PFN:POOLIO.dst'  TYP='POOL_ROOT' OPT='READ'",
                                "DATAFILE='PFN:POOLIO.mdst' TYP='POOL_ROOT' OPT='READ'" ]
              )

PoolDbCacheSvc( OutputLevel = WARNING )

FileCatalog( Catalogs = ["xmlcatalog_file:POOLIO.xml" ] )

