# $Id: POOLIOWrite.py,v 1.4 2008/02/27 20:02:27 marcocle Exp $ 
# ============================================================================
################################
# Example options file.
################################
from Gaudi.Configuration import *

#--Setup for ROOT I/O System
importOptions('GaudiPoolDbRoot.opts')

MessageSvc().OutputLevel     =  VERBOSE
FileCatalog().Catalogs = [ "xmlcatalog_file:POOLIO.xml" ]

RootDst  = OutputStream('RootDst',
                        ItemList = [ "/Event#999" ],
                        Output   = "DATAFILE='PFN:POOLIO.dst'  TYP='POOL_ROOTTREE' OPT='RECREATE'" )
RootMini = OutputStream('RootMini',
                        ItemList = [ "/Event#1" ],
                        Output   = "DATAFILE='PFN:POOLIO.mdst' TYP='POOL_ROOTTREE' OPT='RECREATE'" )
                         
from GaudiExamples.GaudiExamplesConf import WriteAlg, ReadAlg

ApplicationMgr( EvtMax   = 2,
                EvtSel   = 'NONE',
                HistogramPersistency = 'NONE',
                OutStream = [ RootDst, RootMini ],
                TopAlg    = [ WriteAlg(), ReadAlg() ])


