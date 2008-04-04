// $Id: IGslErrorHandler.h,v 1.2 2003/11/19 16:55:57 mato Exp $
// ============================================================================
#ifndef GAUDIGSL_IGSLERRORHANDLER_H 
#define GAUDIGSL_IGSLERRORHANDLER_H 1
// Include files
#include "GaudiKernel/IAlgTool.h"
// local 
#include "GaudiGSL/IIDIGslErrorHandler.h"
// forward declarations 
class GslError;


/** @class IGslErrorHandler IGslErrorHandler.h GaudiGSL/IGslErrorHandler.h
 *  
 *  The abstract interface for arbitrary GSL error handler  
 *
 *  @author Vanya Ivan.Belyaev@itep.ru
 *  @date   30/04/2002
 */
class IGslErrorHandler : 
  public virtual IAlgTool 
{
public:
  
  /** the unique interface identifier (static)  
   *  @return unique interface identifier 
   *  @see IInterface
   */
  static const InterfaceID& interfaceID() { return IID_IGslErrorHandler; }
  
  /** handle the GSL error 
   *  @param error  error to be handled 
   *  @see GslError
   *  @return status code 
   */
  virtual StatusCode handle ( const GslError& error ) const = 0 ;
  
protected:

  /// destructor (virtual and protected)
  virtual ~IGslErrorHandler(){};

};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUDIGSL_IGSLERRORHANDLER_H
// ============================================================================
