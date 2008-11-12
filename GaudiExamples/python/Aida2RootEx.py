#!/usr/bin/env python2.4
# =============================================================================
# $Id: Aida2RootEx.py,v 1.3 2007/08/07 13:00:28 marcocle Exp $
# =============================================================================
# CVS tag $Name:  $ ,version $Revision: 1.3 $
# =============================================================================
## @file
#  Simple algorith to illustrate the usage of aida2root converter
#  @see Gaudi::Aida2ROOT
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-01-24
# =============================================================================
" Simple example to illustrate the usage of aida2root converter "
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@phys.syr.edu"
# =============================================================================
import os,ROOT, gaudimodule
from   GaudiAlgs   import HistoAlgo

SUCCESS   = gaudimodule.SUCCESS

## @see Gaudi::Aida2ROOT 
aida2root = gaudimodule.gbl.Gaudi.Utils.Aida2ROOT.aida2root

## list of booked histograms 
paths = [ 'HistoEx/1'  , 'HistoEx/2'  , 'HistoEx/3'  ,
          'HistoEx1/1' , 'HistoEx1/2' , 'HistoEx1/3' ,
          'HistoEx2/1' , 'HistoEx2/2' , 'HistoEx2/3' ,
          'HistoEx2/4' , 'HistoEx2/5' , 'HistoEx2/6' ]


# =============================================================================
## @class Aida2RootEx1
#  Simple algorithm which used aida2root utility 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-01-24
class Aida2RootEx1 (HistoAlgo) :
    ## Standard Constructor 
    def __init__ ( self , name = "Aida2RootEx1" ) :
        """ Standard Constructor """
        HistoAlgo.__init__( self , name )
        
    ## the main excution method
    def execute ( self ) :
        " The main excution method " 
            
        # list of booked histograms 
        for path in paths :
            self.Print("AIDA object: '%s'" % path )
            # get AIDA pointer 
            aida = self.histoSvc( path )
            if not aida : return self.Error ( "Invalid AIDA at '%s'" % path )
            # explicitely convert to ROOT 
            root = aida2root( aida )
            if not root : return self.Error ( "Invalid conversion to ROOT '%s'" % path )
            # use the native ROOT printout 
            root.Print()
        
        return SUCCESS 
# =============================================================================

# =============================================================================
## @class Aida2RootEx2
#  Simple algorithm which uses aida2root utility 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-01-24
class Aida2RootEx2 (HistoAlgo) :
    ## Standard Constructor 
    def __init__ ( self , name = "Aida2RootEx2" ) :
        """ Standard Constructor """
        HistoAlgo.__init__( self , name )

    ## the main excution method
    def execute ( self ) :
        " The main execution method "

        # get the service itself 
        s = self.histoSvc()

        for path in paths :            
            self.Print("AIDA object: '%s'" % path )
            root = s.getAsROOT( path )
            if not root : return self.Error ( "Invalid conversion to ROOT '%s'" % path )
            # use the native ROOT printout 
            root.Print()

        return SUCCESS 
# =============================================================================


# =============================================================================
## @fn configure
#  The main configuration method
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-01-24
def configure( gaudi = None ) :
    """ the main configuration method """
    
    if not gaudi : gaudi = gaudimodule.AppMgr()
    
    # reuse the previous example 
    import HistoEx2
    HistoEx2.configure( gaudi )

    # create the algorithms 
    alg1 = Aida2RootEx1()
    alg2 = Aida2RootEx2()
    # append them to the list of Top-Level algorithms 
    gaudi.addAlgorithm( alg1 )
    gaudi.addAlgorithm( alg2 )

    return SUCCESS 


# =============================================================================
## @fn useScript
#  the third way to convert AIDA histograms into ROOT
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-01-24
def useScript( histos ) :
    "  the third way to convert AIDA hoistograms into ROOT "
    
    g    = gaudimodule.AppMgr() 
    hsvc = g.histsvc()
    
    for histo in histos :
        root   = hsvc.getAsROOT(histo)
        if not root :
            print "ERROR in access the histogram '%s' "%histo
            continue
        canvas = ROOT.TCanvas('canvas',histo,250,250)
        root.Draw()
        name = histo.replace ('/','_')
        name = name.replace  ('\\','_')
        name = name.replace  ('"','_')
        name = name.replace  ("'",'_')
        name = name.replace  ("'",'_')
        name = name.replace(os.sep,'_') + '.gif'
        canvas.Print(name)
        print "The file name is '%s'"%name
        
# =============================================================================


# =============================================================================
# The actual job excution 
# =============================================================================
if '__main__' == __name__ :
    print __doc__ , __author__
    gaudi = gaudimodule.AppMgr()
    configure( gaudi )
    gaudi.run(5)

    # use the scrtipts
    useScript( paths )
    
# =============================================================================
# The END 
# =============================================================================
