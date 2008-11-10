// $Id: HistoStats.cpp,v 1.5 2008/06/04 08:18:22 marcocle Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <limits>
#include <climits>
// ============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"
// ============================================================================
// GaudiUtils 
// ============================================================================
#include "GaudiUtils/HistoStats.h"
// ============================================================================
/** @file
 *  Implementation file for class Gaudi::Utils::HistoStats
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-08-06 
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// define local "bad" value 
  const double s_bad = -1 * std::numeric_limits<double>::max() ;
  // ==========================================================================
}
// ============================================================================
/*  get the moment of the certain around the specified  "value"
 *  @param histo histogram
 *  @param order the momentm order
 *  @param value central value 
 *  @return the evaluated moment
 */
// ============================================================================
double Gaudi::Utils::HistoStats::moment        
( const AIDA::IHistogram1D* histo , 
  const unsigned int        order ,  
  const double              value ) 
{
  if ( 0 == histo ) { return s_bad ; }                       // RETURN
  if ( 0 == order ) { return 1.0   ; }                       // RETURN
  if ( 1 == order ) { return mean( histo ) - value ; }       // RETURN
  if ( 2 == order ) 
  {
    const double _r =         rms  ( histo ) ;
    const double _d = value - mean ( histo ) ;
    return _r *_r + _d * _d ;                            // RETURN
  }
  const double n = nEff ( histo )  ;
  if ( 0 >= n     ) { return 0.0   ; }                    // RETURN 
  
  const int _order = (int) order ;
  
  // get the exis 
  const AIDA::IAxis& axis = histo -> axis () ;
  // number of bins 
  const int nBins = axis.bins() ;
  double result = 0 ;
  double weight = 0 ;
  // loop over all bins 
  for ( int i = 0 ; i < nBins ; ++i ) 
  {
    const double lE   = axis . binLowerEdge ( i ) ;
    const double uE   = axis . binUpperEdge ( i ) ;
    //
    const double yBin = histo -> binHeight    ( i ) ;   // bin weight 
    const double xBin = 0.5 * ( lE + uE ) ;             // bin center 
    //
    weight += yBin ;
    result += yBin * std::pow ( xBin - value , _order ) ;
  }    
  if ( 0 != weight ) { result /= weight ; }
  return result ;
}
// ======================================================================
/** evaluate the uncertanty for 'bin-by-bin'-moment
 *  @param histo histogram
 *  @param order the moment parameter 
 *  @param value central value 
 *  @return the evaluated uncertanty in the moment
 */
// ======================================================================
double Gaudi::Utils::HistoStats::momentErr
( const AIDA::IHistogram1D* histo , 
  const unsigned int        order ) 
{
  if ( 0 == histo ) { return s_bad ; }                   // RETURN 
  const double n = nEff ( histo ) ;
  if ( 0 >= n     ) { return 0.0   ; }                   // RETURN
  const double a2o = moment ( histo , 2 * order ) ;   // a(2o)
  const double ao  = moment ( histo , 2 * order ) ;   // a(o) 
  double result = a2o - ao*ao ;
  result        /= n ;
  result = std::max ( 0.0 , result ) ;
  return std:: sqrt ( result ) ;                            // RETURN  
}
// ======================================================================
/*  evaluate the central momentum (around the mean value) 
 *  @param histo histogram
 *  @param order the momentm order
 *  @param value central value 
 *  @return the evaluated central moment
 */
// ======================================================================
double Gaudi::Utils::HistoStats::centralMoment 
( const AIDA::IHistogram1D* histo , 
  const unsigned int        order ) 
{
  if ( 0 == histo ) { return s_bad ; }                        // RETURN
  if ( 0 == order ) { return 1.0   ; }                        // RETURN
  if ( 1 == order ) { return 0.0   ; }                        // RETURN
  if ( 2 == order ) 
  {
    const double sigma = histo->rms() ;
    return sigma * sigma ;                                    // RETURN
  }
  // delegate the actual evaluation to another method:
  return moment ( histo , order , mean ( histo ) ) ;
}
// ======================================================================
/*  evaluate the uncertanty for 'bin-by-bin'-central momentum 
 *  (around the mean value)  
 *  ( the uncertanty is calculated with O(1/n2) precision)
 *  @param histo histogram
 *  @param order the moment parameter 
 *  @param value central value 
 *  @return the evaluated uncertanty in the central moment
 */
// ======================================================================
double Gaudi::Utils::HistoStats::centralMomentErr
( const AIDA::IHistogram1D* histo , 
  const unsigned int        order ) 
{
  if ( 0 == histo ) { return s_bad ; }                    // RETURN
  const double n    = nEff ( histo ) ;
  if ( 0 >= n     ) { return 0.0   ; }                    // RETURN
  const double mu2  = centralMoment ( histo , 2             ) ; // mu(2)
  const double muo  = centralMoment ( histo ,     order     ) ; // mu(o)
  const double mu2o = centralMoment ( histo , 2 * order     ) ; // mu(2o)
  const double muom = centralMoment ( histo ,     order - 1 ) ; // mu(o-1)
  const double muop = centralMoment ( histo ,     order + 1 ) ; // mu(o+1)
  double result  =  mu2o  ;
  result        -=  2.0   * order * muom * muop ;
  result        -=  muo   * muo   ;
  result        +=  order * order * mu2  * muom * muom ;
  result        /=  n     ;
  result = std::max ( 0.0 , result ) ;
  return std:: sqrt ( result ) ;                            // RETURN
}
// ======================================================================
// get the skewness for the histogram 
// ======================================================================
double Gaudi::Utils::HistoStats::skewness        
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }                      // RETURN
  const double mu3 = centralMoment ( histo , 3 ) ;
  const double s3  = std::pow ( rms ( histo ) , 3 ) ;
  return ( std::fabs(s3)>0 ? mu3/s3 : 0.0 );
}
// ======================================================================
// get the error in skewness 
// ======================================================================
double Gaudi::Utils::HistoStats::skewnessErr 
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }                     // RETURN 
  const double n = nEff ( histo ) ;
  if ( 2 > n      ) { return 0.0   ; }                     // RETURN
  double result = 6 ;
  result *= ( n - 2 )  ;
  result /= ( n + 1 ) * ( n + 3 ) ;    
  return std::sqrt ( result ) ;  
}
// ======================================================================
// get the kurtosis for the histogram 
// ======================================================================
double Gaudi::Utils::HistoStats::kurtosis          
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }                    // RETURN 
  const double mu4 = centralMoment ( histo , 4 ) ;
  const double s4  = std::pow ( rms ( histo ) , 4 ) ;
  return ( std::fabs(s4)>0 ? mu4/s4 - 3.0 : 0.0 );
}
// ======================================================================
// get the error in kurtosis
// ======================================================================
double Gaudi::Utils::HistoStats::kurtosisErr 
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }                    // RETURN 
  const double n = nEff ( histo ) ;
  if ( 3 > n      ) { return 0.0 ; }                      // RETURN   
  double result = 24 ;
  result *= ( n - 2 ) * ( n - 3 ) ;
  result /= ( n + 1 ) * ( n + 1 ) ;
  result /= ( n + 3 ) * ( n + 5 ) ;
  return std::sqrt ( result ) ;  
}
// ======================================================================
// get the effective entries 
// ======================================================================
double Gaudi::Utils::HistoStats::nEff 
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }
  return histo -> equivalentBinEntries () ;
}
// ======================================================================
// get the mean value for the histogram 
// ======================================================================
double Gaudi::Utils::HistoStats::mean              
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }
  return histo -> mean() ;    
}
// ======================================================================
// get an error in the mean value 
// ======================================================================
double Gaudi::Utils::HistoStats::meanErr 
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }
  const double n = nEff ( histo ) ;
  return 0 >= n ? 0.0 : rms ( histo ) / std::sqrt ( n ) ;
}
// ======================================================================
// get the rms value for the histogram 
// ======================================================================
double Gaudi::Utils::HistoStats::rms
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }
  return histo -> rms () ;    
}
// ======================================================================
// get the error in rms 
// ======================================================================
double Gaudi::Utils::HistoStats::rmsErr  
( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return s_bad ; }  
  const double n = nEff ( histo ) ;    
  if ( 1 >=  n ) { return 0.0 ; }
  double result = 2 + kurtosis ( histo ) ;
  result += 2.0 /( n - 1 ) ;
  result /= 4.0 * n ;
  result = std::max ( result , 0.0 ) ;
  return histo -> rms() * std::sqrt ( result ) ;
}
// ============================================================================
// The END 
// ============================================================================
