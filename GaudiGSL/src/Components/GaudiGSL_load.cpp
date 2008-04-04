// $Id: GaudiGSL_load.cpp,v 1.1 2003/11/19 17:09:41 mato Exp $
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// ============================================================================
/** @file 
 *  Mandatory file for loading of factories 
 *  @author Vanya Ivan.Belyaev@itep.ru
 *  @date   30/04/2002
 */
// ============================================================================

DECLARE_FACTORY_ENTRIES(GaudiGSL) {
  
  DECLARE_SERVICE( GslSvc        ) ;
  
  DECLARE_TOOL(    GslErrorCount     ) ;
  DECLARE_TOOL(    GslErrorPrint     ) ;
  DECLARE_TOOL(    GslErrorException ) ;

  DECLARE_TOOL(    FuncMinimum       ) ;
  
};

// ============================================================================
// The END 
// ============================================================================
