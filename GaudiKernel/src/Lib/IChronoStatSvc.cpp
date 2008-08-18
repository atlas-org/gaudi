// $Id: IChronoStatSvc.cpp,v 1.1 2007/08/06 08:39:39 marcocle Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IChronoSvc.h"
#include "GaudiKernel/IStatSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
// ============================================================================
/** @file
 *  Implementation file for interfaces IChronoSvc & IChronoStatSvc
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-07-08
 */
// ============================================================================
// unique interface identifier
// ============================================================================
const InterfaceID& IChronoSvc::interfaceID() 
{
  static const InterfaceID s_iid ( "IChronoSvc" , 1 , 0 ) ;
  return s_iid ;
}
// ============================================================================
// protected and virtual destructor 
// ============================================================================
IChronoSvc::~IChronoSvc(){}
// ============================================================================
// unique interface identifier
// ============================================================================
const InterfaceID& IStatSvc::interfaceID() 
{
  static const InterfaceID s_iid ( "IStatSvc" , 1 , 0 ) ;
  return s_iid ;
}
// ============================================================================
// protected and virtual destructor 
// ============================================================================
IStatSvc::~IStatSvc(){}
// ============================================================================
// unique interface identifier
// ============================================================================
const InterfaceID& IChronoStatSvc::interfaceID() 
{
  static const InterfaceID s_iid ( "IChronoStatSvc" , 5 , 0 ) ;
  return s_iid ;
}
// ============================================================================
// protected and virtual destructor 
// ============================================================================
IChronoStatSvc::~IChronoStatSvc(){}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
