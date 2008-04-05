
// ============================================================================
/*  @file gaudiHistoID.h
 *
 *  Header file for class GaudiAlg::ID
 *
 *  $Id: GaudiHistoID.h,v 1.7 2007/09/19 08:17:54 marcocle Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-23
 */
// ============================================================================

#ifndef GAUDIALG_GAUDIHISTOID_H
#define GAUDIALG_GAUDIHISTOID_H 1
// STD
#include <string>

// ============================================================================
/** @namespace GaudiAlg
 *
 *  Definitions of few useful hash-maps, classes and typedefs
 *  used for classes GaudiHistos and GaudiTuples.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-23
 */
// ============================================================================

namespace GaudiAlg
{
  // ============================================================================
  /** @class ID GaudiHistoID.h GaudiAlg/GaudiHistoID.h
   *
   *  ID class for Histogram and Ntuples. Internally handles both
   *  numeric and string like IDs
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2005-08-12
   */
  class ID
  {
  public:
    /// type for internal numeric ID
    typedef int         NumericID;
    /// type for internal literal ID
    typedef std::string LiteralID;
  public:
    /// Implicit constructor from a numeric ID
    ID( const NumericID    id = -1 ) : m_nID ( id ) , m_aID( "" ) { }
    /// Implicit constructor from a literal ID
    ID( const LiteralID&   id      ) : m_nID ( -1 ) , m_aID( id ) { }
    /// Implicit constructor from a char *
    ID( const char*        id      ) : m_nID ( -1 ) , m_aID( id ) { }
    /// Destructor
    ~ID( ) { }
    /// Is this ID numeric
    inline bool numeric   () const { return -1 != m_nID ;    }
    /// Is this ID numeric
    inline bool literal   () const { return !m_aID.empty() ; }
    /// Is this ID undefined
    inline bool undefined () const { return !numeric() && !literal(); }
    /// Returns the ID as a LiteralID
    inline const LiteralID& literalID() const { return m_aID; }
    /// Returns the numerical ID
    inline NumericID        numericID() const { return m_nID; }
    /** Operator ++   (prefix)
     *  @attention Only valid for numeric IDs. Has NO effect on literal IDs
     */
    inline ID& operator++() 
    { 
      if ( numeric() ) { ++m_nID; } ; 
      return *this ; 
    }
    /** Operator ++(int)  (postfix)
     *  @attention Only valid for numeric IDs. Has NO effect on literal IDs
     */
    inline ID operator++( int ) 
    { 
      const ID retID = *this;
      this->operator++();
      return retID;
    }
    /** Operator --   (prefix)
     *  @attention Only valid for numeric IDs. Has NO effect on literal IDs
     */
    inline ID& operator--() 
    { 
      if ( numeric() ) { --m_nID; } ; 
      return *this ; 
    }
    /** Operator --(int)  (postfix)
     *  @attention Only valid for numeric IDs. Has NO effect on literal IDs
     */
    inline ID operator--( int ) 
    { 
      const ID retID = *this;
      this->operator--();
      return retID;
    }
    /// Return ID as string, for both numeric and literal IDs
    LiteralID idAsString() const ;
    /// cast operator to std::striqng
    operator std::string () const { return idAsString () ; }
    /** @brief Implement the operator ==
     *  Implementation depends on type of ID
     *  @return boolean indicating if the IDs are equal
     */
    inline bool operator==( const ID& id ) const
    {
      return ( numeric() && id.numeric()   ? id.numericID() == numericID() :
               ( literal() && id.literal() ? id.literalID() == literalID() :
                 id.idAsString() == idAsString() ) );
    }
    /// Implement the != operator, using the == operator
    inline bool operator!=( const ID& id ) const
    {
      return ! this->operator==(id);
    }
    /** @brief Implement the operator <
     *  Implementation depends on type of ID
     *  @return boolean indicating the order of the IDs
     */
    inline bool operator<( const ID& id ) const
    {
      return ( numeric() && id.numeric()   ? this->numericID() < id.numericID() :
               ( literal() && id.literal() ? this->literalID() < id.literalID() :
                 this->idAsString() < id.idAsString() ) );
    }
  private:
    /// Internal numeric ID
    NumericID   m_nID ;
    /// Internal alpha-numeric ID
    LiteralID   m_aID ;
  };
}
/// Operator overloading for ostream
inline std::ostream& operator << ( std::ostream& str , const GaudiAlg::ID& id )
{
  if      ( id.numeric() ) { str << id.numericID(); }
  else if ( id.literal() ) { str << id.literalID(); }
  else                     { str << "UNDEFINED";    }
  return str;
}
#endif // GAUDIALG_GAUDIHISTOID_H
