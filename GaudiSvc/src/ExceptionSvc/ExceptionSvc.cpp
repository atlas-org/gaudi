// $Id: ExceptionSvc.cpp,v 1.6 2007/05/24 14:41:22 hmd Exp $ 
// ============================================================================
// CVS tag $Name:  $ , version $Revision: 1.6 $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <cassert>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
//Local 
// ============================================================================
#include "ExceptionSvc.h"
// ============================================================================


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DECLARE_SERVICE_FACTORY(ExceptionSvc)

//
///////////////////////////////////////////////////////////////////////////
//

ExceptionSvc::ExceptionSvc( const std::string& name, ISvcLocator* svc )
  : Service( name, svc )
  , m_mode_s ( "all" ) 
  , m_algs_v (       )
{
  declareProperty( "Catch"      , m_mode_s ) ;
  declareProperty( "Algorithms" , m_algs_v ) ;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

ExceptionSvc::~ExceptionSvc() {

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
ExceptionSvc::initialize() {
  MsgStream log( msgSvc(), name() );

  StatusCode status = Service::initialize();
  
  if ( status.isFailure() )  { return status ; }                  // RETURN 
  
  if (m_mode_s == "all" || m_mode_s == "All" || m_mode_s == "ALL") {
    m_mode = ALL;
  } else if (m_mode_s == "none" || m_mode_s == "None" || 
             m_mode_s == "NONE") {
    m_mode = NONE;
  } else if (m_mode_s == "list" || m_mode_s == "List" || 
             m_mode_s == "LIST") {
    m_mode = LIST;
  } else {
    log << MSG::ERROR << "Unknown value for property \"State\". Must be"
	<< " one of \"all\", \"none\", or \"list\"" << endreq;
    m_state = OFFLINE;
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::const_iterator itr;
  for (itr = m_algs_v.value().begin(); itr != m_algs_v.value().end(); ++itr) {
    std::string a = *itr;
    std::string alg(a);
    ReturnState ret(RETHROW);

    std::string::size_type ieq = a.find("=");
    if (ieq != std::string::npos) {
      alg = a;
      alg.erase(ieq,alg.length());
      a.erase(0,ieq+1);

      if ( a == "DEFAULT" ) {
	ret = DEFAULT;
      } else if ( a == "SUCCESS" ) {
	log << MSG::WARNING << "Unusual custom error return code SUCCESS for"
	    << " Algorithm \"" << alg << "\"" <<endreq;
	ret = SUCCESS;
      } else if ( a == "FAILURE" ) {
	ret = FAILURE;
      } else if ( a == "RECOVERABLE" ) {
	ret = RECOVERABLE;
      } else if ( a == "RETHROW" ) {
	ret = RETHROW;
      } else {
	log << MSG::ERROR << "In JobOpts: unknown return code \"" << a 
	    << "\" for Algorithm " << alg << std::endl
	    << "Must be one of: DEFAULT, SUCCESS, FAILURE, RECOVERABLE, RETHROW"
	    << endreq;
	m_state = OFFLINE;
	return StatusCode::FAILURE;
      }
    }

    m_algs.insert( alg );
    m_retCodes[alg] = ret;
  }

  if (m_algs.size() > 0 && m_mode != NONE) {
    m_mode = LIST;

    log << MSG::DEBUG << "Will catch exceptions thrown by: " << std::endl;
    std::set<std::string>::const_iterator it;
    for (it = m_algs.begin(); it != m_algs.end(); ++it) {
      log << "            " << *it << "  action: ";

      ReturnState ret = m_retCodes.find(*it)->second;

      switch( ret ) {
      case( DEFAULT ):
	log << "DEFAULT";
	break;
      case( SUCCESS ):
	log << "SUCCESS";
	break;
      case( FAILURE ):
	log << "FAILURE";
	break;
      case( RECOVERABLE ):
	log << "RECOVERABLE";
	break;
      case( RETHROW ):
	log << "RETHROW";
	break;
      }

      log << std::endl;
    }
    log << endreq;

  }

  return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
ExceptionSvc::finalize() {
  MsgStream log( msgSvc(), name() );

  StatusCode status = Service::finalize();

  return status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
ExceptionSvc::queryInterface 
( const InterfaceID&  iid  , 
  void**              ppvi ) 
{
  // invalid placeholder 
  if ( 0 == ppvi ) { return StatusCode::FAILURE   ; }   // RETURN
  // check the identidier 
  if ( IExceptionSvc::interfaceID().versionMatch( iid ) ) 
  { *ppvi = static_cast<IExceptionSvc*>  ( this ) ; }  // OK ! 
  else  // ask for base class 
  { return Service::queryInterface ( iid , ppvi ) ; }  // RETURN 
  // increment the reference counter 
  addRef() ;
  //
  return StatusCode::SUCCESS ;                        // RETURN 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ExceptionSvc::handleErr 
( const INamedInterface& alg , 
  const StatusCode&      st  ) const 
{
  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << "Handling Error " << alg.name() << endreq;
  
  // Don't do anything
  if (m_mode == NONE) {
    return st;
  }
  
  if (m_mode == ALL) {
    return StatusCode::FAILURE;
  }
  
  assert (m_mode == LIST);
  
  // Not one of the requested algs
  if (m_algs.find(alg.name()) == m_algs.end()) {
    return st;
  }
  
  // Requested to do something with this alg
  ReturnState iret = m_retCodes.find(alg.name())->second;
  
  switch ( iret ) {
  case DEFAULT:
    // there is no default
    return st;
  case SUCCESS:
    return StatusCode::SUCCESS;
  case FAILURE:
    return StatusCode::FAILURE;
  case RECOVERABLE:
    return StatusCode::RECOVERABLE;
  case RETHROW:
    return st;
  }
  
  return st;
  
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ExceptionSvc::handle 
( const INamedInterface& alg ) const
{
  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << "Handling unknown exception for " << alg.name() << endreq;
  
  // Don't do anything
  if (m_mode == NONE) {
    return StatusCode::FAILURE;
  }

  if (m_mode == ALL) {
    throw;
  }

  assert (m_mode == LIST);

  // Not one of the requested algs
  if (m_algs.find(alg.name()) == m_algs.end()) {
    return StatusCode::FAILURE;
  }

  // Requested to do something with this alg
  ReturnState iret = m_retCodes.find(alg.name())->second;
  
  switch ( iret ) {
  case DEFAULT:
    // there is no default
    return StatusCode::FAILURE;
  case SUCCESS:
    return StatusCode::SUCCESS;
  case FAILURE:
    return StatusCode::FAILURE;
  case RECOVERABLE:
    return StatusCode::RECOVERABLE;
  case RETHROW:
    throw;    
  }

  return StatusCode::FAILURE;

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ExceptionSvc::handle
( const INamedInterface& alg , 
  const std::exception & exc ) const
{
  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << "Handling std:except for " << alg.name() << endreq;
  
  // Don't do anything
  if (m_mode == NONE) {
    return StatusCode::FAILURE;
  }
  
  if (m_mode == ALL) {
    throw ( exc );
  }

  assert (m_mode == LIST);

  // Not one of the requested algs
  if (m_algs.find(alg.name()) == m_algs.end()) {
    return StatusCode::FAILURE;
  }

  // Requested to do something with this alg
  ReturnState iret = m_retCodes.find(alg.name())->second;
  
  switch ( iret ) {
  case DEFAULT:
    // there is no default
    return StatusCode::FAILURE;
    break;
  case SUCCESS:
    return StatusCode::SUCCESS;
    break;
  case FAILURE:
    return StatusCode::FAILURE;
    break;
  case RECOVERABLE:
    return StatusCode::RECOVERABLE;
    break;
  case RETHROW:
    throw ( exc );    
  }

  return StatusCode::FAILURE;

}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ExceptionSvc::handle 
( const INamedInterface& alg , 
  const GaudiException & exc ) const 
{
  
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Handling GaudiExcept for " << alg.name() << endreq;
  
  // Don't do anything
  if (m_mode == NONE) {
    return StatusCode::FAILURE;
  }
  
  // rethrow
  if (m_mode == ALL) {
    throw (exc);
    return StatusCode::FAILURE;
  }
  
  assert(m_mode == LIST);
  
  //  Not one of the requested algs
  if (m_algs.find(alg.name()) == m_algs.end()) {
    return StatusCode::FAILURE;
  }
  
  // Requested to do something with this alg
  ReturnState iret = m_retCodes.find(alg.name())->second;

  switch ( iret ) {
  case DEFAULT:
    return exc.code();
  case SUCCESS:
    return StatusCode::SUCCESS;
  case FAILURE:
    return StatusCode::FAILURE;
  case RECOVERABLE:
    return StatusCode::RECOVERABLE;
  case RETHROW:
    throw ( exc );    
  }

  return StatusCode::FAILURE;

}

// ============================================================================
// The END 
// ============================================================================
