// $Id: ChronoEntity.cpp,v 1.2 2006/12/10 20:29:18 leggett Exp $ 
// ============================================================================
#define GAUDIKERNEL_CHRONOENTITY_CPP 1 
// ============================================================================
// incldue files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream> 
#include <iomanip> 
#include <cmath> 
#include <algorithm>
#include <cstdio>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h" 
#include "GaudiKernel/System.h" 
#include "GaudiKernel/ChronoEntity.h" 

/** @file 
 *  implementation fiel for class ChronoEntity 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date:   December 1, 1999 
 */

// ============================================================================
// Constructor 
// ============================================================================
ChronoEntity::ChronoEntity() 
  /// current status of this chrono object; 
  : m_ce_currentStatus ( IChronoStatSvc::UNKNOWN )  
  /// number of chrono measurements
  , m_ce_noOfMeasurements                       ( 0          ) 
  /// minimal measurement for user time 
  , m_ce_minimalUserTime                        ( 1000000000 ) 
  /// minimal measurement for kernel time 
  , m_ce_minimalKernelTime                      ( 1000000000 )   
  /// minimal measurement for ellapsed time 
  , m_ce_minimalElapsedTime                     ( 1000000000 )
  /// maximal measurement for user time
  , m_ce_maximalUserTime                        ( 0          )
  /// maximal measurement for kernel time
  , m_ce_maximalKernelTime                      ( 0          )
  /// maximal measurement for ellpsed time
  , m_ce_maximalElapsedTime                     ( 0          )
  /// total user time
  , m_ce_deltaUser                              ( 0          )
  /// total Kernel time
  , m_ce_deltaKernel                            ( 0          )
  /// total Elapsed time
  , m_ce_deltaElapsed                           ( 0          )
  /// total user time
  , m_ce_totalUserTime                          ( 0          )
  /// total Kernel time
  , m_ce_totalKernelTime                        ( 0          )
  /// total Elapsed time
  , m_ce_totalElapsedTime                       ( 0          )
  /// sum of squared user   time intervals
  , m_ce_statisticsForUserTime                  ( 0          )
  /// sum of squared Kernel time intervals
  , m_ce_statisticsForKernelTime                ( 0          )
  /// sum of squared Elapsed time intervals
  , m_ce_statisticsForElapsedTime               ( 0          )
  /// start stamp for current measurement of user time
  , m_ce_startOfCurrentMeasurementOfUserTime    ( 0          )
  /// start stamp for current measurement of Kernel time
  , m_ce_startOfCurrentMeasurementOfKernelTime  ( 0          )
  /// start stamp for current measurement of Elapsed time
  , m_ce_startOfCurrentMeasurementOfElapsedTime ( 0          )
{}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
ChronoEntity::~ChronoEntity(){}
// ============================================================================

// ============================================================================
/// average Kernel Time
// ============================================================================
long double ChronoEntity::kMeanTime      () const
{
  return
    ( 0 == nOfMeasurements() )  ? 0 :
    kTotalTime() / nOfMeasurements() ;
}
// ============================================================================

// ============================================================================
/// average User   Time
// ============================================================================
long double ChronoEntity::uMeanTime      () const
{
  return
    ( 0 == nOfMeasurements() )  ? 0 :
    uTotalTime() / nOfMeasurements() ;
}
// ============================================================================

// ============================================================================
/// average Elapsed   Time
// ============================================================================
long double ChronoEntity::eMeanTime      () const
{
  return
    ( 0 == nOfMeasurements() )  ? 0 :
    eTotalTime() / nOfMeasurements() ;
}
// ============================================================================


// ============================================================================
/// r.m.s Kernel Time
// ============================================================================
long double ChronoEntity::kRMSTime       () const
{
  long double D =
    ( 0 == nOfMeasurements() ) ? 0 :
    ( kStatistics() / ( long double ) nOfMeasurements() -
      kMeanTime() * kMeanTime() ) ;
  return ( 0 > D ) ? 0 : sqrt( D ) ;
}
// ============================================================================

// ============================================================================
/// r.m.s User Time
// ============================================================================
long double ChronoEntity::uRMSTime       () const
{
  long double D =
    ( 0 == nOfMeasurements() ) ? 0 :
    ( uStatistics() /
      ( long double ) nOfMeasurements() - uMeanTime() * uMeanTime() ) ;
  return ( 0 > D ) ? 0 : sqrt( D ) ;
}

// ============================================================================
/// r.m.s Elapsed Time
// ============================================================================
long double ChronoEntity::eRMSTime       () const
{
  long double D =
    ( 0 == nOfMeasurements() ) ? 0 :
    ( eStatistics() /
      ( long double ) nOfMeasurements() - eMeanTime() * eMeanTime() ) ;
  return ( 0 > D ) ? 0 : sqrt( D ) ;
}

// ============================================================================
/// error in mean Kernel time
// ============================================================================
long double ChronoEntity::kMeanErrorTime () const
{
  return
    ( 0 == nOfMeasurements() ) ? 0 :
    ( kRMSTime() / sqrt( ( long double ) nOfMeasurements() ) ) ;
}

/// error in mean User   time
long double ChronoEntity::uMeanErrorTime () const
{
  return
    ( 0 == nOfMeasurements() ) ? 0 :
    ( uRMSTime() / sqrt( ( long double ) nOfMeasurements() ) ) ;
}

/// error in mean Elapsed   time
long double ChronoEntity::eMeanErrorTime () const
{
  return
    ( 0 == nOfMeasurements() ) ? 0 :
    ( eRMSTime() / sqrt( ( long double ) nOfMeasurements() ) ) ;
}

/// start the chrono
IChronoStatSvc::ChronoStatus  ChronoEntity::start()
{
  if ( IChronoStatSvc::RUNNING == status() ) { return status() ; }
  ///
  /// following lines could be platform dependent!
  ///
  /// Store in object the measured times
  m_ce_startOfCurrentMeasurementOfUserTime     =
    System::userTime     ( System::microSec );
  m_ce_startOfCurrentMeasurementOfKernelTime   =
    System::kernelTime   ( System::microSec );
  m_ce_startOfCurrentMeasurementOfElapsedTime =
    System::ellapsedTime ( System::microSec );
  ///
  ///
  m_ce_currentStatus = IChronoStatSvc::RUNNING;
  ///
  return status();
}

/// stop the chrono
IChronoStatSvc::ChronoStatus  ChronoEntity::stop()
{
  if( IChronoStatSvc::RUNNING != status() ) { return status(); }

  /// following lines could be platform dependent!
  m_ce_deltaUser    = System::userTime    ( System::microSec ) -
    m_ce_startOfCurrentMeasurementOfUserTime   ;
  ///
  m_ce_deltaKernel  = System::kernelTime  ( System::microSec ) -
    m_ce_startOfCurrentMeasurementOfKernelTime ;
  ///
  m_ce_deltaElapsed = System::ellapsedTime( System::microSec ) -
    m_ce_startOfCurrentMeasurementOfElapsedTime ;
  ///
  /// update all values
  ///
  /// number of chrono measurements
  ++m_ce_noOfMeasurements;
  /// minimal measurement for user time
  m_ce_minimalUserTime     =
    ( m_ce_deltaUser     < m_ce_minimalUserTime     ) ? m_ce_deltaUser     :
    m_ce_minimalUserTime    ;
  /// minimal measurement for kernel time
  m_ce_minimalKernelTime   =
    ( m_ce_deltaKernel   < m_ce_minimalKernelTime   ) ? m_ce_deltaKernel   :
    m_ce_minimalKernelTime  ;
  /// minimal measurement for kernel time
  m_ce_minimalElapsedTime =
    ( m_ce_deltaElapsed < m_ce_minimalElapsedTime ) ? m_ce_deltaElapsed :
    m_ce_minimalElapsedTime ;
  /// maximal measurement for user time
  m_ce_maximalUserTime     =
    ( m_ce_deltaUser     > m_ce_maximalUserTime     ) ? m_ce_deltaUser     :
    m_ce_maximalUserTime     ;
  /// maximal measurement for kernel time
  m_ce_maximalKernelTime   =
    ( m_ce_deltaKernel   > m_ce_maximalKernelTime   ) ? m_ce_deltaKernel   :
    m_ce_maximalKernelTime   ;
  /// maximal measurement for kernel time
  m_ce_maximalElapsedTime =
    ( m_ce_deltaElapsed > m_ce_maximalElapsedTime ) ? m_ce_deltaElapsed :
    m_ce_maximalElapsedTime ;
  /// total user time
  m_ce_totalUserTime             += m_ce_deltaUser     ;
  /// total Kernel time
  m_ce_totalKernelTime           += m_ce_deltaKernel   ;
  /// total Elapsed time
  m_ce_totalElapsedTime         += m_ce_deltaElapsed ;
  /// sum of squared user   time intervals
  m_ce_statisticsForUserTime     += m_ce_deltaUser     * m_ce_deltaUser     ;
  /// sum of squared Kernel time intervals
  m_ce_statisticsForKernelTime   += m_ce_deltaKernel   * m_ce_deltaKernel   ;
  /// sum of squared Kernel time intervals
  m_ce_statisticsForElapsedTime += m_ce_deltaElapsed * m_ce_deltaElapsed ;
  /// set new status
  m_ce_currentStatus = IChronoStatSvc::STOPPED;
  ///
  return status();
}

/// return the last delta-time of type "type"
IChronoStatSvc::ChronoTime ChronoEntity::delta (IChronoStatSvc::ChronoType type)
{
  IChronoStatSvc::ChronoTime result = 0;
  switch ( type ) {
    case IChronoStatSvc::USER:
      result = m_ce_deltaUser;
      break;
    case IChronoStatSvc::KERNEL:
      result = m_ce_deltaKernel;
      break;
    case IChronoStatSvc::ELAPSED:
      result = m_ce_deltaElapsed;
  }
  return result;
}

/// print user time
std::string ChronoEntity::outputUserTime      () const
{
  std::string res ("Time User   : ") ;
  return res += format( uTotalTime     () ,
			uMinimalTime   () ,
			uMeanTime      () ,
			uRMSTime       () ,
			uMaximalTime   () ,
			nOfMeasurements() );
}

/// print system time
std::string ChronoEntity::outputSystemTime      () const
{
  std::string res ("Time System : ") ;
  return res += format( kTotalTime     () ,
			kMinimalTime   () ,
			kMeanTime      () ,
			kRMSTime       () ,
			kMaximalTime   () ,
			nOfMeasurements() );
}

/// print time
std::string ChronoEntity::outputElapsedTime      () const
{
  std::string res ("TimeElapsed: ") ;
  return res += format( eTotalTime     () , 
			eMinimalTime   () , 
			eMeanTime      () , 
			eRMSTime       () , 
			eMaximalTime   () , 
			nOfMeasurements() );  
}
// ============================================================================
 

// ============================================================================
/// print the chrono  
// ============================================================================
std::string ChronoEntity::format( long double   total     , 
				  long double   minimal   ,
                                  long double   mean      , 
                                  long double   rms       , 
                                  long double   maximal   , 
                                  unsigned long number    ) const
{
  /// the "current measurement will be discarded! 
  long double microsecond  =  1.0               ;     /// unit is microsecond 
  long double millisecond  = 1000 * microsecond ;
  long double second       = 1000 * millisecond ; 
  long double minute       =   60 *      second ; 
  long double hour         =   60 *      minute ; 
  long double day          =   24 *        hour ; 
  long double week         =    7 *         day ; 
  long double month        =   30 *         day ;
  long double year         =  365 *         day ;
  ///  
  const int buffer_size         = 256 ;
  char      buffer[buffer_size]       ;
  int       index               = 0   ;
  ///
  long double  unit = 1.0 ;
  std::string cunit = ""  ; 
  ///  
  if     ( total / microsecond  <  500 ) 
    { unit = microsecond ; cunit = " [us]"    ; } 
  else if( total / millisecond  <  500 )
    { unit = millisecond ; cunit = " [ms]"    ; } 
  else if( total /      second  <  500 ) 
    { unit =      second ; cunit = "  [s]"     ; }
  else if( total /      minute  <  500 )
    { unit =      minute ; cunit = "[min]"   ; }
  else if( total /        hour  <  500 ) 
    { unit =        hour ; cunit = "  [h]"     ; }
  else if( total /         day  <   10 )
    { unit =         day ; cunit = "[day]"   ; }
  else if( total /        week  <    5 ) 
    { unit =        week ; cunit = "  [w]"  ; }
  else if( total /       month  <   20 ) 
    { unit =       month ; cunit = "[mon]" ; }
  else                                   
    { unit =        year ; cunit = "  [y]"  ; } 
  ///
  index += sprintf( buffer + index , "Tot=%5.3g" , (double) (total / unit) ) ;
  index += sprintf( buffer + index , "%s" , cunit.c_str()          ) ;
  ///
  if( 1 < number )
    {
      
      if     ( mean / microsecond  <  500 ) 
        { unit = microsecond ; cunit = " [us]"    ; } 
      else if( mean / millisecond  <  500 )
        { unit = millisecond ; cunit = " [ms]"    ; } 
      else if( mean /      second  <  500 ) 
        { unit =      second ; cunit = "  [s]"     ; }
      else if( mean /      minute  <  500 )
        { unit =      minute ; cunit = "[min]"   ; }
      else if( mean /        hour  <  500 ) 
        { unit =        hour ; cunit = "  [h]"     ; }
      else if( mean /         day  <   10 )
        { unit =         day ; cunit = "[day]"   ; }
      else if( mean /        week  <    5 ) 
        { unit =        week ; cunit = "  [w]"  ; }
      else if( mean /       month  <   20 ) 
        { unit =       month ; cunit = "[mon]" ; }
      else                                   
        { unit =        year ; cunit = "[year]"  ; } 
      ///      
      index += 
        sprintf( buffer + index , 
                 " Ave/Min/Max=%5.3g" , (double) ( mean / unit ) ) ;
      index += 
        sprintf( buffer + index , 
                 "(+-%5.3g)"          , (double) ( rms  / unit ) ) ;
      index += 
        sprintf( buffer + index , 
                 "/%5.3g"             , (double) ( minimal  / unit ) ) ;
      index += 
        sprintf( buffer + index , 
                 "/%5.3g"             , (double) ( maximal  / unit ) ) ;
      ///
      index += sprintf( buffer + index , "%s" , cunit.c_str() );
      ///
    }
  else
    { index += sprintf( buffer + index , "%44s" , " " ); }    
    ///
  index += sprintf( buffer + index , " #=%3lu" , number );
  ///
  return std::string( buffer, 0 , index );
  ///
}
// ============================================================================

// ============================================================================
/// comparison operator 
// ============================================================================
bool ChronoEntity::operator<( const ChronoEntity& e ) const
{ 
  return 
    ( totalTime () < e.totalTime () ) ?  true :
    ( totalTime () > e.totalTime () ) ? false :    
    ( eTotalTime() < e.eTotalTime() ) ?  true : 
    ( eTotalTime() > e.eTotalTime() ) ? false : 
    ( uTotalTime() < e.uTotalTime() ) ?  true : 
    ( uTotalTime() > e.uTotalTime() ) ? false : false ; 
}
