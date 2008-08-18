# File: AthenaCommon/python/Logging.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Athena Python-side logging."""

import sys
import Constants as Constants


### data _____________________________________________________________________
__version__ = '1.0.2'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'log', 'logging', 'AthError' ]



### basic logging and messages -----------------------------------------------
import logging


### configure logger module --------------------------------------------------
logging.VERBOSE = logging.DEBUG - 1
logging.ALL     = logging.DEBUG - 2
logging.addLevelName( logging.VERBOSE, 'VERBOSE' )
logging.addLevelName( logging.ALL, 'ALL' )


### specialized logger to allow setting with Gaudi message levels ============
class AthenaLogger( logging._loggerClass, object ):
   __levelmap = { Constants.ALL      : logging.ALL,
                  Constants.VERBOSE  : logging.VERBOSE,
                  Constants.DEBUG    : logging.DEBUG,
                  Constants.INFO     : logging.INFO,
                  Constants.WARNING  : logging.WARNING,
                  Constants.ERROR    : logging.ERROR,
                  Constants.FATAL    : logging.FATAL }

   def setLevel( self, level ):
      try:
         level = self.__levelmap[ level ]
      except KeyError:
         pass

      super( AthenaLogger, self ).setLevel( level )

   @classmethod
   def mapLevelLoggingToGaudi( self, level ):
      for k,v in self.__levelmap.items():
         if v == level:
            return k
      return level

   @classmethod
   def mapLevelGaudiToLogging( self, level ):
      try:
         return self.__levelmap[ level ]
      except KeyError:
         return level

   def all( self, msg, *args, **kwargs):
      if self.manager.disable >= logging.ALL:
         return
      if logging.ALL >= self.getEffectiveLevel():
         apply(self._log, (logging.ALL, msg, args), kwargs)

   def verbose( self, msg, *args, **kwargs):
      if self.manager.disable >= logging.VERBOSE:
         return
      if logging.VERBOSE >= self.getEffectiveLevel():
         apply(self._log, (logging.VERBOSE, msg, args), kwargs)


logging.setLoggerClass( AthenaLogger )


### configure logger ---------------------------------------------------------
log = logging.getLogger()
log.name = "GaudiKernel"
hdlr = logging.StreamHandler( sys.stdout )
frmt = logging.Formatter( "# Py:%(name)-14s%(levelname)8s %(message)s" )
hdlr.setFormatter( frmt )
for h in log.handlers:                 # basicConfig handler already installed?
   if h.formatter._fmt == logging.BASIC_FORMAT:
      log.removeHandler( h )
      break
log.addHandler( hdlr )

def AthError( descr ):
   raise RuntimeError( descr )               # <== Note exception!
