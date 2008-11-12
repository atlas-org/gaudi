// $Id: AlgDecorators.h,v 1.2 2005/11/08 16:31:03 mato Exp $
// ============================================================================
#ifndef GAUDIPYTHON_ALGDECORATORS_H 
#define GAUDIPYTHON_ALGDECORATORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include  <string>
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// GaudiPython
// ============================================================================
#include "GaudiPython/GaudiPython.h"
// ============================================================================
// forward declarations 
// ============================================================================
class IInterface     ;
class IAlgTool       ;
class GaudiAlgorithm ;
// ============================================================================

/** @file
 *  
 *  Collection of "decorators" for python algorithms
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr 
 *  @date   2005-08-03
 */

namespace GaudiPython
{
  
  /** @class Decorator
   *  simple class with allows to "decorate" the python 
   *  algorithm with 'basic' funtionality from class GaudiAlgorithm
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2007-08-03
   */
  class AlgDecorator 
  {
  public:
    /** get the tool from GaudiAlgorithm
     *  @param alg GaudiAlgorithm
     *  @param type tool type
     *  @param name tool name 
     *  @param parent tool parent 
     *  @param create flag to create 
     *  @return the tool 
     */
    static IAlgTool*   tool_ 
    ( const GaudiAlgorithm* alg            ,
      const std::string&    type           , 
      const std::string&    name           , 
      const IInterface*     parent = 0     , 
      const bool            create = true  ) ;
    
    /** get the tool from GaudiAlgorithm
     *  @param alg GaudiAlgorithm
     *  @param typeAndName tool type/name 
     *  @param parent tool parent 
     *  @param create flag to create 
     *  @return the tool 
     */
    static IAlgTool*   tool_ 
    ( const GaudiAlgorithm* alg            ,
      const std::string&    typeAndName    , 
      const IInterface*     parent  = 0    , 
      const bool            create  = true ) ;
    
    /** get the service from GaudiAlgorithm
     *  @param alg GaudiAlgorithm
     *  @param name service name 
     *  @param create flag to create 
     *  @return the tool 
     */
    static IInterface* svc_  
    ( const GaudiAlgorithm* alg            ,
      const std::string&    name           ,
      const bool            create = false ) ;
    
  };
  
  
}; // end of namespace GaudiPython
  
#endif // GAUDIPYTHON_ALGDECORATORS_H
