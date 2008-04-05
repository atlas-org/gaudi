
// ============================================================================
/*  @file gaudiHistoID.cpp
 *
 *  Implementation file for class GaudiAlg::ID
 *
 *  $Id: GaudiHistoID.cpp,v 1.1 2006/03/09 14:50:22 hmd Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   03/02/2006
 */
// ============================================================================

// local
#include "GaudiAlg/GaudiHistoID.h"

// Boost
#include "boost/lexical_cast.hpp"

// ============================================================================

GaudiAlg::ID::LiteralID GaudiAlg::ID::idAsString() const
{
  return ( literal() ? literalID() : 
           boost::lexical_cast<LiteralID>(numericID()) );
}

// ============================================================================
