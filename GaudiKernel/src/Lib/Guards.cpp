// $Id: Guards.cpp,v 1.3 2008/04/03 18:28:49 marcocle Exp $
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.3 $
// ============================================================================
// $Log: Guards.cpp,v $
// Revision 1.3  2008/04/03 18:28:49  marcocle
// Fixed Windows compilation problem.
//
// Revision 1.2  2008/04/03 14:40:19  marcocle
// Marco Clemencic
//  - Patch #1725. New IAuditor interface.
//    - The Auditor member functions are now only 8, for all the combinations of:
//      * before or after
//      * standard event or custom event
//      * INamedInterface* or string
//    - All the after methods accept a StatusCode (if not specified, a SUCCESS with
//      the checked flag set is used).
//    - The obsolete member functions are kept for backward compatibility.
//    - The default implementations of "before" and "after" are using the obsolete
//      methods to make legacy Auditors to work.
//    - IAuditorSvc is now an extension of IAuditor.
//    - AuditorGuard has been adapted to the new interface and the audit with a
//      status code check is simpler.
//    - The standard event types defined in the enum IAuditor::StandardEventType
//      can be printed and converted to strings with operator<<.
//
// Revision 1.1  2007/07/24 17:59:57  marcocle
// Patch #1246 -Vanya Belyaev
//    Introduced 2 new classes:
//      * Gaudi::Guards::ExceptionGuard
//        executes a functor in a "standard" try-catch block.
//      * Gaudi::Guards::AuditorGuard
//        executes a function of an object when the instance is created and
//        a second one when destroied, allowing automatic "clean-up" (in case
//        of early exit).
//    See GaudiKernel/Gaurds.h (doxygen) for more details.
//
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <exception>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Guards.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
// ============================================================================
/** @file 
 *  Implementation file for class Gaudi::Guards::ExceptionGuard 
 *  and class Gaudi::Guards::AuditorGuard 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-07
 */
// ============================================================================
// Local handle (print) of GaudiException
// ============================================================================
void Gaudi::Guards::ExceptionGuard::handle
( const GaudiException& exc , MsgStream& log )
{
  // the general printout 
  log << MSG::FATAL 
      << System::typeinfoName( typeid ( exc ) ) 
      << "('" << exc.tag() << "') is caught!" << endreq ;
  // print the detailes about the exception:
  log << MSG::ERROR << exc << endreq ;
  // get the status code form the exception:
  m_sc = exc.code() ;
}
// ============================================================================
// Local handle (print) of std::exception
// ============================================================================
void Gaudi::Guards::ExceptionGuard::handle
( const std::exception& exc , MsgStream& log )
{
  // the general printout 
  log << MSG::FATAL 
      << System::typeinfoName( typeid ( exc ) ) << " is caught!" << endreq ;
  // print the detailes abotu the exception:
  log << MSG::ERROR << exc.what() << endreq ;
}
// ============================================================================
// Local handle (print) of unknown exception
// ============================================================================
void Gaudi::Guards::ExceptionGuard::handle
( MsgStream& log )
{
  // the general printout 
  log << MSG::FATAL << "UNKNOWN exception is caught!" << endreq ;
}
// ============================================================================
// dectructor
// ============================================================================
Gaudi::Guards::ExceptionGuard::~ExceptionGuard() { m_sc.ignore() ; }
// ============================================================================
// constructor with standard post-action
// ============================================================================
Gaudi::Guards::AuditorGuard::AuditorGuard ( INamedInterface*             obj      , 
                              IAuditor*                    svc      ,
                              IAuditor::StandardEventType  evt      ):
                 m_obj(obj),
                 m_objName(),
                 m_svc(svc),
                 m_evt(evt),
                 m_cevt(),
                 m_sc(0),
                 m_customEvtType(false)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( INamedInterface*          obj      , 
                              IAuditor*                       svc      ,
                              IAuditor::CustomEventTypeRef    evt      ):
                 m_obj(obj),
                 m_objName(),
                 m_svc(svc),
                 m_evt(IAuditor::Initialize), // Windows needs an explicit value
                 m_cevt(evt),
                 m_sc(0),
                 m_customEvtType(true)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( INamedInterface*             obj      , 
    IAuditor*                    svc      ,
    IAuditor::StandardEventType  evt      ,
    const StatusCode             &sc       ):
      m_obj(obj),
      m_objName(),
      m_svc(svc),
      m_evt(evt),
      m_cevt(),
      m_sc(&sc),
      m_customEvtType(false)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( INamedInterface*                   obj      , 
    IAuditor*                          svc      ,
    IAuditor::CustomEventTypeRef       evt      ,
    const StatusCode                  &sc       ):
      m_obj(obj),
      m_objName(),
      m_svc(svc),
      m_evt(IAuditor::Initialize), // Windows needs an explicit value
      m_cevt(evt),
      m_sc(&sc),
      m_customEvtType(true)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( const std::string           &name      , 
    IAuditor*                    svc      ,
    IAuditor::StandardEventType  evt      ):
      m_obj(0),
      m_objName(name),
      m_svc(svc),
      m_evt(evt),
      m_cevt(),
      m_sc(0),
      m_customEvtType(false)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( const std::string           &name      , 
    IAuditor*                          svc      ,
    IAuditor::CustomEventTypeRef       evt      ):
      m_obj(0),
      m_objName(name),
      m_svc(svc),
      m_evt(IAuditor::Initialize), // Windows needs an explicit value
      m_cevt(evt),
      m_sc(0),
      m_customEvtType(true)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( const std::string           &name      , 
    IAuditor*                    svc      ,
    IAuditor::StandardEventType  evt      ,
    const StatusCode            &sc       ):
      m_obj(0),
      m_objName(name),
      m_svc(svc),
      m_evt(evt),
      m_cevt(),
      m_sc(&sc),
      m_customEvtType(false)
{
  i_before();
}
Gaudi::Guards::AuditorGuard::AuditorGuard ( const std::string           &name      , 
    IAuditor*                          svc      ,
    IAuditor::CustomEventTypeRef       evt      ,
    const StatusCode                  &sc       ):
      m_obj(0),
      m_objName(name),
      m_svc(svc),
      m_evt(IAuditor::Initialize), // Windows needs an explicit value
      m_cevt(evt),
      m_sc(&sc),
      m_customEvtType(true)
{
  i_before();
}
// ============================================================================
// dectructor
// ============================================================================
Gaudi::Guards::AuditorGuard::~AuditorGuard() 
{
  i_after();
}
// ============================================================================
/// The END 
// ============================================================================
