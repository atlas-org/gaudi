// $Id: Maps.h,v 1.7 2006/12/10 19:11:56 leggett Exp $
#ifndef GAUDIALG_MAPS_H
#define GAUDIALG_MAPS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/HistoID.h"
#include "GaudiAlg/TupleID.h"
// ============================================================================

// ============================================================================
/* @file Maps.h
 *
 *  Definitions of few useful hash-maps, classes and typedefs
 *  used for classes GaudiHistos and GaudiTuples.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-01-23
 */
// ============================================================================

// forward declrations
namespace AIDA
{
  class IHistogram1D ;
  class IHistogram2D ;
  class IHistogram3D ;
  class IProfile1D   ;
  class IProfile2D   ;
}
namespace Tuples { class TupleObj; }

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

  // ==============================================================
  // Linux
  // ==============================================================

  // ============================ 1D ==============================

  /// the actual type for  (title)->(1D histogram) map
  typedef GaudiUtils::HashMap
  <
    std::string                         , // Key
    AIDA::IHistogram1D*                   // Value
    > Histo1DMapTitle ;

  /// the actual type for (Numeric ID)->(1D histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::NumericID                  , // Key
    AIDA::IHistogram1D*                   // Value
    > Histo1DMapNumericID ;

  /// the actual type for (Literal ID)->(1D histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::LiteralID                  , // Key
    AIDA::IHistogram1D*                   // Value
    > Histo1DMapLiteralID ;

  // ============================ 2D ==============================

  /// the actual type for  (title)->(2D histogram) map
  typedef GaudiUtils::HashMap
  <
    std::string                         , // Key
    AIDA::IHistogram2D*                   // Value
    > Histo2DMapTitle ;

  /// the actual type for (Numeric ID)->(2D histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::NumericID                  , // Key
    AIDA::IHistogram2D*                   // Value
    > Histo2DMapNumericID ;

  /// the actual type for (Literal ID)->(2D histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::LiteralID                  , // Key
    AIDA::IHistogram2D*                   // Value
    > Histo2DMapLiteralID ;

  // ============================ 3D ==============================

  /// the actual type for  (title)->(3D histogram) map
  typedef GaudiUtils::HashMap
  <
    std::string                         , // Key
    AIDA::IHistogram3D*                   // Value
    > Histo3DMapTitle ;

  /// the actual type for (Numeric ID)->(3D histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::NumericID                  , // Key
    AIDA::IHistogram3D*                   // Value
    > Histo3DMapNumericID ;

  /// the actual type for (Literal ID)->(3D histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::LiteralID                  , // Key
    AIDA::IHistogram3D*                   // Value
    > Histo3DMapLiteralID ;

  // ============================ 1D Prof ==============================

  /// the actual type for  (title)->(1D Profile Histogram) map
  typedef GaudiUtils::HashMap
  <
    std::string                         , // Key
    AIDA::IProfile1D*                     // Value
    > Profile1DMapTitle ;

  /// the actual type for (Numeric ID)->(1D Profile histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::NumericID                  , // Key
    AIDA::IProfile1D*                     // Value
    > Profile1DMapNumericID ;

  /// the actual type for (Literal ID)->(1D Profile histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::LiteralID                  , // Key
    AIDA::IProfile1D*                     // Value
    > Profile1DMapLiteralID ;

  // ============================ 2D Prof ==============================

  /// the actual type for  (title)->(2D Profile Histogram) map
  typedef GaudiUtils::HashMap
  <
    std::string                         , // Key
    AIDA::IProfile2D*                     // Value
    > Profile2DMapTitle ;

  /// the actual type for (Numeric ID)->(2D Profile histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::NumericID                  , // Key
    AIDA::IProfile2D*                     // Value
    > Profile2DMapNumericID ;

  /// the actual type for (Literal ID)->(2D Profile histogram) map
  typedef GaudiUtils::HashMap
  <
    HistoID::LiteralID                  , // Key
    AIDA::IProfile2D*                     // Value
    > Profile2DMapLiteralID ;

  // ============================ NTuples ==============================

  /// the actual type for  (title)->(tuple) map
  typedef GaudiUtils::HashMap
  <
    std::string                         , // Key
    Tuples::TupleObj*                     // Value
    > TupleMapTitle ;

  /// the actual type for  (Numeric ID)->(tuple) map
  typedef GaudiUtils::HashMap
  <
    TupleID::NumericID                  , // Key
    Tuples::TupleObj*                     // Value
    > TupleMapNumericID  ;

  /// the actual type for  (Literal ID)->(tuple) map
  typedef GaudiUtils::HashMap
  <
    TupleID::LiteralID                  , // Key
    Tuples::TupleObj*                     // Value
    > TupleMapLiteralID  ;

}

#endif // GAUDIALG_MAPS_H
