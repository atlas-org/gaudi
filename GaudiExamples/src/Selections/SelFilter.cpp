// $Id: SelFilter.cpp,v 1.1 2008/10/10 08:06:33 marcocle Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Local 
// ============================================================================
#include "MyTrack.h"
// ============================================================================
// Boost
// ============================================================================
#ifdef __ICC
// disable icc remark #177: declared but never referenced
// Problem with boost::lambda
#pragma warning(disable:177)
#endif
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#ifdef __ICC
// re-enable icc remark #177: declared but never referenced
#pragma warning(enable:177)
#endif
// ============================================================================
namespace bl = boost::lambda ;
// ============================================================================
namespace Gaudi
{
  namespace Examples 
  {
    /** @class SelFilter
     *  Simple class to create few "containers" in TES 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-23
     */
    class SelFilter : public GaudiAlgorithm
    {
      // ======================================================================
      /// friend factory for instantiation 
      friend class AlgFactory<Gaudi::Examples::SelFilter> ;
      // ======================================================================
    public:
      // ======================================================================
      /// the only one essential method 
      virtual StatusCode execute() 
      {

        typedef Gaudi::NamedRange_<Gaudi::Examples::MyTrack::ConstVector>     Range ;
        
        static Rndm::Numbers  flat    ( randSvc () , Rndm::Flat    ( -1 , 1 ) ) ;
        
        if      ( exist<Gaudi::Examples::MyTrack::Selection> ( m_input ) ) 
        { info() << "Selection at '" << m_input << "'" << endmsg ; }
        else if ( exist<Gaudi::Examples::MyTrack::Container> ( m_input ) ) 
        { info() << "Container at '" << m_input << "'" << endmsg ; }
        
        // get input data in 'blind' way 
        Range range = get<Range> ( m_input ) ;
        
        // create new selection 
        Gaudi::Examples::MyTrack::Selection* sample = 
          new Gaudi::Examples::MyTrack::Selection()  ;
        
        const double pxCut =     flat () ;
        const double pyCut =     flat () ;
        
        // select particles with 'large' px 
        sample -> insert 
          ( range.begin () , 
            range.end   () ,
            bl::bind( &Gaudi::Examples::MyTrack::px,bl::_1) > pxCut ) ;

        const size_t size = sample -> size() ;
        
        // remove the particles with 'small' py 
        sample -> erase
          ( bl::bind( &Gaudi::Examples::MyTrack::py,bl::_1) < pyCut ) ;
        
        info () << "Sample size is " 
                << range.size() 
                << "/" << size 
                << "/" << sample -> size()  << endmsg ;
        
        // register it in TES 
        put ( sample , name() ) ;
        
        return StatusCode::SUCCESS ; 
      }
      // ======================================================================
    protected:
      // ======================================================================
      /** standard constructor 
       *  @param name the algorithm instance name 
       *  @param pSvc pointer to Service Locator
       */
      SelFilter ( const std::string& name ,   //    the algorithm instance name 
                  ISvcLocator*       pSvc )   // pointer to the Service Locator
        : GaudiAlgorithm ( name , pSvc )
        , m_input ()
      {
        declareProperty 
          ( "Input" , m_input , 
            "TES location of input container" ) ;
      }
      /// virtual (and protected) destructor 
      virtual ~SelFilter() {}
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SelFilter () ;                                  // no default constructor  
      /// copy constructor is disabled 
      SelFilter ( const SelFilter& ) ;                   // no copy constructor 
      /// assignement operator is disabled
      SelFilter& operator=( const SelFilter& ) ;     // no assignement operator 
      // ======================================================================
    private:
      // ======================================================================
      /// the input  location 
      std::string m_input  ;                             // the input  location 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Gaudi::Examples 
} // end of namespace Gaudi
// ============================================================================
/// The factory (needed for instantiation)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Gaudi::Examples,SelFilter)
// ============================================================================
// The END
// ============================================================================
  
