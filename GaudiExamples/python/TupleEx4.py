#!/usr/bin/env python
# =============================================================================
## @file
#  Simple exmaple to illustrate the usage ofsmart and friendly
#  N-Tuples outside of algoruthm-scope in "script-like" environment
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-07-04
# =============================================================================
"""
Simple exmaple to illustrate the usage ofsmart and friendly
N-Tuples outside of algoruthm-scope in 'script-like' environment
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

import GaudiPython
import GaudiPython.TupleUtils as TupleUtils

## get the application manager   (create if needed) 
gaudi = GaudiPython.AppMgr() 


gaudi.EvtSel               = 'NONE' ## no external event input
#gaudi.HistogramPersistency = 'HBOOK' ## define the persistency type for N-tuples
gaudi.HistogramPersistency = 'ROOT' ## define the persistency type for N-tuples

toolSvc = gaudi.toolsvc()
#print 'tool service: ', toolSvc

## configure N-Tuple Service 
## ntSvc = gaudi.service ( 'NTupleSvc' )
ntSvc = GaudiPython.iService ( 'NTupleSvc' )
## define 3 output files in 'NEW' mode
ntSvc.Output = [ "MYLUN1 DATAFILE='TupleEx4_1.root' OPT='NEW'" ,
                 "MYLUN2 DATAFILE='TupleEx4_2.root' OPT='NEW'" ,
                 "MYLUN3 DATAFILE='TupleEx4_3.root' OPT='NEW'" ]

## configure & initiliallize 
gaudi.config()
gaudi.initialize()

## get some random numbers
Rndm        = GaudiPython.gbl.Rndm
IRndmGenSvc = GaudiPython.gbl.IRndmGenSvc
rndmSvc     = gaudi.service('RndmGenSvc',IRndmGenSvc) 
if not rndmSvc : gaudi.createSvc('RndmGenSvc') 
rndmSvc     = gaudi.service('RndmGenSvc',IRndmGenSvc) 

gauss       = Rndm.Numbers ( rndmSvc , Rndm.Gauss ( 0.0 , 1.0 ) )

    
## get the first N-tuple
tup1 = TupleUtils.nTuple( "path"             , ## the path 
                          "It is a title for my n-tuple" , ## the title 
                          LUN = 'MYLUN1'                 ) ## logical unit  
## fill it:
for i in xrange(0,5000) :
    tup1.column ( 'i'  , i           )  ## int 
    tup1.column ( 'g1' , gauss()     )  ## double 
    tup1.column ( 'g2' , gauss()     )  ## double
    b = 0 < gauss() 
    tup1.column ( 'b1' , b           )  ## bool 
    tup1.write  ()


# get some math-stuff
Math = GaudiPython.gbl.ROOT.Math

## get the second N-tuple
tup2 = TupleUtils.nTuple( "another/path"                  , ## the path
                          "MyTupleLV"                     , ## literal ID 
                          "N-tuple: Lorentz Vectors "     , ## the title
                          LUN = 'MYLUN1'                  ) ## logical unit  

## fill it:
for i in xrange(0,1000) :

    ## Lorentz Vector
    lv1 = Math.PxPyPzEVector   ( gauss() , gauss() , gauss() , gauss() )
    tup2.column ( "lv1" , lv1 )                              ## Lorentz Vector
   
    ## Lorentz Vector
    lv2 = Math.PtEtaPhiEVector ( gauss() , gauss() , gauss() , gauss() )
    tup2.column ( "lv2" , lv2 )                              ## Lorentz Vector

    tup2.write  () 


## get the third N-tuple
tup3 = TupleUtils.nTuple( "another/path"                  , ## the path
                          "MyTuple3DV"                    , ## literal ID 
                          "N-tuple: 3D-Vectors "          , ## the title
                          LUN = 'MYLUN2'                  ) ## logical unit  

## fill it:
for i in xrange(0,1000) :

    ## 3D-Vector
    v1 = Math.XYZVector( gauss() , gauss() , gauss() )
    tup3.column ( "v1" , v1 )                                ## 3D-Vector

    ## 3D-Vector
    v2 = Math.Polar3DVector( gauss() , gauss() , gauss() )
    tup3.column ( "v2" , v2 )                                ## 3D-Vector
    
    ## 3D-Vector
    v3 = Math.RhoEtaPhiVector( gauss() , gauss() , gauss() )
    tup3.column ( "v3" , v3 )                                ## 3D-Vector
    
    ## 3D-Vector
    v4 = Math.RhoZPhiVector( gauss() , gauss() , gauss() )
    tup3.column ( "v4" , v4 )                                ## 3D-Vector

    tup3.write  () 


## get the fourth N-tuple
tup4 = TupleUtils.nTuple( "another/path"                  , ## the path
                          "MyTuple3DP"                    , ## literal ID 
                          "N-tuple: 3D-Points"            , ## the title
                          LUN = 'MYLUN3'                  ) ## logical unit  

## fill it:
for i in xrange(0,1000) :
    
    ## 3D-Point
    p1 = Math.XYZPoint( gauss() , gauss() , gauss() )
    tup4.column ( "p1" , p1 )                                ## 3D-Point
    
    ## 3D-Point
    p2 = Math.Polar3DPoint( gauss() , gauss() , gauss() )
    tup4.column ( "p2" , p2 )                                ## 3D-Point

    ## 3D-Point
    p3 = Math.RhoEtaPhiPoint( gauss() , gauss() , gauss() )
    tup4.column ( "p3" , p3 )                               ## 3D-Point

    ## 3D-Point
    p4 = Math.RhoZPhiPoint( gauss() , gauss() , gauss() )
    tup4.column ( "p4" , p4 )                               ## 3D-Point
    
    tup4.write  ()


## get the 5th N-tuple
tup5 = TupleUtils.nTuple( "another/path"                  , ## the path
                          415                             , ## integer ID 
                          "N-tuple: VarArrays"            , ## the title
                          LUN = 'MYLUN1'                  ) ## logical unit

vct1 = GaudiPython.gbl.vector('double')
GaudiPython.loaddict('CLHEPRflx')
vct2 = GaudiPython.gbl.CLHEP.HepVector

## fill it!
for i in xrange ( 0 , 100 ) :

    ## variable size vector:
    v1=vct1()
    n=max(10,long(50+25*gauss()))
    n=min(n,100)
    while n > v1.size() : v1.push_back ( gauss() )   
    
    tup5.farray ( 'vct1' , v1 , 'len1' , 100 )  ## std::vector<double> 

    n2=min(50,max(5,long(50+25*gauss())))
    v2 = vct2(n2)
    
    tup5.farray( 'vct2' , v2 , 'len2' , 70 )   ## CLHEP::HepVector<double> 

    tup5.write ()



## get the 6th N-tuple
tup6 = TupleUtils.nTuple( "another/path"                  , ## the path
                          "xTuple"                        , ## literal ID 
                          "N-tuple: FixArrays"            , ## the title
                          LUN = 'MYLUN2'                  ) ## logical unit

Gaudi = GaudiPython.gbl.Gaudi

for i in xrange(0,10) :

    v1=vct1()
    for j in range(0,5) : v1.push_back( gauss() )
    tup6.array ( "v1" , v1 )                           ## std::vector<double>

    v2=vct2( 10 )
    tup6.array ( "v2" , v2 )                           ## CLHEP::HepVector<double>

    v3= Gaudi.Vector2()
    tup6.array ( "v3" , v3 )                           ## Gaudi::Vector2 

    v4= Gaudi.Vector3()
    tup6.array ( "v4" , v4 )                           ## Gaudi::Vector3 

    
    v5= Gaudi.Vector4()
    tup6.array ( "v5" , v5 )                           ## Gaudi::Vector4 

    v6= Gaudi.Vector5()
    tup6.array ( "v6" , v6 )                           ## Gaudi::Vector5 

    v7= Gaudi.Vector6()
    tup6.array ( "v7" , v7 )                           ## Gaudi::Vector6 

    v8= Gaudi.Vector7()
    tup6.array ( "v8" , v8 )                           ## Gaudi::Vector7 

    tup6.write ()

## get the 7th N-tuple
tup7 = TupleUtils.nTuple( "another/path"                  , ## the path
                          "FixMatrices"                   , ## literal ID 
                          "N-tuple: FixMatrices"          , ## the title
                          LUN = 'MYLUN3'                  ) ## logical unit

for i in xrange(0,100) :

    m2 = Gaudi.Matrix2x2()
    tup7.matrix ( "m2" , m2 )                            ## Gaudi::Matrix2x2

    m3 = Gaudi.Matrix3x3()
    tup7.matrix ( "m3" , m3 )                            ## Gaudi::Matrix3x3
    
    m4 = Gaudi.Matrix4x4()
    tup7.matrix ( "m4" , m4 )                            ## Gaudi::Matrix4x4
    
    m5 = Gaudi.Matrix5x5()
    tup7.matrix ( "m5" , m5 )                            ## Gaudi::Matrix5x5

    m6 = Gaudi.Matrix6x6()
    tup7.matrix ( "m6" , m6 )                            ## Gaudi::Matrix6x6

    m7 = Gaudi.Matrix7x7()
    tup7.matrix ( "m7" , m7 )                            ## Gaudi::Matrix7x7
    
    tup7.write () 

## get the 8th N-tuple
tup8 = TupleUtils.nTuple( "another/path"                  , ## the path
                          "FixSymMatrices"                , ## literal ID 
                          "N-tuple: FixSymMatrices"       , ## the title
                          LUN = 'MYLUN2'                  ) ## logical unit

for i in xrange(0,100) :

    m2 = Gaudi.SymMatrix2x2()
    tup8.matrix ( "m2" , m2 )                            ## Gaudi::SymMatrix2x2

    m3 = Gaudi.SymMatrix3x3()
    tup8.matrix ( "m3" , m3 )                            ## Gaudi::SymMatrix3x3
    
    m4 = Gaudi.SymMatrix4x4()
    tup8.matrix ( "m4" , m4 )                            ## Gaudi::SymMatrix4x4
    
    m5 = Gaudi.SymMatrix5x5()
    tup8.matrix ( "m5" , m5 )                            ## Gaudi::SymMatrix5x5

    m6 = Gaudi.SymMatrix6x6()
    tup8.matrix ( "m6" , m6 )                            ## Gaudi::SymMatrix6x6

    m7 = Gaudi.SymMatrix7x7()
    tup8.matrix ( "m7" , m7 )                            ## Gaudi::SymMatrix7x7
    
    tup8.write () 



## release all tuples at the end 
TupleUtils.releaseTuples ()


# =============================================================================
# The END
# =============================================================================
