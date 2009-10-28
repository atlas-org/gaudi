#--------------------------------------------------------------
# PartPropSvc Service options
#--------------------------------------------------------------
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()
PartPropSvc.InputFile = "PartPropSvc/PDGTABLE.MeV=PDG" 

#theApp.Dlls += [ "PartPropSvc" ]
#theApp.ExtSvc += ["PartPropSvc"]
#PartPropSvc = Service( "PartPropSvc" )
#PartPropSvc.InputType = "PDG"
#PartPropSvc.InputFile = ["PDGTABLE.MeV"]
#--------------------------------------------------------------
