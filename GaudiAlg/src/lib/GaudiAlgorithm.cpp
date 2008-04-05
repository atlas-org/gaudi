// $Id: GaudiAlgorithm.cpp,v 1.11 2007/09/25 16:12:41 marcocle Exp $ 
// ============================================================================
#define  GAUDIALG_GAUDIALGORITHM_CPP 1
// ============================================================================
// include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
/** @file
 *
 *  Implementation file for the class GaudiAlgorithm
 *
 *  @see GaudiAlgorithm
 *  @see Algorithm
 *  @see IAlgorithm
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @author Christopher Jones  Christopher.Rob.Jones@cern.ch
 *  @date 30/06/2001
 */
// ============================================================================
// templated methods
// ============================================================================
#include "GaudiCommon.icpp"
// ============================================================================
template class GaudiCommon<Algorithm>;
// ============================================================================
// Standard algorithm like constructor
// ============================================================================
GaudiAlgorithm::GaudiAlgorithm ( const std::string&  name        ,
                                 ISvcLocator*        pSvcLocator )
  : GaudiCommon<Algorithm> ( name , pSvcLocator )
  //
  , m_evtColSvc  ( 0        ) // pointer to Event Tag Collection Service
  , m_contextSvc ( 0        ) // pointer to Algorithm Context Service
  , m_contextSvcName ( "AlgContextSvc" ) // Algorithm Context Service name 
  // enforce the algorithm registration for Algorithm Context Service
  , m_registerContext ( true )
{
  declareProperty 
    ( "ContextService" , 
      m_contextSvcName , 
      "The name of Algorithm Context Service" ) ;
  declareProperty 
    ( "RegisterForContextService" , 
      m_registerContext  , 
      "The flag to enforce the registration for Algorithm Context Service") ;
}
// ============================================================================
// Destructor
// ============================================================================
GaudiAlgorithm::~GaudiAlgorithm() { }
// ============================================================================
// standard initialization method
// ============================================================================
StatusCode GaudiAlgorithm::initialize()
{
  // initialize the base class
  const StatusCode sc = GaudiCommon<Algorithm>::initialize() ;
  if ( sc.isFailure() ) { return sc; }

  // Add any customisations here, that cannot go in GaudiCommon

  // return
  return sc;
}
// ============================================================================
// standard finalization method
// ============================================================================
StatusCode GaudiAlgorithm::finalize()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Finalize base class GaudiAlgorithm" << endreq;

  // reset pointers
  m_evtColSvc = 0 ;

  // finalize the base class and return
  return GaudiCommon<Algorithm>::finalize() ;
}
// ============================================================================
// standard execution method
// ============================================================================
StatusCode GaudiAlgorithm::execute()
{
  return Error ( "Default GaudiAlgorithm execute method called !!" ) ;
}
// ============================================================================
// The standard event collection service
// ============================================================================
INTupleSvc* GaudiAlgorithm::evtColSvc() const
{
  if ( 0 == m_evtColSvc )
  {
    m_evtColSvc = svc< INTupleSvc > ( "EvtTupleSvc" , true ) ;
  }
  return m_evtColSvc ;
}
// ============================================================================
// The standard Algorithm Context Service 
// ============================================================================
IAlgContextSvc* GaudiAlgorithm::contextSvc() const
{
  if ( 0 == m_contextSvc )
  {
    m_contextSvc = svc< IAlgContextSvc > ( m_contextSvcName , true ) ;
  }
  return m_contextSvc ;
}
// ============================================================================
/*  The generic actions for the execution.
 *  @see  Algorithm
 *  @see IAlgorithm
 *  @see Algorithm::sysExecute
 *  @return status code 
 */
// ============================================================================
StatusCode GaudiAlgorithm::sysExecute () 
{
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // Lock the context 
  Gaudi::Utils::AlgContext cnt ( ctx , this ) ;  ///< guard/sentry
  /// execute the generic method:
  return Algorithm::sysExecute() ;
}
// ============================================================================




// ============================================================================
// The END 
// ============================================================================

