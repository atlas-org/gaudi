# File: Gaudi/python/Gaudi/Configuration.py
# Author: Pere Mato (per.mato@cern.ch)

from GaudiKernel.Constants import *
from GaudiKernel.Configurable import *
from GaudiKernel.ConfigurableDb import loadConfigurableDb, cfgDb
from GaudiKernel.ConfigurableDb import getConfigurable as confDbGetConfigurable
from CommonGaudiConfigurables import *
from GaudiKernel.ProcessJobOptions import importOptions
from GaudiKernel.ProcessJobOptions import InstallRootLoggingHandler as _InstallRootLoggingHandler

import logging
log = logging.getLogger("Gaudi.Configuration")
# Ensure that a root logging handler is always present.
_InstallRootLoggingHandler()

allConfigurables = Configurable.allConfigurables

def _fillConfDict():
    nFiles = loadConfigurableDb()
    log = logging.getLogger( 'PropertyProxy' ) 
    log.debug( "Read module info for %d configurables from %d genConfDb files",
              len(cfgDb), nFiles )
    if len(cfgDb.duplicates()) > 0:
        log.warning( "Found %d duplicates among the %d genConfDb files :",
                     len(cfgDb.duplicates()), nFiles )
        log.warning( "--------------------------------------------------" )
        log.warning( "  -%s: %s - %s",
                     "<component name>", "<module>", "[ <duplicates> ]" )
        log.warning( "--------------------------------------------------" )
        dups = cfgDb.duplicates()
        for cfgName in dups.keys():
            log.warning( "  -%s: %s - %s",
                         cfgName,
                         cfgDb[cfgName]['module'],
                         str([ d['module'] for d in dups[cfgName]]) )
            pass
        del dups
        log.warning( "Fix your cmt/requirements file !!" )
        pass
    else:
        log.debug( "No duplicates have been found: that's good !" )
        pass
    return

# fill the configurable dictionary at module load
_fillConfDict()

import os, sys

def importConfiguration(conf, local=locals()) : 
    local[conf] = confDbGetConfigurable(conf)

class ConfFacade(object) :
    def __init__(self) :
        pass
    def __getattr__(self, name) :
        if '_' == name[0] : return None
        else:               return confDbGetConfigurable(name)

def configurationDict(all=False):
    """Return a dictionary representing the configuration.
    The dictionary contains one entry per configurable which is a dictionary
    with one entry per property.
    The optional argument "all" is used to decide if to inluce only values
    different from the default or all of them.
    """
    from GaudiKernel.Proxy.Configurable import getNeededConfigurables
    
    catalog = allConfigurables
    keys = getNeededConfigurables() # use only interesting configurables
    conf_dict = {}
    if all:
        for n in keys :
            if n not in conf_dict:
                conf_dict[n] = {}
            for p, v in  catalog[n].getDefaultProperties().items() :
                conf_dict[n][p] = v
    
    for n in keys :
        if n not in conf_dict:
            conf_dict[n] = {}
        for p, v in catalog[n].getValuedProperties().items() :
            conf_dict[n][p] = v
    # purge empty configurables
    keys = conf_dict.keys()
    for n in keys:
        if not conf_dict[n]:
            del conf_dict[n]
    return conf_dict

def getConfigurable(name, defaultType = None):
    """Helper function to get a configurable with the given name regardless
    for the type.
    If defaultType can be a class derived from configurable or a string. If not
    specified, the tool name is used as type."""
    if name in allConfigurables:
        return allConfigurables[name]
    else:
        # if the configurable is not found, we need to instantiate it
        if defaultType is None:
            # try to use the name of the configurable as default type
            defaultType = name
        if type(defaultType) is str:
            # we need to convert from string to actual class
            if defaultType in globals():
                # We the type is defined in the global namespace
                defaultType = globals()[defaultType]
            else:
                # otherwise we try to get it from the Configurables database
                exec "from Configurables import %s" % defaultType
                defaultType = locals()[defaultType]
        return defaultType(name)

sys.modules['Configurables'] = ConfFacade()
