// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/Chrono.h,v 1.5 2007/01/12 10:56:09 hmd Exp $
#ifndef     GAUDIKERNEL_CHRONO_H
#define     GAUDIKERNEL_CHRONO_H

// Include files
#include "GaudiKernel/Kernel.h" 
#include "GaudiKernel/IChronoStatSvc.h" 
#include <string>


/** @class Chrono Chrono.h GaudiKernel/Chrono.h

    A small utility class for chronometry of user codes

    @author Vanya Belyaev
    @date    Nov 26, 1999   
*/
class Chrono {
 public: 
  /// Constructor, start the chrono 
  Chrono( IChronoStatSvc*        chronoSvc = 0                 , 
	  const std::string&     chronoTag = "CHRONO::UNNAMED" )
    : m_chronoSvc( chronoSvc ) 
      , m_chronoTag( chronoTag ) {
    if( 0 != m_chronoSvc ) { 
      m_chronoSvc->chronoStart( m_chronoTag ) ; 
    }  
  };
  /// Destructor , stop the chrono 
  ~Chrono() { 
    if( 0 != m_chronoSvc ) { 
      m_chronoSvc->chronoStop ( m_chronoTag ) ; 
    } 
  }; 
 private:
  /// Chrono service   
  mutable IChronoStatSvc* m_chronoSvc; 
  /// Chrono tag 
  std::string             m_chronoTag; 
};

#endif  //  GAUDIKERNEL_CHRONO_H

