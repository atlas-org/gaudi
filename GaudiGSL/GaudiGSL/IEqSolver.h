// =============================================================================
#ifndef GAUDIGSL_IEQSOLVER_H 
#define GAUDIGSL_IEQSOLVER_H 1
// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace Genfun    /// CLHEP
{
  class AbsFunction;
  class Argument;
}

/** @var IID_IEqSolver
 *  Declaration of the unique interface identifier
 *  (interface id, major version, minor version)
 */
static const InterfaceID IID_IEqSolver("IEqSolver", 1 , 0);

/** @class IEqSolver IEqSolver.h GaudiGSL/IEqSolver.h
 *  Interface file for multidimensional root-finding 
 *  (solving nonlinear system with n equations in n unknowns)
 *  @author Kirill Miklyaev kirillm@iris1.itep.ru
 *  @date   2003-07-07
 */

class IEqSolver : virtual public IAlgTool

{
  
public:
  /// Function (@see CLHEP/GenericFunctions/AbsFunction.hh)
  typedef Genfun::AbsFunction          GenFunc   ;
  /// Vector of the function "GenFunc" which we solver
  typedef std::vector<const GenFunc*>  Equations ;
  /// Argument of function "GenFunc" (@see CLHEP/GenericFunctions/Argument.hh)
  typedef Genfun::Argument             Arg       ;
 
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IEqSolver; }
  
  /// Do Multidimensional Root-finding
  
  /** Solving nonlinear system with N equations in N unknowns 
   *  of the function "GenFunc"
   *  @param equations   - vector of functions 
   *  @see CLHEP/GenericFunctions/AbsFunction.hh
   *  @param solution    - argument 
   *  @see CLHEP/GenericFunctions/Argument.hh
   *  @return StatusCode
   */
  virtual StatusCode solver( const Equations& equations ,
                             Arg&             solution  ) const = 0 ;

 
protected:

private:
};

#endif // GAUDIGSL_IEQSOLVER_H
