// $Id: AlgContextSvc.cpp,v 1.4 2007/05/24 13:49:47 hmd Exp $
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.4 $
// ============================================================================
// $Log: AlgContextSvc.cpp,v $
// Revision 1.4  2007/05/24 13:49:47  hmd
// ( Vanya Belyaev) patch #1171. The enhancement of existing Algorithm Context Service
//    is the primary goal of the proposed patch. The existing
//    AlgContextSvc is not safe with respect to e.g. Data-On-Demand
//    service or to operations with subalgorithms. The patched service
//    essentially implements the queue of executing algorithms, thus the
//    problems are eliminiated. In addition the enriched interface
//    provides the access to the whole queue of executing algorithms.
// 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// Local
// ============================================================================
#include "AlgContextSvc.h"
// ============================================================================
/** @file
 *  Implementation firl for class AlgContextSvc
 *  @author ATLAS Collaboration
 *  @author modified by Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-05 (modified)
 */
// ============================================================================
/** Instantiation of a static factory class used by clients to create
 *  instances of this service
 */
DECLARE_SERVICE_FACTORY(AlgContextSvc) 
// ============================================================================
// Standard Constructor
// ============================================================================
AlgContextSvc::AlgContextSvc
( const std::string& name , 
  ISvcLocator*       svc  )
  : Service ( name , svc  )
  , m_algorithms (  0   )
  , m_inc        (  0   ) 
  , m_check      ( true )
{
  declareProperty ( "Check" , m_check , "Flag to perform more checks" );
}
// ============================================================================
// Standard Destructor
// ============================================================================
AlgContextSvc::~AlgContextSvc() {}
// ============================================================================
// Query the interfaces.
// ============================================================================
StatusCode AlgContextSvc::queryInterface
( const InterfaceID& riid , 
  void**             ppvi ) 
{
  if      ( 0 == ppvi ) { return StatusCode::FAILURE   ; }         // RETURN 
  else if ( IAlgContextSvc    ::interfaceID() == riid )    
  { *ppvi = static_cast<IAlgContextSvc*>    ( this ) ; }
  else if ( IIncidentListener ::interfaceID() == riid )    
  { *ppvi = static_cast<IIncidentListener*> ( this ) ; }
  else { return Service::queryInterface ( riid , ppvi ) ; }         // RETURN 
  // increment the reference count!
  addRef();                          ///< increment the reference count!
  return StatusCode::SUCCESS ;
}
// ============================================================================
// standard initialization of the service 
// ============================================================================
StatusCode AlgContextSvc::initialize () 
{
  // intialize the base class 
  StatusCode sc = Service::initialize () ;
  if ( sc.isFailure () ) { return sc ; }
  // Incindent Service 
  if ( 0 != m_inc     ) 
  {
    m_inc -> removeListener ( this ) ;
    m_inc -> release() ;
    m_inc = 0 ;
  }
  // perform more checks?
  if ( m_check ) 
  { 
    sc = Service::service ( "IncidentSvc" , m_inc , true ) ;
    if ( sc.isFailure() )
    {
      MsgStream log ( msgSvc() , name() )  ;
      log << MSG::ERROR << "Could not locate 'IncidentSvc'" << sc << endreq ;
      return sc ;                                               // RETURN
    }
    if ( 0 == m_inc ) 
    {
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ERROR << "Invalid pointer to IIncindentSvc" << endreq ;
      return StatusCode::FAILURE ;                               // RETURN 
    }    
    m_inc -> addListener ( this , IncidentType::BeginEvent ) ;
    m_inc -> addListener ( this , IncidentType::EndEvent   ) ;
  }  
  if ( !m_algorithms.empty() )
  {
    MsgStream log ( msgSvc() , name() ) ;
    log << MSG::WARNING
        << "Non-empty stack of algorihtms #"
        << m_algorithms.size() << endreq ;
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// standard finalization  of the service  @see IService
// ============================================================================
StatusCode AlgContextSvc::finalize   () 
{
  if ( !m_algorithms.empty() )
  {
    MsgStream log ( msgSvc() , name() ) ;
    log << MSG::WARNING
        << "Non-empty stack of algorihtms #"
        << m_algorithms.size() << endreq ;
  }
  // Incindent Service 
  if ( 0 != m_inc     ) 
  {
    m_inc -> removeListener ( this ) ;
    m_inc -> release() ;
    m_inc = 0 ;
  }
  // finalize the base class 
  return Service::finalize () ;
}
// ============================================================================
// set     the currently executing algorithm  ("push_back") @see IAlgContextSvc
// ============================================================================
StatusCode AlgContextSvc::setCurrentAlg  ( IAlgorithm* a ) 
{
  if ( 0 == a ) 
  {
    MsgStream log ( msgSvc() , name() ) ;
    log << MSG::WARNING << "IAlgorithm* points to NULL" << endreq ;
    //
    return StatusCode::RECOVERABLE ;                              // RETURN
  }  
  m_algorithms.push_back ( a ) ; 
  //
  return StatusCode::SUCCESS ;                                    // RETURN
}
// ============================================================================
// remove the algorithm                       ("pop_back") @see IAlgContextSvc
// ============================================================================
StatusCode AlgContextSvc::unSetCurrentAlg ( IAlgorithm* a ) 
{
  if ( 0 == a ) 
  {
    MsgStream log ( msgSvc() , name() ) ;
    log << MSG::WARNING << "IAlgorithm* points to NULL" << endreq ;
    //
    return StatusCode::RECOVERABLE ;                              // RETURN
  }  
  if ( m_algorithms.empty() || m_algorithms.back() != a ) 
  {
    MsgStream log ( msgSvc() , name() ) ;
    log << MSG::ERROR << "Algorithm stack is invalid" << endreq ;  
    //
    return StatusCode::FAILURE ;
  }
  //
  m_algorithms.pop_back() ;                                      // POP_BACK
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
/// accessor to current algorithm: @see IAlgContextSvc
// ============================================================================
IAlgorithm* AlgContextSvc::currentAlg  () const
{ return m_algorithms.empty() ? 0 : m_algorithms.back() ; }
// ============================================================================
// handle incident @see IIncidentListener
// ============================================================================
void AlgContextSvc::handle ( const Incident& )
{
  if ( !m_algorithms.empty() )
  {
    MsgStream log ( msgSvc() , name() ) ;
    log << MSG::ERROR
        << "Non-empty stack of algorihtms #"
        << m_algorithms.size() << endreq ;
  }
}
// ============================================================================




// ============================================================================
/// The END 
// ============================================================================
