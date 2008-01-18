// $Id: GFALStageBlock.h,v 1.1.1.1 2007/01/15 12:22:56 hmd Exp $
#ifndef GAUDIGRIDSVC_GFALSTAGEBLOCK_H 
#define GAUDIGRIDSVC_GFALSTAGEBLOCK_H 1

// Include files
#include <string>
#include <vector>
#include <iterator>

#include "GFALStageFile.h"

/** @class GFALStageBlock GFALStageBlock.h GaudiGridSvc/GFALStageBlock.h
 *  
 *
 *  @author Andres Felipe OSORIO OLIVEROS
 *  @author Greig COWAN
 *  @date   2006-11-11
 *
 *
 *
 */


class GFALStageBlock {
public: 

  enum Status {UNKNOWN=0,STGING,STGED,FAILED} ;
  enum FillStatus {EMPTY=0,FREE,FULL} ;
  
  /// Standard constructor
  GFALStageBlock( );
  
  virtual ~GFALStageBlock( ); ///< Destructor
  
  GFALStageBlock(size_t nmaxfiles,IMessageSvc* ms) ;
  
  StatusCode Stage() ;
  
  StatusCode stageInitialize() ;
  
  StatusCode stageFinalize() ;
  
  StatusCode addFile(GFALStageFile *) ;
  
  void updateStatus() ;
  
  Status getStatus() const;
  
  FillStatus getFillStatus() const ;
  
  const std::string name() const { return std::string("StageBlock") ; }
  
  IMessageSvc* msgSvc() const { return m_MS ; } 

  std::vector<GFALStageFile*> m_files ;


protected:

private:
  
  void updateFillStatus() ;
  
  size_t m_nfiles ;
  size_t m_nmaxfiles;
  int m_nprotocols;
  
  std::vector<std::string> m_surls;
  std::vector<std::string> m_protocols;
  
  std::vector<int> m_failedAt;
  
  //gfal srm_getx arguments:
  
  char *ptr_surls[10000];
  char *ptr_protocols[100];
  char **m_tokens;
  int m_reqid;
  char *m_errbuf;
  int m_errbufsz;
  int m_timeout;
  
  struct srm_filestatus *m_filestatuses;
  
  Status m_status ;
  
  FillStatus m_fillstatus ;
  
  mutable IMessageSvc* m_MS ;
  
  
};
#endif // GAUDIGRIDSVC_GFALSTAGEBLOCK_H
