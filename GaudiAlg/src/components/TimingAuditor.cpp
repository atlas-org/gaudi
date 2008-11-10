// $Id: TimingAuditor.cpp,v 1.5 2008/04/05 08:04:22 marcocle Exp $
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.5 $ 
// ============================================================================
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/AudFactory.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/ISequencerTimerTool.h"
// ============================================================================
/** @class TimingAuditor
 *
 *  Simple auditor which uses SequencerTimerTool for *ALL* 
 *  algoritms, inclusing the algorithm from main Gaudi "TopAlg" list
 *  and algorithms managed by Data-On-Demand Service 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-01-31
 */
class TimingAuditor
  : public virtual IIncidentListener 
  , public                   Auditor
{
public:
  
  virtual void before(StandardEventType evt, INamedInterface* alg);
  virtual void after(StandardEventType evt, INamedInterface* alg, const StatusCode &sc);

  virtual void before(CustomEventTypeRef evt, const std::string& name);
  virtual void after(CustomEventTypeRef evt, const std::string& name, const StatusCode &sc);

private:
  void i_beforeInitialize( INamedInterface* alg );
  void i_afterInitialize( INamedInterface* alg );
  void i_beforeExecute( INamedInterface* alg );
  void i_afterExecute( INamedInterface* alg);
  
public:
  /// Inform that a new incident has occured
  virtual void handle ( const Incident& ) ;
  
public:
  virtual StatusCode initialize () ;
  virtual StatusCode finalize   () ;
  virtual StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvInterface ) ;
public:
  /// standard constructor
  TimingAuditor 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : Auditor ( name , pSvc ) 
    // 
    , m_toolSvc    ( 0 ) 
    , m_incSvc     ( 0 )
    //
    , m_timer      ( 0 ) 
    // 
    , m_appMgr     ( 0 ) 
    //
    , m_map        (       ) 
    , m_indent     ( 0     )
    , m_inEvent    ( false )
    , m_goodForDOD ( false )
    , m_mapUser    (       ) 
  {
    declareProperty ( "OptimizedForDOD" , m_goodForDOD ) ;
  } ;
  /// virtual destructor
  virtual ~TimingAuditor() {}
private:
  // the default constructor is disabled  
  TimingAuditor () ;
  // copy constructor is disabled  
  TimingAuditor           ( const TimingAuditor& ) ;
  // assignement operator is disabled  
  TimingAuditor& operator=( const TimingAuditor& ) ;  
private:
  // tool service 
  IToolSvc*            m_toolSvc ; ///< tool service 
  // incident service 
  IIncidentSvc*        m_incSvc  ; ///< incident service 
  // the timer tool 
  ISequencerTimerTool* m_timer   ; ///< the timer tool 
  // ApplicationManager 
  INamedInterface*     m_appMgr  ; ///< ApplicationManager 
  // 
  typedef GaudiUtils::VectorMap<const INamedInterface*,int>  Map ;
  Map                  m_map     ;
  // indentation level 
  int                  m_indent  ; ///< indentation level 
  // "in event"
  bool                 m_inEvent ; ///< "In event" flag
  // "optimized for Data-On-Demand Service"
  bool                 m_goodForDOD ; ///< "optimized for DOD"
  //
  typedef GaudiUtils::HashMap<std::string,int> MapUser ;
  MapUser              m_mapUser ; ///< map used to record user timing events
  
} ;
// ============================================================================
/// factory:
// ============================================================================
DECLARE_AUDITOR_FACTORY(TimingAuditor) ;
// ============================================================================
StatusCode TimingAuditor::queryInterface
( const InterfaceID& riid,
  void** ppv ) 
{
  if      ( 0 == ppv ) { return StatusCode::FAILURE ; }  // RETURN 
  else if ( IIncidentListener::interfaceID() == riid ) 
  { *ppv = static_cast<IIncidentListener*> ( this ) ; }
  else 
  { return Auditor::queryInterface( riid , ppv ) ; } // RETURN 
  addRef() ;
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
StatusCode TimingAuditor::initialize () 
{
  StatusCode sc = Auditor::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                  // RETURN 
  
  MsgStream log ( msgSvc() , name() ) ;
  
  // get tool service 
  if ( 0 == m_toolSvc ) 
  {
    StatusCode sc = Auditor::service ( "ToolSvc" , m_toolSvc ) ;
    if ( sc.isFailure() ) 
    {
      log << "Could not retrieve 'ToolSvc' " << sc << endreq ;
      return sc ;                                        // RETURN 
    }
    if ( 0 == m_timer ) 
    {
      sc = m_toolSvc->retrieveTool 
        ( "SequencerTimerTool/TIMER" , m_timer , this  , true ) ;
      if ( sc.isFailure() ) 
      {
        log << MSG::ERROR 
            << "Could not retrieve ISequencerTimerTool" << endreq ;
        return sc ;
      }
    }    
  }
  // get incident service
  if ( 0 == m_incSvc ) 
  { 
    StatusCode sc = Auditor::service ( "IncidentSvc" , m_incSvc ) ;
    if ( sc.isFailure() ) 
    { 
      log << MSG::ERROR 
          << "Could not retrieve 'IncidentSvc'" << sc << endreq ;
      return sc ;
    }
    m_incSvc -> addListener ( this , IncidentType::BeginEvent ) ;
    m_incSvc -> addListener ( this , IncidentType::EndEvent   ) ;
  }  
  // get the application manager 
  if ( 0 == m_appMgr ) 
  {
    StatusCode sc = Auditor::service ( "ApplicationMgr" , m_appMgr ) ;
    if ( sc.isFailure() )
    { 
      log << MSG::ERROR 
          << "Could not retrieve 'ApplicationMgr'" << sc << endreq ;
      return sc ;
    }
    if ( m_map.end() == m_map.find( m_appMgr ) ) 
    {
      int timer = m_timer->addTimer( "EVENT LOOP" ) ;
      m_map.insert ( m_appMgr , timer ) ;
    }
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
StatusCode TimingAuditor::finalize   () 
{
  if ( 0 != m_incSvc ) 
  { 
    m_incSvc -> removeListener ( this , IncidentType::BeginEvent ) ;
    m_incSvc -> removeListener ( this , IncidentType::EndEvent   ) ; 
    m_incSvc -> release () ;
    m_incSvc = 0 ;
  }
  if ( 0 != m_toolSvc ) 
  {
    // the 2 following line are commented out: it is 
    // is a temporary hack which prevent a crash due to a problem in
    // the reference counting
    //     if ( 0 != m_timer ) 
    //     { m_toolSvc -> releaseTool ( m_timer ) . ignore() ; m_timer = 0 ; }
    m_toolSvc -> release () ;
    m_toolSvc = 0 ;
  }
  if ( 0 != m_appMgr ) { m_appMgr -> release () ;  m_appMgr = 0 ; }
  // clear the map 
  m_map.clear() ;  
  // finalize the base class 
  return Auditor::finalize () ;
}
// ============================================================================
void TimingAuditor::before(StandardEventType evt, INamedInterface *alg)
{
  switch (evt) {
  case IAuditor::Initialize : i_beforeInitialize( alg ); break;
  case IAuditor::Execute    : i_beforeExecute( alg );    break;
  default: break;
  }
}
// ============================================================================
void TimingAuditor::after(StandardEventType evt, INamedInterface *alg, const StatusCode &)
{
  switch (evt) {
  case IAuditor::Initialize : i_afterInitialize( alg ); break;
  case IAuditor::Execute    : i_afterExecute( alg ); break;
  default: break;
  }
}
// ============================================================================
void TimingAuditor::i_beforeInitialize( INamedInterface* alg ) 
{
  if ( m_goodForDOD ) { return ; }
  //
  if ( 0 == alg ) { return ; }
  Map::iterator found = m_map.find( alg ) ;
  if ( m_map.end() != found ) { return ; }
  ++m_indent ;
  std::string nick = alg->name() ;
  if ( 0 < m_indent ) { nick = std::string ( m_indent , ' ') + nick ; }
  if ( m_inEvent ) 
  { 
    nick[0] = '*' ;
    MsgStream log( msgSvc() , name() ) ;
    log << MSG::DEBUG 
        << "Insert non-structural component '"
        << alg->name() << "' of type '"
        << System::typeinfoName(typeid(*alg)) << "' at level "
        << m_indent << endreq ;
  }
  int timer = m_timer->addTimer( nick ) ;
  m_map.insert ( alg , timer ) ;
  m_timer->start( timer ) ;
}
// ============================================================================
void TimingAuditor::i_afterInitialize( INamedInterface* alg ) 
{
  if ( m_goodForDOD ) { return ; }
  if ( 0 == alg     ) { return ; }
  --m_indent ;
}
// ============================================================================
void TimingAuditor::i_beforeExecute( INamedInterface* alg ) 
{
  if ( 0 == alg ) { return ; }
  ++m_indent ;
  Map::iterator found = m_map.find( alg ) ;
  if ( m_map.end() == found ) 
  {
    MsgStream log( msgSvc() , name() ) ;
    log << MSG::DEBUG 
        << "Insert non-structural component '"
        << alg->name() << "' of type '"
        << System::typeinfoName(typeid(*alg)) << "' at level "
        << m_indent << endreq ;
    std::string nick = alg->name() ;
    if ( 0 < m_indent  ) { nick = std::string ( m_indent , ' ') + nick ; }
    if ( !m_goodForDOD ) { nick[0]='*' ;}
    int timer = m_timer->addTimer( nick ) ;
    m_map.insert ( alg , timer ) ;
    m_timer->start( timer ) ;
    return ;
  }
  m_timer->start( found->second ) ; 
}
// ============================================================================
void TimingAuditor::i_afterExecute( INamedInterface* alg ) 
{
  if ( 0 == alg ) { return ; }
  Map::iterator found = m_map.find( alg ) ;
  if ( m_map.end() == found ) { return ; }
  m_timer->stop( found->second ) ;
  --m_indent ;
}
// ============================================================================
void TimingAuditor::before(CustomEventTypeRef evt, const std::string& name) 
{
  // Ignore obvious mistakes
  if ( name.empty() && evt.empty() ) { return; }
  
  // look for the user timer in the map
  int timer = 0;
  std::string nick = name + ":" + evt;
  MapUser::iterator found = m_mapUser.find( nick );
  
  if ( m_mapUser.end() == found ) {
    // add a new timer if not yet available
    timer = m_timer->addTimer( nick ) ;
    m_mapUser[nick] = timer;
  }
  else {
    timer = found->second;
  }
  
  m_timer->start( timer );
}
// ============================================================================
void TimingAuditor::after(CustomEventTypeRef evt, const std::string& name, const StatusCode &) 
{
  // Ignore obvious mistakes
  if ( name.empty() && evt.empty() ) { return; }

  // look for the user timer in the map
  std::string nick = name + ":" + evt;
  MapUser::iterator found = m_mapUser.find( nick );
  
  // We cannot do much if the timer is not available
  if ( m_mapUser.end() == found ) {
    MsgStream log(msgSvc(), this->name());
    log << MSG::WARNING << "Trying to stop the measure  of the timing for '"
                        << nick << "' but it was never started. Check the code"
                        << endmsg;
    return;
  }
  m_timer->stop( found->second );
}
// ============================================================================
void TimingAuditor::handle ( const Incident& i ) 
{
  if      ( IncidentType::BeginEvent == i.type () ) 
  { 
    m_timer -> start ( m_map[ m_appMgr ] ) ; 
    ++m_indent ;
    m_inEvent = true ;
  }
  else if ( IncidentType::EndEvent   == i.type () ) 
  { 
    m_timer -> stop  ( m_map[ m_appMgr ] ) ; 
    --m_indent ;
    m_inEvent = false ;
  } 
}
// ============================================================================
// The END 
// ============================================================================
