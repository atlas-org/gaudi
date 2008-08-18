// $Id: ParseMaps.cpp,v 1.4 2008/01/14 19:42:56 marcocle Exp $
// ============================================================================
// Include files
// ============================================================================
// Local
// ============================================================================
#include "Parsers.icpp"
// ============================================================================
/** @file 
 *
 *  Implementation of the parsing functions for std::map<TYPE1,TYPE2>
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
    // map< TYPE, TYPE >
    // ========================================================================
    StatusCode parse
    ( map< int , int >& result , 
      const string&     input  )
    {
      return parse
        ( createIterator(input)  , 
          IteratorT()            ,
          MapGrammar< IntGrammar<int> , IntGrammar<int> >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map< int , double >& result , 
      const string&        input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar< IntGrammar<int> , RealGrammar<double> >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map<string,double>&  result , 
      const string&        input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<StringGrammar,RealGrammar<double> >()[var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map< string , string >& result , 
      const string&           input  ) 
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<StringGrammar,StringGrammar>()[var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map< string , int >& result , 
      const string&        input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<StringGrammar,IntGrammar<int> >()[var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map< std::string , std::vector< std::string > >& result , 
      const string&                                    input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<StringGrammar,VectorGrammar<StringGrammar> >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map< std::string , std::vector< int > >& result , 
      const string&                            input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<StringGrammar,VectorGrammar<IntGrammar<int> > >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map< std::string , std::vector< double > >& result , 
      const string&                               input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<StringGrammar,VectorGrammar<RealGrammar<double> > >()
          [var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    /*  parse the pair expression (map-component)  " 'name' :value" 
     *  
     *  @code 
     *  
     *  const std::string input = "'PackageName':GaudiKernel" ;
     *  std::string name  ;
     *  std::string value ;
     *  StatusCode sc = Gaudi::Parsers::parse ( name , value , input ) ;
     *  if ( sc.isFailure() ) { ... } 
     *  std::cout <<  "\tParsed name  is " << name 
     *            <<  "\tParsed value is " << value << std::endl 
     *  @endcode 
     *  
     *  @param  name  (output) the parsed name of the component, defined 
     *                as 'name' or "name" before the column symbol ":", 
     *                the leading and trailing blans are omitted 
     *  @param  value (output) the parsed value of the component, 
     *                defined as everything after the column symbol ":" 
     *                till the end of the string
     *  @param  input (input) string to be parsed 
     *  @return status code 
     *  
     *  @author Alexander MAZUROV Alexander.Mazurov@gmail.com 
     *  @author Vanya BELYAEV  ibelyaev@physics.syr.edu
     *  @date 2006-05-12 
     */
    // ========================================================================
    StatusCode parse ( string& name , string&  value , const string& input )
    {
      return parse 
        ( input.c_str(),
          (
           ch_p('"' ) >> (+(anychar_p-ch_p('"' )))[assign_a(name)] >> ch_p('"' )
           |
           ch_p('\'') >> (+(anychar_p-ch_p('\'')))[assign_a(name)] >> ch_p('\'')
           )
          >> ":"
          >> (+anychar_p)[assign_a(value)], space_p).full;
    }
    // ========================================================================    
    StatusCode parse
    ( map<int,std::string>& result , 
      const string&         input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<IntGrammar<int>,StringGrammar>()[var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
    StatusCode parse
    ( map<unsigned int,std::string>& result , 
      const string&         input  )
    {
      return parse
        ( createIterator(input), 
          IteratorT(),
          MapGrammar<IntGrammar<unsigned int>,
          StringGrammar>()[var(result)=arg1],
          SkipperGrammar()).full;
    }
    // ========================================================================
  } // end of namespace Parsers
} // end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================


