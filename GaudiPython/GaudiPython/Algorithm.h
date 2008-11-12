// $Id: Algorithm.h,v 1.20 2008/10/28 10:40:19 marcocle Exp $
// ============================================================================
// ============================================================================
#ifndef GAUDIPYTHON_ALGORITHM_H
#define GAUDIPYTHON_ALGORITHM_H
// ============================================================================
// Include Files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Bootstrap.h"
// ============================================================================
// GaudiPython
// ============================================================================
#include "GaudiPython/GaudiPython.h"
// ============================================================================
// Python
// ============================================================================
#include "Python.h"
// ============================================================================

namespace GaudiPython
{
  /// call the python method
  StatusCode call_python_method ( PyObject* self , const char* method ) ;
};

namespace GaudiPython
{

  /** @class PyAlgorithm
   *  Python Algorithm base class
   */
  class PyAlgorithm : public Algorithm
  {
  public:
    /** constructor
     *  @param self python objects
     *  @param name name of algorithm instance
     */
    PyAlgorithm
    ( PyObject*          self ,
      const std::string& name ) ;
  public:
    StatusCode  initialize () ;
    StatusCode  start      () ;
    StatusCode  beginRun   () ;
    StatusCode  endRun     () ;
    StatusCode  execute    () ;
    StatusCode  stop       () ;
    StatusCode  finalize   () ;
    IAlgorithm* myself() { return this; }
  private:
    PyObject* m_self;
  };

  /** @class PyAlg
   *  general class to embedd the existing algorithm/base class
   *  into the python
   *  @author Vanya BELYAEV  Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-08-03
   */
  template <class ALGORITHM>
  class PyAlg :
    public          ALGORITHM ,
    public virtual IAlgorithm ,
    public virtual IProperty
  {
  public:
    /** constructor from Python object and the name
     *  @param self python object
     *  @param name name of algorithm instance
     */
    PyAlg
    ( PyObject*          self ,
      const std::string& name )
      : ALGORITHM ( name , Gaudi::svcLocator() )
      , m_self ( self )
    {
      // the printout of actual type for embedded algorithm has no sence
      setProperty ( "TypePrint" , "false" ) ;
      setProperty ( "StatPrint" , "true"  ) ;

      // The owner of the Algorithm is Python (as creator) therfore
      // it should not be deleted by Gaudi (added an extra addRef()).
      addRef() ;
      addRef() ;
    }
  protected:
    /// virtual destructor
    virtual ~PyAlg() {}
    PyObject* _obj() const { return m_self ; }
  public:

    virtual StatusCode initialize  ()
    { return GaudiPython::call_python_method ( m_self , "initialize" ) ; }

    virtual StatusCode start  ()
    { return GaudiPython::call_python_method ( m_self , "start" ) ; }

    virtual StatusCode beginRun  ()
    { return GaudiPython::call_python_method ( m_self , "beginRun" ) ; }
    virtual StatusCode endRun  ()
    { return GaudiPython::call_python_method ( m_self , "endRun" ) ; }

    virtual StatusCode execute     ()
    { return GaudiPython::call_python_method ( m_self , "execute"    ) ; }

    virtual StatusCode stop  ()
    { return GaudiPython::call_python_method ( m_self , "stop" ) ; }
    virtual StatusCode finalize    ()
    { return GaudiPython::call_python_method ( m_self , "finalize"   ) ; }

    virtual IAlgorithm* ialgorithm () { return this ; }
    virtual IProperty*  iproperty  () { return this ; }

    // preserve the existing methods
    virtual StatusCode initialize_ () { return ALGORITHM::initialize () ; }
    virtual StatusCode finalize_   () { return ALGORITHM::finalize   () ; }

  private:
    // the default constructor is desabled
    PyAlg() ;
    // the copy constructor is desabled
    PyAlg ( const PyAlg& );
    // the assignement operator is desabled
    PyAlg& operator=( const PyAlg& ) ;
  private:
    // "shadow" python class
    PyObject* m_self;
  } ;

} // namespace GaudiPython

// ============================================================================
// The END
// ============================================================================
#endif // GAUDIPYTHON_ALGORITHM_H
// ============================================================================
