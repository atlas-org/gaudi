###############################################################
# Job options file
#==============================================================
from Gaudi.Configuration import *
from Configurables import MyDataAlgorithm, DataCreator

importOptions('Common.opts')
MessageSvc().OutputLevel = INFO

mdigi = DataCreator('MuonDigits', Data = 'Rec/Muon/Digits')
mfoo  = DataCreator('MuonFoos',   Data = 'Rec/Muon/Foos')
dondem = DataOnDemandSvc( UsePreceedingPath = True,
                          NodeMap = {'Rec': 'DataObject' , 
                                     'Rec/Muon' : 'DataObject' },
                          AlgMap  = { mdigi.Data : mdigi.getFullName() ,
                                      mfoo.Data  : mfoo.getFullName() } )
pcache = PoolDbCacheSvc( Dlls = ['GaudiKernelDict'] )

ApplicationMgr( TopAlg = [ MyDataAlgorithm() ],
                ExtSvc = [ pcache, dondem ],
                EvtMax = 10,
                EvtSel = 'NONE' )
                
PoolDbCacheSvc().Dlls = ['GaudiKernelDict']
EventDataSvc().EnableFaultHandler = True

