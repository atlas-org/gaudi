// $Id: IExceptionSvc.h,v 1.3 2007/05/24 14:39:11 hmd Exp $ 
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.3 $
// ============================================================================
#ifndef GAUDIKERNEL_IEXCEPTIONSVC_H
#define GAUDIKERNEL_IEXCEPTIONSVC_H
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/GaudiException.h"
// ============================================================================
// Forard declarations 
// ============================================================================
class INamedInterface ;
// ============================================================================
/** @class IExceptionSvc 
 *  The anstract interface for exception handling service 
 *  @see GaudiException
 *  @author (1) ATLAS collaboration 
 *  @author (2) modified by Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-08
 */
// ============================================================================
class IExceptionSvc : virtual public IInterface 
{
public:
  /// Handle caught GaudiExceptions
  virtual StatusCode handle
  ( const INamedInterface& o , 
    const GaudiException&  e ) const = 0 ; ///< Handle caught exceptions
  /// Handle caught std::exceptions
  virtual StatusCode handle
  ( const INamedInterface& o , 
    const std::exception & e ) const = 0 ; ///< Handle caught exceptions
  /// Handle caught (unknown)exceptions
  virtual StatusCode handle
  ( const INamedInterface& o ) const = 0 ; ///< Handle caught exceptions
  /// Handle errors
  virtual StatusCode handleErr
  ( const INamedInterface& o , 
    const StatusCode&      s ) const = 0 ; ///< Handle errors 
public:
  // The unique interface indetifier 
  static const InterfaceID& interfaceID(); ///< The unique interface indetifier 
protected :
  // virtual and proteted destructor 
  virtual ~IExceptionSvc() ; ///< virtual and proteted destructor 
} ;
// ============================================================================


// ============================================================================
#endif // GAUDIKERNEL_IEXCEPTIONSVC_H
// ============================================================================
// The END 
// ============================================================================
