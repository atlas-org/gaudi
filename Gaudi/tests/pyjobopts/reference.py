from Gaudi.Configuration import *

class MyAlgType(ConfigurableUser):
    __slots__ = { "Cut": 0.0,
                  "Tool1": "None",
                  "Tool2": "None",
                  "File" : "None"
                }

class MyToolType(ConfigurableUser):
    __slots__ = { "Cut": 0.0 }

tool = MyToolType()
alg = MyAlgType("MyAlg")

alg.Tool2 = tool

importOptions("reference.opts")

alg.Cut = 20.0
