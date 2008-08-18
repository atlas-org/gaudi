// $Id: ChronoEntity.h,v 1.1 2004/08/05 09:58:08 mato Exp $
// ============================================================================ 
#ifndef GAUDIKERNEL_CHRONOENTITY_H
#define GAUDIKERNEL_CHRONOENTITY_H 1 
// ============================================================================ 
// Include files 
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/IChronoStatSvc.h" 
// ============================================================================

/** @class ChronoEntity ChronoEntity.h GaudiKernel/ChronoEntity.h 
 *  a small helper class for 
 *  implementation of ChronoStatSvc service 
 * 
 *  @author   Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date December 1, 1999
 */

class ChronoEntity 
{
public:
  /// default constructor
  ChronoEntity();
  /// destructor 
  ~ChronoEntity();
  ///
public: 
  /// start the current chrono   
  IChronoStatSvc::ChronoStatus      start() ;
  /// stop the chrono
  IChronoStatSvc::ChronoStatus      stop () ;
  /// return the last delta-time of type "type"
  IChronoStatSvc::ChronoTime        delta ( IChronoStatSvc::ChronoType type ) ;
  /// print the chrono;
  std::string                outputUserTime     () const ; 
  /// print the chrono;
  std::string                outputSystemTime   () const ; 
  /// print the chrono;
  std::string                outputElapsedTime  () const ;
  /// return the status of chrono
  inline IChronoStatSvc::ChronoStatus     status() const
  { return m_ce_currentStatus; };
  //// access to Chrono functions:
  /// number of chrono measurements
  inline unsigned long        nOfMeasurements   () const
  { return m_ce_noOfMeasurements;};
  /// minimal measurement for user time
  inline long double          uMinimalTime      () const
  { return (long double)m_ce_minimalUserTime         ; };
  /// minimal measurement for kernel time
  inline long double          kMinimalTime      () const
  { return (long double)m_ce_minimalKernelTime       ; };
  /// minimal measurement for ellapsed time
  inline long double          eMinimalTime      () const
  { return (long double)m_ce_minimalElapsedTime     ; };
  /// maximal measurement for user time
  inline long double          uMaximalTime      () const
  { return (long double)m_ce_maximalUserTime         ; };
  /// maximal measurement for kernel time
  inline long double          kMaximalTime      () const
  { return (long double)m_ce_maximalKernelTime       ; };
  /// maximal measurement for ellapsed time
  inline long double          eMaximalTime      () const
  { return (long double)m_ce_maximalElapsedTime     ; };
  /// total user time
  inline long double          uTotalTime        () const
  { return (long double)m_ce_totalUserTime           ; };
  /// total Kernel time
  inline long double          kTotalTime        () const
  { return (long double)m_ce_totalKernelTime         ; };
  /// total Elapsed time
  inline long double          eTotalTime        () const
  { return (long double)m_ce_totalElapsedTime       ; };
  /// total time
  inline long double          totalTime         () const
  { return uTotalTime() + kTotalTime()  ; };
  /// sum of squared user   time intervals
  inline long double          uStatistics       () const
  { return (long double)m_ce_statisticsForUserTime   ; };
  /// sum of squared Kernel time intervals
  inline long double          kStatistics       () const
  { return (long double)m_ce_statisticsForKernelTime ; };
  /// sum of squared Elapsed time intervals
  inline long double          eStatistics       () const
  { return (long double)m_ce_statisticsForElapsedTime ; };
  /// statistics
  /// average Kernel Time
  long double                 kMeanTime         () const ;
  /// average User   Time
  long double                 uMeanTime         () const ;
  /// average Elapsed   Time
  long double                 eMeanTime         () const ;
  /// r.m.s Kernel Time
  long double                 kRMSTime          () const ;
  /// r.m.s User Time
  long double                 uRMSTime          () const ;
  /// r.m.s Elapsed Time
  long double                 eRMSTime          () const ;
  /// error in mean Kernel time
  long double                 kMeanErrorTime    () const ;
  /// error in mean User   time
  long double                 uMeanErrorTime    () const ;
  /// error in mean Elapsed   time
  long double                 eMeanErrorTime    () const ;
  /// comparison operator
  bool operator<( const ChronoEntity& entity ) const;
  ///
protected:
  ///
  std::string format 
  ( long double   total     ,
    long double   minimal   ,
    long double   mean      ,
    long double   rms       ,
    long double   maximal   ,
    unsigned long number    ) const ;
  ///
private:
  /// current status of this chrono object;
  IChronoStatSvc::ChronoStatus m_ce_currentStatus;
  /// number of chrono measurements
  unsigned long                m_ce_noOfMeasurements;
  /// minimal measurement for user time
  IChronoStatSvc::ChronoTime   m_ce_minimalUserTime;
  /// minimal measurement for kernel time
  IChronoStatSvc::ChronoTime   m_ce_minimalKernelTime;
  /// minimal measurement for ellapsed time
  IChronoStatSvc::ChronoTime   m_ce_minimalElapsedTime;
  /// maximal measurement for user time
  IChronoStatSvc::ChronoTime   m_ce_maximalUserTime;
  /// maximal measurement for kernel time
  IChronoStatSvc::ChronoTime   m_ce_maximalKernelTime;
  /// maximal measurement for ellapsed time
  IChronoStatSvc::ChronoTime   m_ce_maximalElapsedTime;
  /// delta user time
  IChronoStatSvc::ChronoTime   m_ce_deltaUser;
  /// delta Kernel time
  IChronoStatSvc::ChronoTime   m_ce_deltaKernel;
  /// delta Elapsed time
  IChronoStatSvc::ChronoTime   m_ce_deltaElapsed;
  /// total user time
  IChronoStatSvc::ChronoTime   m_ce_totalUserTime;
  /// total Kernel time
  IChronoStatSvc::ChronoTime   m_ce_totalKernelTime;
  /// total Elapsed time
  IChronoStatSvc::ChronoTime   m_ce_totalElapsedTime;
  /// sum of squared user   time intervals
  IChronoStatSvc::ChronoTime   m_ce_statisticsForUserTime;
  /// sum of squared Kernel time intervals
  IChronoStatSvc::ChronoTime   m_ce_statisticsForKernelTime;
  /// sum of squared Ellpsed time intervals
  IChronoStatSvc::ChronoTime   m_ce_statisticsForElapsedTime;
  /// start stamp for current measurement of user time
  IChronoStatSvc::ChronoTime   m_ce_startOfCurrentMeasurementOfUserTime;
  /// start stamp for current measurement of Kernel time
  IChronoStatSvc::ChronoTime   m_ce_startOfCurrentMeasurementOfKernelTime;
  /// start stamp for current measurement of Elapsed time
  IChronoStatSvc::ChronoTime   m_ce_startOfCurrentMeasurementOfElapsedTime;
  ///
};

#endif  //  GAUDIKERNEL_CHRONOENTITY_H










