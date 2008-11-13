#ifndef        __GAUDI_INTERFACES_ITIME_H__
#define        __GAUDI_INTERFACES_ITIME_H__

#include <iostream> 
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/MsgStream.h"

//
//   Class ITime:  a crude interface to any time-stamps
//                 provide minimal interface for stamp 
//                 manipulation with "time" object. 
//        
//                 One should distinguish the notion of
//                 time-stamp and time-difference.
//                 e.g. unit of "month" is good for 
//                 "time-stamp" and it is unacceptable for 
//                 "time-difference, since the month can 
//                 consists of 30 or 31 day. The same argument
//                 is applied to "year"
//                 Another difference is from teh notion of "0"-year
// 
//                 This interface is common for "time-stamp"
//                 and "time-difference"
//                 


class ITime
{
public:  
  //
  // internal representation
  // (probably in ns units - according to CLHEP/Units/SystemOfUnits.h
  // if it is so 1 year == (approx) (10**9) * (3*10**7) = 3*10^16 "units"
  //
  // representation in given units
  typedef longlong       AbsoluteTime;
  typedef double         DimensionedTime;


  
 public: 
  
  //
  // accessor to internal representation 
  virtual       AbsoluteTime      absoluteTime  () const = 0; 
  
  // accessor to representation in seconds 
  virtual       DimensionedTime   seconds       () const = 0;      

  // accessor to representation in minutes 
  virtual       DimensionedTime   minutes       () const = 0;      

  // accessor to representation in hours
  virtual       DimensionedTime   hours         () const = 0; 

  // accessor to representation in days 
  virtual       DimensionedTime   days          () const = 0;
  
  
  // comparison operators
  virtual       bool              operator==( const ITime& ) const = 0;
  virtual       bool              operator!=( const ITime& ) const = 0;
  virtual       bool              operator<=( const ITime& ) const = 0;    
  virtual       bool              operator>=( const ITime& ) const = 0;
  virtual       bool              operator< ( const ITime& ) const = 0;    
  virtual       bool              operator> ( const ITime& ) const = 0;
  
  // adding   
  virtual       ITime&            operator+=( const ITime& ) = 0 ; 
  
  // substraction
  virtual       ITime&            operator-=( const ITime& ) = 0 ; 
  
  virtual std::ostream&  printOut( std::ostream& ) const = 0;  

  // destructor 
  virtual ~ITime(){};

};

//
//
//

inline std::ostream& operator<<(std::ostream& os , const ITime& time ) {
  return  time.printOut(os); 
} 

inline MsgStream& operator<<(MsgStream& ms , const ITime& time ) {
  if ( ms.isActive() ) time.printOut(ms.stream()); 
  return ms;
} 

//
//
//

#endif   //    __GAUDI_INTERFACES_ITIME_H__
