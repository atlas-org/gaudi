# File: Gaudi/python/Gaudi/Configuration.py
# Author: Pere Mato (per.mato@cern.ch)

from GaudiKernel.Constants import *
from GaudiKernel.Configurable import *
from GaudiKernel.Logging import logging
from GaudiKernel.ConfigurableDb import loadConfigurableDb, cfgDb, getConfigurable
from CommonGaudiConfigurables import *
from GaudiKernel.ProcessJobOptions import importOptions

allConfigurables = Configurable.allConfigurables

def _fillConfDict():
    nFiles = loadConfigurableDb()
    log = logging.getLogger( 'PropertyProxy' ) 
    log.info( "Read module info for %d configurables from %d genConfDb files",
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
        log.info( "No duplicates have been found: that's good !" )
        pass
    return

# fill the configurable dictionary at module load
_fillConfDict()

import os, sys

def importConfiguration(conf, local=locals()) : 
    local[conf] = getConfigurable(conf)

class ConfFacade(object) :
    def __init__(self) :
        pass
    def __getattr__(self, name) :
        if '_' == name[0] : return None
        else:               return getConfigurable(name)

def configurationDict(all=False):
    """Return a dictionary representing the configuration.
    The dictionary contains one entry per configurable which is a dictionary
    with one entry per property.
    The optional argument "all" is used to decide if to inluce only values
    different from the default or all of them.
    """
    catalog = allConfigurables
    keys = catalog.keys()
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

sys.modules['Configurables'] = ConfFacade()
