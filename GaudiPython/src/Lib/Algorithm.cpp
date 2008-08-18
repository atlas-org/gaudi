// $Id: Algorithm.cpp,v 1.5 2006/12/19 16:36:26 hmd Exp $
// ============================================================================
// Incldue files 
// ============================================================================
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#include "Python.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Bootstrap.h"
// ============================================================================
// GaudiPython
// ============================================================================
#include "GaudiPython/Algorithm.h"
// ============================================================================


// ============================================================================
/// call the python method
// ============================================================================
StatusCode GaudiPython::call_python_method
( PyObject* self, char* method ) 
{
  StatusCode sc = StatusCode::FAILURE;
  // check arguments 
  if ( 0 == self || 0 == method ) { return StatusCode::FAILURE ; }
  
  // call Python 
  PyObject*  r  = PyObject_CallMethod ( self , method , "" ) ;
  
  if ( 0 == r ) { PyErr_Print() ;              return sc ; } // RETURN
  
  if ( PyInt_Check ( r ) ) 
  { sc = PyInt_AS_LONG( r ) ; Py_DECREF( r ) ; return sc ; } // RETURN 
  
  // look for the method getCode with the signature: 
  //  ' int getCode() '
  PyObject* c = PyObject_CallMethod( r  , "getCode" , "" ) ;   
  
  if      ( 0 == c           ) {      PyErr_Print()      ; }
  else if ( PyLong_Check( c )) { sc = PyLong_AsLong( c ); }
  else 
  {
    std::string msg( " call_python_method unexpected type from '" );
    msg += method ;
    msg += "().getCode()' " ;
    PyErr_SetString( PyExc_TypeError , msg.c_str() ) ;
    PyErr_Print() ; 
  }
  // release used objects 
  Py_XDECREF( c ) ;
  Py_XDECREF( r ) ;
  // 
  return sc;
} ;
// ============================================================================

// ============================================================================
/** constructor 
 *  @param self python objects
 *  @param name name of algorithm instance 
 */
// ============================================================================
GaudiPython::PyAlgorithm::PyAlgorithm
( PyObject*          self , 
  const std::string& name ) 
  : Algorithm ( name , Gaudi::svcLocator() ) 
  , m_self ( self ) 
{
  // The owner of the Algorithm is Python (as creator) therfore
  // it should not be deleted by Gaudi (added an extra addRef()). 
  addRef() ;
  addRef() ; 
};
// ============================================================================

// ============================================================================
StatusCode GaudiPython::PyAlgorithm::initialize () 
{ return GaudiPython::call_python_method ( m_self , "initialize" ) ; }
// ============================================================================
StatusCode GaudiPython::PyAlgorithm::execute    () 
{ return GaudiPython::call_python_method ( m_self , "execute"    ) ; }
// ============================================================================
StatusCode GaudiPython::PyAlgorithm::finalize   () 
{ return GaudiPython::call_python_method ( m_self , "finalize"   ) ; }

StatusCode GaudiPython::PyAlgorithm::beginRun() {
  return GaudiPython::call_python_method ( m_self , "beginRun" ) ; }
StatusCode GaudiPython::PyAlgorithm::endRun() {
  return GaudiPython::call_python_method ( m_self , "endRun" ) ; }

