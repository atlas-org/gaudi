// $Id: HbookTool.cpp,v 1.3 2007/05/24 14:41:22 hmd Exp $
// ============================================================================
// CVS tag $Name:  $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h" 
// ============================================================================

// ============================================================================
/** @file
 *
 *  Definition and implementation of HbookTool 
 *  - helper tool, which allows to transfere some configurable 
 *  properties to HFileCnv. It is useful to e.g. increase the 
 *  default file parameters like LRECL, mode , etc...
 *  In particular it is nesessary to obtain *LARGE* HBOOK-files
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2005-11-25
 */
// ============================================================================

// ============================================================================
/** @class HbookTool HbookTool.h
 *  
 *   Helper tool, which allows to transfere some configurable 
 *  properties to HFileCnv. It is useful to e.g. increase the 
 *  default file parameters like LRECL, mode , etc...
 *  In particular it is nesessary to obtain *LARGE* HBOOK-files
 *
 *  The tool has 3 properties 
 *
 *   - LRECL : the default record length (default = 1024), it could
 *             increased to increase the file capacity. 
 *   - NRECL : Number of records (used only if "Q" specified for 
 *             the MODE)
 *   - MODE  : HROPEN mode, (the default value is "P"). "PQ" or even 
 *             "PQE" should be used for creation of *HUGE* files.
 *             @see http://cern.ch/webasd/cgi-bin/listpawfaqs.pl/7
 * 
 *  @attention If "LRECL" if different from the default value, do not 
 *  forget in PAW (in the command "HI/FILE") to specify the "LRECL"
 *  parameters. Note that if the recond length is smaller than 8192,  
 *  PAW is able to autmatically determine the correct record length 
 *  if "LRECL" parameter is set to 0 for the command "HI/FILE"
 *
 *  The recommended setup to produce *HUGE* HBOOK files:
 *
 *    - LRECL=4096 
 *    - NRECL=512000
 *    - MODE="PQE" 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2005-11-25
 */
// ============================================================================
class HbookTool : public AlgTool 
{
public: 
  /** Standard constructor
   *  @param type the actual tool type (?)
   *  @param name tool name 
   *  @param parent tool parent 
   */
  HbookTool 
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent )
    : AlgTool( type , name , parent ) 
    // 
    , m_LRECL ( 1024 ) 
    , m_NRECL ( -1   ) 
    , m_MODE  ( "P"  )
  {
    declareProperty ( "LRECL" , m_LRECL ) ;
    declareProperty ( "NRECL" , m_NRECL ) ;
    declareProperty ( "MODE"  , m_MODE  ) ;
  };
  
  virtual ~HbookTool(){}; ///< Destructor
  
private:
  
  int         m_LRECL ;
  int         m_NRECL ;
  std::string m_MODE  ;
  
};
// ===========================================================================

// ===========================================================================
DECLARE_TOOL_FACTORY(HbookTool)
// ============================================================================
