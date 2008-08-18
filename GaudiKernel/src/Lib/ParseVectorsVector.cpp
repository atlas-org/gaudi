// $Id: ParseVectorsVector.cpp,v 1.3 2007/11/15 14:01:06 marcocle Exp $
// ============================================================================
// CVS tag $Name:  $, version $Revision: 1.3 $ 
// ============================================================================
// Include files
// ============================================================================
// Local
// ============================================================================
#include "Parsers.icpp"
// ============================================================================
/** @file 
 *
 *  Implementation of the parsing functions for std::vetctor<TYPE>
 *  
 *  For adding new type you should have only grammar class which recognize 
 *  this type. Then by combining this grammar with such already implemented 
 *  grammars  like VectorGrammar, MapGrammar, PairGrammar you can very simply
 *  implement container types which will work with your type.
 *
 *  If you want your type can be handled in property file - see ValueGrammar
 *  in JobOptionsSvc which lays in GaudiSvc.
 *
 *  @author Alexander MAZUROV  Alexander.Mazurov@gmail.com
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-05-12 
 */
// ============================================================================
namespace Gaudi
{ 
  namespace Parsers
  {
    // ========================================================================
    // vector< vector< TYPE > >
    // ========================================================================
    StatusCode parse 
    ( std::vector< std::vector<std::string> >& result , 
      const std::string&                       input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          VectorGrammar< VectorGrammar<StringGrammar> >()[var(result)=arg1],
          SkipperGrammar()).full ;
    }
    // ========================================================================
    StatusCode parse 
    ( std::vector< std::vector<double> >& result , 
      const std::string&                  input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          VectorGrammar< VectorGrammar< RealGrammar< double > > >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( std::vector< std::pair< double , double > >& result , 
      const string&                                input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          VectorGrammar<PairGrammar<RealGrammar<double>,RealGrammar<double> > >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( std::vector< std::pair< int , int > >& result , 
      const string&                          input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          VectorGrammar<PairGrammar<IntGrammar<int>,IntGrammar<int> > >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
  } // end of namespace Parsers
} // end of namespace Gaudi

// ============================================================================
// The END 
// ============================================================================


