// $Id: GslErrorCount.cpp,v 1.2 2006/01/10 20:00:05 hmd Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Stat.h"
// local
#include "GslErrorCount.h"

// ============================================================================
/** @file 
 * 
 *  Implementation file for class GslErrorCount
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/04/2002
 */
// ============================================================================

// ============================================================================
/** @var GslErrorCountFactory
 *  Declaration of the Tool Factory
 *  @see  ToolFactory
 *  @see IToolFactory
 *  @see     IFactory
 */
// ============================================================================
DECLARE_TOOL_FACTORY(GslErrorCount)
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type   tool type (?)
 *  @param name   tool name
 *  @param parent pointer to parent 
 */
// ============================================================================
GslErrorCount::GslErrorCount
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : AlgTool ( type, name , parent ) 
  , m_counters ()
{ declareInterface<IGslErrorHandler> (this);};
// ============================================================================

// ============================================================================
/// destructor (protetced and virtual)
// ============================================================================
GslErrorCount::~GslErrorCount(){};
// ============================================================================

// ============================================================================
/** standard finalization of Tool
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode GslErrorCount::finalize   () 
{
  // printout the Error table 
  MsgStream log( msgSvc() , name() );
  const std::string stars( 78 , '*' );
  log << MSG::INFO  << stars << endreq ;
  log << MSG::ERROR <<  m_counters.size() << " GSL errors handled" << endreq ; 
  for( Counters::const_iterator error = m_counters.begin() ;
       error != m_counters.end() ; ++error ) 
    {
      log << MSG::ERROR
          << " #times "   << error->second 
          << " GSL code " << error->first.code 
          << " Message '" << error->first.reason << "'" 
          << " File '"    << error->first.file   << "'"
          << " Line "     << error->first.line   << endreq ;  
    }
  log << MSG::INFO << stars << endreq ;      
  // clear the counters 
  m_counters.clear();
  // finalze the base class 
  return AlgTool::finalize ();
};
// ============================================================================

// ============================================================================
/** handle the GSL error 
 *  @see IGslErrorHandler
 *  @param error  error to be handled 
 *  @see GslError
 *  @return status code 
 */
// ============================================================================
StatusCode GslErrorCount::handle 
( const GslError& error  ) const 
{
  // increase the counter 
  m_counters[ error ] += 1 ;
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
