// $Id: HistoDecorator.cpp,v 1.5 2008/10/09 09:59:14 marcocle Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiHistoAlg.h"
// ============================================================================
// GaudiPython
// ============================================================================
#include "GaudiPython/Vector.h"
#include "GaudiPython/HistoDecorator.h"
// ============================================================================
/** @file
 * Implementation file for class GaudiPython::HistoDecorator
 *  @date 2005-08-04 
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 */
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data  data 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo  , 
  const double                 data  , 
  const std::string&           title ,
  const double                 low   ,
  const double                 high  ,
  const unsigned long          bins  ) 
{ return algo.plot1D( data , title , low , high , bins ) ; }
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data data  
 *  @param ID   histogram ID 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo  , 
  const double                 data  , 
  const GaudiAlg::HistoID&     ID    ,
  const std::string&           title ,
  const double                 low   ,
  const double                 high  ,
  const unsigned long          bins  ) 
{ return algo.plot1D( data , ID , title , low , high , bins ) ; }
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data data  
 *  @param ID   histogram ID 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo  , 
  const double                 data  , 
  const long                   ID    ,
  const std::string&           title ,
  const double                 low   ,
  const double                 high  ,
  const unsigned long          bins  ) 
{ return algo.plot1D( data , ID , title , low , high , bins ) ; }
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data data  
 *  @param ID   histogram ID 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo  , 
  const double                 data  , 
  const std::string&           ID    ,
  const std::string&           title ,
  const double                 low   ,
  const double                 high  ,
  const unsigned long          bins  ) 
{ return algo.plot1D( data , ID , title , low , high , bins ) ; }
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data vector of data 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo  , 
  const GaudiPython::Vector&   data  , 
  const std::string&           title ,
  const double                 low   ,
  const double                 high  ,
  const unsigned long          bins  ) 
{ 
  return algo.plot 
    ( GaudiPython::_identity<GaudiPython::Vector::value_type>() , 
      data.begin ()              ,
      data.end   ()              , 
      title , low , high , bins  ) ;
} 
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data vector of data 
 *  @param ID   histogram ID 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo   , 
  const GaudiPython::Vector&   data   , 
  const GaudiAlg::HistoID&     ID     ,
  const std::string&           title  ,
  const double                 low    ,
  const double                 high   ,
  const unsigned long          bins   ) 
{
  return algo.plot 
    ( GaudiPython::_identity<GaudiPython::Vector::value_type>() , 
      data.begin ()                       ,
      data.end   ()                       , 
      ID , title , low , high , bins      ) ;
}
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data vector of data 
 *  @param ID   histogram ID 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo   , 
  const GaudiPython::Vector&   data   , 
  const long                   ID     ,
  const std::string&           title  ,
  const double                 low    ,
  const double                 high   ,
  const unsigned long          bins   ) 
{
  return algo.plot 
    ( GaudiPython::_identity<GaudiPython::Vector::value_type>() , 
      data.begin ()                       ,
      data.end   ()                       , 
      ID , title , low , high , bins      ) ;
}
// ============================================================================
/*  "plot"(book&fill) a sequence of data from the vector (implicit loop) 
 *  (Expected to be more efficient)
 *  @param algo the algorihtm
 *  @param data vector of data 
 *  @param ID   histogram ID 
 *  @param title histogram title 
 *  @param low   low edge 
 *  @param high  high edge
 *  @param bins  number of bins 
 */
// ============================================================================
AIDA::IHistogram1D*   GaudiPython::HistoDecorator::plot1D
( const GaudiHistoAlg&         algo   , 
  const GaudiPython::Vector&   data   , 
  const std::string&           ID     ,
  const std::string&           title  ,
  const double                 low    ,
  const double                 high   ,
  const unsigned long          bins   ) 
{
  return algo.plot 
    ( GaudiPython::_identity<GaudiPython::Vector::value_type>() , 
      data.begin ()                       ,
      data.end   ()                       , 
      ID , title , low , high , bins      ) ;
}
// ============================================================================
/*  fill the 2D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param binsX  number of bins in x
 *  @param binsY  number of bins in y
 *  @param weight weight
 *  @return pointer to AIDA 2D histogram
 */
// ============================================================================
AIDA::IHistogram2D*  GaudiPython::HistoDecorator::plot2D
( const GaudiHistoAlg&      algo    ,
  const double              valueX  ,
  const double              valueY  ,
  const std::string&        title   ,
  const double              lowX    ,
  const double              highX   ,
  const double              lowY    ,
  const double              highY   ,
  const unsigned long       binsX   ,
  const unsigned long       binsY   ,
  const double              weight  ) 
{
  return algo.plot2D ( valueX , valueY , 
                       title  , 
                       lowX   , highX  , 
                       lowY   , highY  ,
                       binsX  , binsY  , weight ) ;
}
// ============================================================================
/** fill the 2D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param ID     Histogram ID to use
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param binsX  number of bins in x
 *  @param binsY  number of bins in y
 *  @param weight weight
 *  @return pointer to AIDA 2D histogram
 */
// ============================================================================
AIDA::IHistogram2D*  GaudiPython::HistoDecorator::plot2D
( const GaudiHistoAlg&      algo    ,
  const double              valueX  ,
  const double              valueY  ,
  const GaudiAlg::HistoID&  ID      ,
  const std::string&        title   ,
  const double              lowX    ,
  const double              highX   ,
  const double              lowY    ,
  const double              highY   ,
  const unsigned long       binsX   ,
  const unsigned long       binsY   ,
  const double              weight  ) 
{
  return algo.plot2D ( valueX , valueY , 
                       ID     , title  , 
                       lowX   , highX  , 
                       lowY   , highY  ,
                       binsX  , binsY  , weight ) ;
}
// ============================================================================
/*  fill the 2D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param ID     Histogram ID to use
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param binsX  number of bins in x
 *  @param binsY  number of bins in y
 *  @param weight weight
 *  @return pointer to AIDA 2D histogram
 */
// ============================================================================
AIDA::IHistogram2D*  GaudiPython::HistoDecorator::plot2D
( const GaudiHistoAlg&      algo    ,
  const double              valueX  ,
  const double              valueY  ,
  const long                ID      ,
  const std::string&        title   ,
  const double              lowX    ,
  const double              highX   ,
  const double              lowY    ,
  const double              highY   ,
  const unsigned long       binsX   ,
  const unsigned long       binsY   ,
  const double              weight  ) 
{
  return algo.plot2D ( valueX , valueY , 
                       ID     , title  , 
                       lowX   , highX  , 
                       lowY   , highY  ,
                       binsX  , binsY  , weight ) ;
}
// ============================================================================
/*  fill the 2D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param ID     Histogram ID to use
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param binsX  number of bins in x
 *  @param binsY  number of bins in y
 *  @param weight weight
 *  @return pointer to AIDA 2D histogram
 */
// ============================================================================
AIDA::IHistogram2D*  GaudiPython::HistoDecorator::plot2D
( const GaudiHistoAlg&      algo    ,
  const double              valueX  ,
  const double              valueY  ,
  const std::string&        ID      ,
  const std::string&        title   ,
  const double              lowX    ,
  const double              highX   ,
  const double              lowY    ,
  const double              highY   ,
  const unsigned long       binsX   ,
  const unsigned long       binsY   ,
  const double              weight  ) 
{
  return algo.plot2D ( valueX , valueY , 
                       ID     , title  , 
                       lowX   , highX  , 
                       lowY   , highY  ,
                       binsX  , binsY  , weight ) ;
}
// ============================================================================
/*  fill the 3D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param valueZ z value to be filled
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param lowZ  low z limit for histogram
 *  @param highZ high z limit for histogram
 *  @param binsX number of bins in x
 *  @param binsY number of bins in y
 *  @param binsZ number of bins in z
 *  @param weight weight
 *  @return pointer to AIDA 3D histogram
 */
// ============================================================================
AIDA::IHistogram3D*  GaudiPython::HistoDecorator::plot3D
( const GaudiHistoAlg&      algo   ,  
  const double        valueX       ,
  const double        valueY       ,
  const double        valueZ       ,
  const std::string&  title        ,
  const double        lowX         ,
  const double        highX        ,
  const double        lowY         ,
  const double        highY        ,
  const double        lowZ         ,
  const double        highZ        ,
  const unsigned long binsX        ,
  const unsigned long binsY        ,
  const unsigned long binsZ        ,
  const double        weight       ) 
{
  return algo. plot3D ( valueX , valueY , valueZ , 
                        title                    , 
                        lowX   , highX  , 
                        lowY   , highY  , 
                        lowZ   , highZ  , 
                        binsX  , binsY  , binsZ  , 
                        weight                   ) ;
}
// ============================================================================
/*  fill the 3D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param valueZ z value to be filled
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param lowZ  low z limit for histogram
 *  @param highZ high z limit for histogram
 *  @param binsX number of bins in x
 *  @param binsY number of bins in y
 *  @param binsZ number of bins in z
 *  @param weight weight
 *  @return pointer to AIDA 3D histogram
 */
// ============================================================================
AIDA::IHistogram3D*  GaudiPython::HistoDecorator::plot3D
( const GaudiHistoAlg&      algo   ,  
  const double        valueX       ,
  const double        valueY       ,
  const double        valueZ       ,
  const GaudiAlg::HistoID&  ID     ,
  const std::string&  title        ,
  const double        lowX         ,
  const double        highX        ,
  const double        lowY         ,
  const double        highY        ,
  const double        lowZ         ,
  const double        highZ        ,
  const unsigned long binsX        ,
  const unsigned long binsY        ,
  const unsigned long binsZ        ,
  const double        weight       ) 
{
  return algo. plot3D ( valueX , valueY , valueZ , 
                        ID     , title  , 
                        lowX   , highX  , 
                        lowY   , highY  , 
                        lowZ   , highZ  , 
                        binsX  , binsY  , binsZ  , 
                        weight                   ) ;
}
// ============================================================================
/*  fill the 3D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param valueZ z value to be filled
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param lowZ  low z limit for histogram
 *  @param highZ high z limit for histogram
 *  @param binsX number of bins in x
 *  @param binsY number of bins in y
 *  @param binsZ number of bins in z
 *  @param weight weight
 *  @return pointer to AIDA 3D histogram
 */
// ============================================================================
AIDA::IHistogram3D*  GaudiPython::HistoDecorator::plot3D
( const GaudiHistoAlg&      algo   ,  
  const double        valueX       ,
  const double        valueY       ,
  const double        valueZ       ,
  const long          ID           ,
  const std::string&  title        ,
  const double        lowX         ,
  const double        highX        ,
  const double        lowY         ,
  const double        highY        ,
  const double        lowZ         ,
  const double        highZ        ,
  const unsigned long binsX        ,
  const unsigned long binsY        ,
  const unsigned long binsZ        ,
  const double        weight       ) 
{
  return algo. plot3D ( valueX , valueY , valueZ , 
                        ID     , title  , 
                        lowX   , highX  , 
                        lowY   , highY  , 
                        lowZ   , highZ  , 
                        binsX  , binsY  , binsZ  , 
                        weight                   ) ;
}
// ============================================================================
/*  fill the 3D histogram (book on demand)
 *  @param valueX x value to be filled
 *  @param valueY y value to be filled
 *  @param valueZ z value to be filled
 *  @param title histogram title (must be unique within the algorithm)
 *  @param lowX  low x limit for histogram
 *  @param highX high x limit for histogram
 *  @param lowY  low y limit for histogram
 *  @param highY high y limit for histogram
 *  @param lowZ  low z limit for histogram
 *  @param highZ high z limit for histogram
 *  @param binsX number of bins in x
 *  @param binsY number of bins in y
 *  @param binsZ number of bins in z
 *  @param weight weight
 *  @return pointer to AIDA 3D histogram
 */
// ============================================================================
AIDA::IHistogram3D*  
GaudiPython::HistoDecorator::plot3D
( const GaudiHistoAlg&      algo   ,  
  const double        valueX       ,
  const double        valueY       ,
  const double        valueZ       ,
  const std::string&  ID           ,
  const std::string&  title        ,
  const double        lowX         ,
  const double        highX        ,
  const double        lowY         ,
  const double        highY        ,
  const double        lowZ         ,
  const double        highZ        ,
  const unsigned long binsX        ,
  const unsigned long binsY        ,
  const unsigned long binsZ        ,
  const double        weight       ) 
{
  return algo. plot3D ( valueX , valueY , valueZ , 
                        ID     , title  , 
                        lowX   , highX  , 
                        lowY   , highY  , 
                        lowZ   , highZ  , 
                        binsX  , binsY  , binsZ  , 
                        weight                   ) ;
}
// ========================================================================
// 1D-profiles: by title  
// ========================================================================  
AIDA::IProfile1D* 
GaudiPython::HistoDecorator::profile1D
( const GaudiHistoAlg&   algo   , 
  const double           valueX ,
  const double           valueY ,
  const std::string&     title  ,
  const double           lowX   ,
  const double           highX  ,
  const unsigned long    binsX  ,
  const std::string&     opt    ,
  const double           lowY   , 
  const double           highY  ,
  const double           weight ) 
{
  return algo.profile1D 
    ( valueX , valueY , title , lowX , highX , binsX , 
      opt , lowY , highY , weight ) ;
}
// ========================================================================      
// 1D-profiles: by generic ID 
// ========================================================================  
AIDA::IProfile1D* 
GaudiPython::HistoDecorator::profile1D
( const GaudiHistoAlg&     algo   , 
  const double             valueX ,
  const double             valueY ,
  const GaudiAlg::HistoID& ID     ,
  const std::string&       title  ,
  const double             lowX   ,
  const double             highX  ,
  const unsigned long      binsX  ,
  const std::string&       opt    ,
  const double             lowY   , 
  const double             highY  ,
  const double             weight )
{
  return algo.profile1D 
    ( valueX , valueY , ID , title , lowX , highX , binsX , 
      opt , lowY , highY , weight ) ;
}
// ========================================================================      
// 1D-profiles: by numeric ID 
// ========================================================================  
AIDA::IProfile1D* 
GaudiPython::HistoDecorator::profile1D
( const GaudiHistoAlg&   algo   , 
  const double           valueX ,
  const double           valueY ,
  const long             ID     ,
  const std::string&     title  ,
  const double           lowX   ,
  const double           highX  ,
  const unsigned long    binsX  ,
  const std::string&     opt    ,
  const double           lowY   , 
  const double           highY  ,
  const double           weight )
{
  return algo.profile1D 
    ( valueX , valueY , ID , title , lowX , highX , binsX , 
      opt , lowY , highY , weight ) ;
}
// ========================================================================      
// 1D-profiles: by string ID 
// ========================================================================  
AIDA::IProfile1D* 
GaudiPython::HistoDecorator::profile1D
( const GaudiHistoAlg&   algo   , 
  const double           valueX ,
  const double           valueY ,
  const std::string&     ID     ,
  const std::string&     title  ,
  const double           lowX   ,
  const double           highX  ,
  const unsigned long    binsX  ,
  const std::string&     opt    ,
  const double           lowY   , 
  const double           highY  ,
  const double           weight ) 
{
  return algo.profile1D 
    ( valueX , valueY , ID , title , lowX , highX , binsX , 
      opt , lowY , highY , weight ) ;
}
// ========================================================================  
// 2D-profiles: by title 
// ========================================================================
AIDA::IProfile2D* 
GaudiPython::HistoDecorator::profile2D
( const GaudiHistoAlg&   algo   , 
  const double           valueX ,
  const double           valueY ,
  const double           valueZ ,
  const std::string&     title  ,
  const double           lowX   ,
  const double           highX  ,
  const double           lowY   ,
  const double           highY  ,
  const unsigned long    binsX  ,
  const unsigned long    binsY  ,
  const double           weight ) 
{
  return algo.profile2D 
    ( valueX , valueY , valueZ ,
      title  , 
      lowX   , highX  , 
      lowY   , highY  , 
      binsX  , binsY  , weight  ) ;
}
// ========================================================================  
// 2D-profiles: by generic ID  
// ========================================================================  
AIDA::IProfile2D* 
GaudiPython::HistoDecorator::profile2D
( const GaudiHistoAlg&     algo   , 
  const double             valueX ,
  const double             valueY ,
  const double             valueZ ,
  const GaudiAlg::HistoID& ID     ,
  const std::string&       title  ,
  const double             lowX   ,
  const double             highX  ,
  const double             lowY   ,
  const double             highY  ,
  const unsigned long      binsX  ,
  const unsigned long      binsY  ,
  const double             weight ) 
{
  return algo.profile2D 
    ( valueX , valueY , valueZ ,
      ID     , title  , 
      lowX   , highX  , 
      lowY   , highY  , 
      binsX  , binsY  , weight  ) ;
}
// ========================================================================  
// 2D-profiles: by numeric ID  
// ========================================================================  
AIDA::IProfile2D* 
GaudiPython::HistoDecorator::profile2D
( const GaudiHistoAlg&   algo   , 
  const double           valueX ,
  const double           valueY ,
  const double           valueZ ,
  const long             ID     ,
  const std::string&     title  ,
  const double           lowX   ,
  const double           highX  ,
  const double           lowY   ,
  const double           highY  ,
  const unsigned long    binsX  ,
  const unsigned long    binsY  ,
  const double           weight ) 
{
  return algo.profile2D 
    ( valueX , valueY , valueZ ,
      ID     , title  , 
      lowX   , highX  , 
      lowY   , highY  , 
      binsX  , binsY  , weight  ) ;
}
// ========================================================================  
// 2D-profiles: by string ID  
// ========================================================================  
AIDA::IProfile2D* 
GaudiPython::HistoDecorator::profile2D
( const GaudiHistoAlg&   algo   , 
  const double           valueX ,
  const double           valueY ,
  const double           valueZ ,
  const std::string&     ID     ,
  const std::string&     title  ,
  const double           lowX   ,
  const double           highX  ,
  const double           lowY   ,
  const double           highY  ,
  const unsigned long    binsX  ,
  const unsigned long    binsY  ,
  const double           weight ) 
{
  return algo.profile2D 
    ( valueX , valueY , valueZ ,
      ID     , title  , 
      lowX   , highX  , 
      lowY   , highY  , 
      binsX  , binsY  , weight  ) ;
}
// ============================================================================
// The END 
// ============================================================================
