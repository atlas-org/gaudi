# $Id: POOLIORead.py,v 1.6 2008/11/04 22:49:25 marcocle Exp $ 
####################################################################
# Read a DST, distinguishing between events and file summary records
####################################################################
#
from Gaudi.Configuration import *

importOptions("GaudiPoolDbRoot.opts")

from Configurables import ReadAlgFSR, ReadTES, GaudiSequencer, SequencerTimerTool, ChronoStatSvc

eventAlgs = GaudiSequencer( "EventAlgs",
                            Members = [ ReadAlgFSR( "ReadAlg", OutputLevel=DEBUG ) ],
                            VetoObjects = [ "FSR" ] )
fsrAlgs   = GaudiSequencer( "FSRAlgs",
                            Members = [ ReadTES( OutputLevel=DEBUG, Locations=["FSR"] ) ],
                            RequireObjects = ["FSR"] )

ApplicationMgr( TopAlg      = [ eventAlgs, fsrAlgs ],
                HistogramPersistency = 'NONE',
                EvtMax      = -1
              )
                
EventSelector( OutputLevel  = DEBUG, 
               PrintFreq    = 50,
               FirstEvent   = 1,
               Input        = [ "DATAFILE='PFN:POOLIO_FSR.dst'  TYP='POOL_ROOT' OPT='READ'",
                                "DATAFILE='PFN:POOLIO_FSR.mdst' TYP='POOL_ROOT' OPT='READ'" ]
              )

PoolDbCacheSvc( OutputLevel = WARNING )
ChronoStatSvc( OutputLevel = WARNING )
SequencerTimerTool( OutputLevel = WARNING )

FileCatalog( Catalogs = ["xmlcatalog_file:POOLIO.xml" ] )

