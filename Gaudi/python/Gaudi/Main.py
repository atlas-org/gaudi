import sys, os
from Gaudi import Configuration
from GaudiKernel.ConfigurableDb import cfgDb, loadConfigurableDb, getConfigurable
import logging

log = logging.getLogger(__name__)

class gaudimain(object) :
    def __init__(self) :
        from Configurables import ApplicationMgr
        appMgr = ApplicationMgr()
        if "GAUDIAPPNAME" in os.environ:
            appMgr.AppName = str(os.environ["GAUDIAPPNAME"])
        if "GAUDIAPPVERSION" in os.environ:
            appMgr.AppVersion = str(os.environ["GAUDIAPPVERSION"])
    
    def generatePyOutput(self, all = False):
        from pprint import pformat
        conf_dict = Configuration.configurationDict(all)
        return pformat(conf_dict)

    def generateOptsOutput(self, all = False):
        from pprint import pformat
        conf_dict = Configuration.configurationDict(all)
        out = []
        names = conf_dict.keys()
        names.sort()
        for n in names:
            props = conf_dict[n].keys()
            props.sort()
            for p in props:
                out.append('%s.%s = %s;' % (n,p, repr(conf_dict[n][p])))
        return "\n".join(out)

    def _writepickle(self, filename) :
        #--- Lets take the first file input file as the name of the pickle file
        import pickle
        output = open(filename, 'wb')
        # Dump only the the configurables that make sense to dump (not User ones)
        from GaudiKernel.Proxy.Configurable import getNeededConfigurables
        to_dump = {}
        for n in getNeededConfigurables():
            to_dump[n] = Configuration.allConfigurables[n]
        pickle.dump(to_dump, output, -1)
        output.close()

    def printconfig(self, old_format = False, all = False) :
        msg = 'Dumping all configurables and properties'
        if not all:
            msg += ' (different from default)'
        log.info(msg)
        conf_dict = Configuration.configurationDict(all)
        if old_format:
            print self.generateOptsOutput(all)
        else:
            print self.generatePyOutput(all)

    def writeconfig(self, filename, all = False):
        write = { ".pkl" : lambda filename, all: self._writepickle(filename),
                  ".py"  : lambda filename, all: open(filename,"w").write(self.generatePyOutput(all) + "\n"),
                  ".opts": lambda filename, all: open(filename,"w").write(self.generateOptsOutput(all) + "\n"),
                }
        from os.path import splitext
        ext = splitext(filename)[1]
        if ext in write:
            write[ext](filename, all)
        else:
            log.error("Unknown file type '%s'. Must be any of %r.", ext, write.keys())
            sys.exit(1)

    def run(self) :
        #--- Instantiate the ApplicationMgr------------------------------
        import GaudiPython
        self.g = GaudiPython.AppMgr()
        success = self.g.run(self.g.EvtMax).isSuccess()
        success = self.g.exit().isSuccess() and success
        if not success:
            return 1
        return 0
