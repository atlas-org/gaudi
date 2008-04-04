import sys, os
from Gaudi import Configuration
from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb, getConfigurable

class gaudimain(object) :
  def __init__(self) :
      from Configurables import ApplicationMgr
      appMgr = ApplicationMgr()
      if "GAUDIAPPNAME" in os.environ:
          appMgr.AppName = str(os.environ["GAUDIAPPNAME"])
      if "GAUDIAPPVERSION" in os.environ:
          appMgr.AppVersion = str(os.environ["GAUDIAPPVERSION"])
  
  def configure(self, configfiles) :
    #----Run the configuration scripts-------------------------------
    for file in configfiles :
      Configuration.importOptions(file)
      
  def dumpconfig(self, old_format = False, all = False) :
        if old_format:
            print '//',
        else:
            print '#',
        print 'Dumping all configurables and properties',
        if all:
            print ""
        else:
            print '(different from default)'
        
        conf_dict = Configuration.configurationDict(all)
        if old_format:
            names = conf_dict.keys()
            names.sort()
            for n in names:
                props = conf_dict[n].keys()
                props.sort()
                for p in props:
                    print '%s.%s = %s;' % (n,p, repr(conf_dict[n][p]))
        else:
            from pprint import PrettyPrinter
            pp = PrettyPrinter()
            pp.pprint(conf_dict)

  def writepickle(self, filename) :
    #--- Lets take the first file input file as the name of the pickle file
    import pickle
    output = open(filename, 'wb')
    pickle.dump(Configuration.allConfigurables, output, -1)
    output.close()
    
  def run(self) :
    #--- Instantiate the ApplicationMgr------------------------------
    import GaudiPython
    self.g = GaudiPython.AppMgr()
    self.g.run(self.g.EvtMax)

