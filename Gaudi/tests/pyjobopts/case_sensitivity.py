from Gaudi.Configuration import *

class MyAlgType(ConfigurableUser):
    __slots__ = { "TestProperty": "None",
                  "AnotherTest" :"None" }

# Instantiate the configurable before setting properties in old opts
MyAlgType("MyAlg1")

importOptions("case_sensitivity.opts")

# Instantiate the configurable after setting properties in old opts
MyAlgType("MyAlg2")
