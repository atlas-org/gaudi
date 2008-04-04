// $Id: Adapters.h,v 1.2 2005/11/25 10:27:03 mato Exp $
// ============================================================================
#ifndef GAUDIMATH_ADAPTERS_H 
#define GAUDIMATH_ADAPTERS_H 1
// ============================================================================
// Include files
// ============================================================================
// Local
// ============================================================================
#include "GaudiMath/FunAdapters.h"
#include "GaudiMath/GSLFunAdapters.h"
// ============================================================================


namespace GaudiMath
{  
  typedef Genfun::GaudiMathImplementation::AdapterIFunction
  AIDAFunction   ;
  typedef Genfun::GaudiMathImplementation::Adapter2DoubleFunction
  Function2D     ;
  typedef Genfun::GaudiMathImplementation::Adapter3DoubleFunction
  Function3D     ;
  typedef Genfun::GaudiMathImplementation::SimpleFunction
  SimpleFunction ;
  
  typedef Genfun::GaudiMathImplementation::GSLFunctionWithMode 
  GSLFunctionWithMode ;
  typedef Genfun::GaudiMathImplementation::GSLFunctionWithError
  GSLFunctionWithError ;
  typedef Genfun::GaudiMathImplementation::GSLFunctionWithModeAndError 
  GSLFunctionWithModeAndError ;
  
  AIDAFunction                adapter
  ( const      AIDA::IFunction&           function ) ;
  Function2D                  adapter 
  (       Function2D::Function            function ) ;
  Function3D                  adapter 
  (       Function3D::Function            function ) ;
  SimpleFunction              adapter 
  (   SimpleFunction::Function1           function ) ;
  SimpleFunction              adapter 
  ( const size_t                          dim      , 
      SimpleFunction::Function2           function ) ;
  SimpleFunction              adapter 
  ( const size_t                          dim      , 
      SimpleFunction::Function3           function ) ;
  GSLFunctionWithMode         adapter 
  ( GSLFunctionWithMode::Function         function , 
    const gsl_mode_t&                     mode     ) ;
  GSLFunctionWithError        adapter 
  ( GSLFunctionWithError::Function        function ) ;
  GSLFunctionWithModeAndError adapter 
  ( GSLFunctionWithModeAndError::Function function , 
    const gsl_mode_t&                     mode     ) ;
  
}; // end of namespace GaudiMath

#endif // GAUDIMATH_ADAPTERS_H
// ============================================================================
