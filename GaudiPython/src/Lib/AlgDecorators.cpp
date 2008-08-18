// $Id: AlgDecorators.cpp,v 1.2 2005/11/08 16:31:03 mato Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// GaudiPython
// ============================================================================
#include "GaudiPython/AlgDecorators.h"
// ============================================================================

/** @file
 *  Implementation file for "Decorators"
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-08-03 
 */

// ============================================================================
/** get the tool from GaudiAlgorithm
 *  @param alg GaudiAlgorithm
 *  @param type tool type
 *  @param name tool name 
 *  @param parent tool parent 
 *  @param create flag to create 
 *  @return the tool 
 */
// ============================================================================
IAlgTool* GaudiPython::AlgDecorator::tool_
( const GaudiAlgorithm* alg    ,
  const std::string&    type   , 
  const std::string&    name   , 
  const IInterface*     parent , 
  const bool            create ) 
{
  if ( 0 == alg ) { return 0 ; }
  return alg -> tool<IAlgTool> ( type , name , parent , create ) ;
};
// ============================================================================

// ============================================================================
/** get the tool from GaudiAlgorithm
 *  @param alg GaudiAlgorithm
 *  @param typeAndName tool type/name 
 *  @param parent tool parent 
 *  @param create flag to create 
 *  @return the tool 
 */
// ============================================================================
IAlgTool* GaudiPython::AlgDecorator::tool_
( const GaudiAlgorithm* alg         ,
  const std::string&    typeAndName , 
  const IInterface*     parent      , 
  const bool            create      ) 
{
  if ( 0 == alg ) { return 0 ; }
  return alg->tool<IAlgTool>( typeAndName , parent , create ) ;
};
// ============================================================================

// ============================================================================
/** get the service from GaudiAlgorithm
 *  @param alg GaudiAlgorithm
 *  @param name service name 
 *  @param create flag to create 
 *  @return the tool 
 */
// ============================================================================
IInterface* GaudiPython::AlgDecorator::svc_
( const GaudiAlgorithm* alg    ,
  const std::string&    name   ,
  const bool            create ) 
{
  if ( 0 == alg ) { return 0 ; }
  return alg -> svc<IInterface> ( name , create ) ;
};
// ============================================================================
