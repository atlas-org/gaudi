// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/TimePoint.h,v 1.8 2006/12/10 20:29:17 leggett Exp $
#ifndef GAUDIKERNEL_TIMEPOINT_H
#define GAUDIKERNEL_TIMEPOINT_H

// Include files
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/StreamBuffer.h"
#include <time.h>


/** @class TimePoint TimePoint.h GaudiKernel/TimePoint.h

    A very crude and rought implementation of ITime interface.
    The internal precision is nanosecond (1.0e-9 seconds)

*/
class TimePoint: public ITime
{
public:
  /// Constructors
  TimePoint() : m_time( time(0) * (longlong)1.0e+9 ) { }
  TimePoint( const ITime::AbsoluteTime& atime ) : m_time( atime ) { }  
  TimePoint( const ITime& itime ) : m_time( itime.absoluteTime() ) { }
  /// Virtual destructor
  ~TimePoint(){ }
  
 public:  
  /// Accessor to internal representation 
  ITime::AbsoluteTime  absoluteTime() const { return m_time; }
  
  /// accessor to representation in seconds 
  ITime::DimensionedTime seconds() const { return (DimensionedTime) ( absoluteTime() / 1.0e+9 ) ; }      
  
  /// accessor to representation in minutes 
  ITime::DimensionedTime minutes() const { return seconds() / 60.0 ; }     
  
  /// accessor to representation in hours
  ITime::DimensionedTime hours() const { return minutes() / 60.0 ; }
  
  /// accessor to representation in days 
  ITime::DimensionedTime days() const { return hours  () / 24.0 ; }
  
  /// most significant bits
  unsigned long msb() const { return (unsigned long)(m_time >> 32);}
  /// less significant bits
  unsigned long lsb() const { return (unsigned long)((m_time << 32) >> 32);}

  /// comparison operators
  bool operator==( const ITime& T ) const { return absoluteTime()==T.absoluteTime(); }
  bool operator!=( const ITime& T ) const { return absoluteTime()!=T.absoluteTime(); }
  bool operator<=( const ITime& T ) const { return absoluteTime()<=T.absoluteTime(); }
  bool operator>=( const ITime& T ) const { return absoluteTime()>=T.absoluteTime(); }
  bool operator< ( const ITime& T ) const { return absoluteTime()< T.absoluteTime(); }
  bool operator> ( const ITime& T ) const { return absoluteTime()> T.absoluteTime(); }
  
  /// adding   
  ITime& operator+=( const ITime& T );

  /// substract   
  ITime& operator-=( const ITime& T );
  
  /// printout   
  virtual std::ostream&  printOut( std::ostream& ) const;  
    
 private:
  
  ITime::AbsoluteTime  m_time;    
  friend StreamBuffer& operator>>(StreamBuffer& s, TimePoint& t);
};


  static const  TimePoint time_absolutepast((longlong)LONGLONG_MIN);
  static const  TimePoint time_absolutefuture((longlong)LONGLONG_MAX);

inline ITime& TimePoint::operator+=( const ITime& T ) 
{ 
  m_time +=T.absoluteTime(); 
  m_time = ( m_time > time_absolutefuture.absoluteTime() )  ? time_absolutefuture.absoluteTime() : m_time ; 
  m_time = ( m_time < time_absolutepast  .absoluteTime() )  ? time_absolutepast  .absoluteTime() : m_time ; 
  return *this; 
} 


inline ITime& TimePoint::operator-=( const ITime& T ) 
{ 
  m_time -= T.absoluteTime(); 
  m_time  = ( m_time > time_absolutefuture.absoluteTime() )  ? time_absolutefuture.absoluteTime() : m_time ; 
  m_time  = ( m_time < time_absolutepast  .absoluteTime() )  ? time_absolutepast  .absoluteTime() : m_time ; 
  return *this; 
} 

inline std::ostream&  TimePoint::printOut( std::ostream& os ) const  {
  return os << "Time is " << seconds() << "[s]";
}

inline MsgStream& operator<<(MsgStream& os, const TimePoint& t)  {
  if ( os.isActive() ) t.printOut(os.stream());
  return os;
}

// Output serialization
inline StreamBuffer& operator<<(StreamBuffer& s, const TimePoint& t)  {
  return s << t.absoluteTime();
}
// Input serialization
inline StreamBuffer& operator>>(StreamBuffer& s, TimePoint& t)  {
  return s >> t.m_time;
}
#endif   //   GAUDIKERNEL_TIMEPOINT_H
