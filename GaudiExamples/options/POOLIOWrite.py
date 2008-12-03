# $Id: POOLIOWrite.py,v 1.5 2008/11/04 22:49:25 marcocle Exp $ 
##############################################################
# Write a DST and a miniDST, including File Summary Records
##############################################################
from Gaudi.Configuration import *

#--Setup for ROOT I/O System
importOptions('GaudiPoolDbRoot.opts')

MessageSvc().OutputLevel     =  VERBOSE
IncidentSvc().OutputLevel    =  DEBUG

FileCatalog().Catalogs = [ "xmlcatalog_file:POOLIO.xml" ]

RootDst  = OutputFSRStream('RootDst',
                           ItemList = [ "/Event#999" ],
                           FSRItemList = [ "/Event/FSR#999" ],
                           Output   = "DATAFILE='PFN:POOLIO.dst'  TYP='POOL_ROOTTREE' OPT='RECREATE'" )
RootMini = OutputFSRStream('RootMini',
                           ItemList = [ "/Event#1" ],
                           FSRItemList = [ "/Event/FSR#999" ],
                           Output   = "DATAFILE='PFN:POOLIO.mdst' TYP='POOL_ROOTTREE' OPT='RECREATE'" )
                         
from GaudiExamples.GaudiExamplesConf import WriteAlg, ReadAlg

ApplicationMgr( EvtMax   = 2,
                EvtSel   = 'NONE',
                HistogramPersistency = 'NONE',
                OutStream = [ RootDst, RootMini ],
                TopAlg    = [ WriteAlg(), ReadAlg() ])


