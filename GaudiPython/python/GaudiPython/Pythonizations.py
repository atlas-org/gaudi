# File: GaudiPython/Pythonizations.py
# Author: Pere Mato (pere.mato@cern.ch)

""" This Pythonizations module provides a number of useful pythonizations
    of adaptation of some classes.
"""

__all__ = [ ]

import PyCintex

#--- Hack to match the name scheme of dictionary on Linux ----------------------------
_loadDict_save = PyCintex.loadDict
def _loadDict(name):
  import sys
  if sys.platform != 'win32' and name[:3] != 'lib' : name = 'lib'+name
  return _loadDict_save(name)
PyCintex.loadDict = _loadDict
 
#--- Adding extra functionality to C++ raw classes------------------------------------
def _printHisto1D(h) :
  x = h.axis()
  return  'Histogram 1D "%s" %d bins [%f,%f]' % (h.title(), x.bins(), x.lowerEdge(), x.upperEdge())
def _contentsHisto1D(h) :
  x = h.axis()
  return  map(h.binEntries, range(x.bins()))
def _printHisto2D(h) :
  x,y = h.xAxis(),h.yAxis()
  return  'Histogram 2D "%s" %d xbins [%f,%f], %d ybins [%f,%f]' % \
  (h.title(), x.bins(), x.lowerEdge(), x.upperEdge(), y.bins(), y.lowerEdge(), y.upperEdge() )
def _printStatusCode(s) :
  if s.isSuccess() : return 'SUCCESS'
  else             : return 'FAILURE'
def _printBitReference(b) :
  return str(1==b.bool())
def _printFillStream(o) :
  s = gbl.stringstream()
  o.fillStream(s)
  out = s.str()
  if out == '' :
    out = o.__class__.__name__ + ' object'
    if hasattr( o, 'hasKey') and o.hasKey() :
      out += ' key = '+ str(o.key())
  return out
def _container__getitem__(self, k) :
  return self.containedObject(k)
def _container__len__(self) :
  return self.numberOfObjects()
def _container__iter__(self) :
  if hasattr(self,'containedObjects') : sequential = self.containedObjects()
  else                                : sequential = self
  count = 0
  limit = self.__len__()
  while count < limit :
    yield sequential.__getitem__(count)
    count += 1

gbl = PyCintex.gbl
gbl.AIDA.IHistogram1D.__repr__ = _printHisto1D
gbl.AIDA.IHistogram1D.contents = _contentsHisto1D
gbl.AIDA.IHistogram2D.__repr__ = _printHisto2D
gbl.StatusCode.__repr__ = _printStatusCode
try: gbl._Bit_reference.__repr__ = _printBitReference
except: pass
gbl.ContainedObject.__repr__ = _printFillStream
gbl.DataObject.__repr__ = _printFillStream
gbl.ObjectContainerBase.__getitem__ = _container__getitem__
gbl.ObjectContainerBase.__len__ = _container__len__
gbl.ObjectContainerBase.__iter__ = _container__iter__

gbl.IUpdateManagerSvc.update = lambda self,obj: gbl.IUpdateManagerSvc.PythonHelper.update(self,obj)
gbl.IUpdateManagerSvc.invalidate = lambda self,obj: gbl.IUpdateManagerSvc.PythonHelper.invalidate(self,obj)

#---Globals--------------------------------------------------------------------
gbl.StatusCode.SUCCESS = 1 
gbl.StatusCode.FAILURE = 0
