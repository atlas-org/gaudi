// $Id: ICounterSvc.cpp,v 1.2 2007/08/06 08:39:39 marcocle Exp $
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.2 $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <vector>
#include <stdexcept>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/ICounterSvc.h"
// ============================================================================
// Boots 
// ============================================================================
#include "boost/format.hpp"
// ============================================================================
/** @file 
 *  Implementation file with helper methdos for interface ICounterSvc
 *  @date 2007-05-25 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// Retrieve the unuque interface ID
// ============================================================================
const InterfaceID& ICounterSvc::interfaceID() 
{ 
  static const InterfaceID s_IID_ICounterSvc ( "ICounterSvc" , 3 , 0 ) ; 
  return s_IID_ICounterSvc ; 
}
// ============================================================================
// virtual and protected destructor 
// ============================================================================
ICounterSvc::~ICounterSvc(){}
// ============================================================================
// Callback for printout with Counter pointers
// ============================================================================
StatusCode ICounterSvc::Printout::operator() 
  ( MsgStream& log, const Counter* cnt )  const  
{ return m_svc ? m_svc->defaultPrintout(log, cnt) : StatusCode::FAILURE ; } 
// ============================================================================
// Standard initializing constructor
// ============================================================================
ICounterSvc::Printout::Printout(ICounterSvc* svc)  
  : m_svc(svc)    
{ if ( 0 != m_svc ) { m_svc->addRef() ; }   } 
// ============================================================================
// destructor 
// ============================================================================
ICounterSvc::Printout::~Printout() 
{ if ( 0 != m_svc ) { m_svc->release() ; m_svc = 0 ; } }

// ============================================================================
// The END 
// ============================================================================
