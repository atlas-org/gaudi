// $Id: ExceptionSvc.h,v 1.4 2007/05/24 14:41:22 hmd Exp $ 
// ============================================================================
// CvS tag $Name:  $, version $Revision: 1.4 $
// ============================================================================
#ifndef GAUDISVC_EXCEPTIONSVC_H
#define GAUDISVC_EXCEPTIONSVC_H
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IExceptionSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
// ============================================================================
/** @class ExceptionSvc 
 *  Simple implementationof IExceptionSvc abstract interface 
 *  @author (1) ATLAS collaboration
 *  @author (2) modified by Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @daet 2007-03-08
 */
// ============================================================================
class ExceptionSvc 
  : public               Service
  , public virtual IExceptionSvc 
{
  friend class SvcFactory<ExceptionSvc> ;
public:
  /// Handle caught GaudiExceptions
  virtual StatusCode handle
  ( const INamedInterface& o , 
    const GaudiException&  e ) const     ; ///< Handle caught exceptions
  /// Handle caught std::exceptions
  virtual StatusCode handle
  ( const INamedInterface& o , 
    const std::exception & e ) const     ; ///< Handle caught exceptions
  /// Handle caught (unknown)exceptions
  virtual StatusCode handle
  ( const INamedInterface& o ) const     ; ///< Handle caught exceptions
  /// Handle errors
  virtual StatusCode handleErr
  ( const INamedInterface& o , 
    const StatusCode&      s ) const     ; ///< Handle errors 
public: 
  /// initialize the service 
  virtual StatusCode initialize   () ;
  /// finalize the service 
  virtual StatusCode finalize     () ;
public: 
  /**  query the interface 
   *   @see IInterface 
   */
  virtual StatusCode queryInterface
  ( const InterfaceID& iid  , 
    void**             ppvi ) ;
public:
  /** standard constructor
   *  @param name service instance name 
   *  @param pSvc pointer to Service Locator
   */
  ExceptionSvc 
  ( const std::string& name , 
    ISvcLocator*       svc  ) ;
  /// Destructor.
  virtual ~ExceptionSvc();
private:
  // default constructor is disabled 
  ExceptionSvc () ; ///< no default constructor  
  // copy constructor is disabled 
  ExceptionSvc ( const ExceptionSvc& ) ; ///< no copy constructor
  // assignement operator is disabled 
  ExceptionSvc& operator=( const ExceptionSvc& ) ; ///< no assignement
private:

  enum ExceptState { ALL, NONE, LIST };
  enum ReturnState { DEFAULT, SUCCESS, FAILURE, RECOVERABLE, RETHROW };

  std::string m_mode_s;
  ExceptState m_mode;
  StringArrayProperty m_algs_v;
  std::set<std::string> m_algs;
  std::map<std::string,ReturnState> m_retCodes;

};

// ============================================================================
#endif // GAUDISVC_EXCEPTIONSVC_H
// ============================================================================
// The END 
// ============================================================================
