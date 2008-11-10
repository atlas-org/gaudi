// $Id: GFALDataStreamTool.h,v 1.1.1.1 2007/01/15 12:22:56 hmd Exp $
#ifndef GAUDIGRIDSVC_GFALDATASTREAMTOOL_H 
#define GAUDIGRIDSVC_GFALDATASTREAMTOOL_H 1

// Include files

#include <vector>

// Local
#include "GFALStageBlock.h"

// from Gaudi
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/EventSelectorDataStream.h"
#include "GaudiKernel/DataStreamTool.h"



/** @class GFALDataStreamTool GFALDataStreamTool.h GaudiGridSvc/GFALDataStreamTool.h
 *  
 *
 *  @author Andres Osorio
 *  @author Greig Cowan
 *  @date   2006-09-29
 *
 *  Implementation of DataStreamTool using GFAL
 *
 *
 */

//class DataStreamTool;

class GFALDataStreamTool : virtual public DataStreamTool {
public: 
  
  enum Status {UNKNOWN=0,STGING,DONE};
  
  /// Standard constructor
  GFALDataStreamTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  virtual ~GFALDataStreamTool( ); ///< Destructor

  virtual StatusCode initialize();
  
  virtual StatusCode finalize();
  
  virtual StatusCode addStreams(const StreamSpecs &);

  virtual StatusCode getNextStream( const EventSelectorDataStream* &, size_type & );
  
protected:
  
  StatusCode updateStreams( const std::string & , const std::string & );
  
private:
  
  size_type m_turlCount;
    
  StreamSpecs m_streamTurls;
  
  StatusCode setBlocks();
  
  StatusCode firstBlockStage();
  
  StatusCode nextBlockStage();
  
  StatusCode updateBlockStatus();

  StatusCode updateBlockStatus(GFALStageBlock *);
  
  StatusCode updateStreamTurls();
  
  void addBlock(GFALStageBlock *);
    
  std::vector<GFALStageBlock*> m_blocks ;
  
  std::vector<GFALStageBlock*>::iterator m_blockitr;
  
  long int m_pos;
      
  size_t m_blockindex ;
  ///< index of the last staged block
  size_t m_ntotblocks ;
  ///< total number of blocks
  size_t m_blocksize ;
  ///< number of files in one stage block
  size_t m_initstage ;
  ///< number of blocks for the initial prestage
  size_t m_readfile ;
  ///< number of files (streams) read.

  int m_atblock ;
  
  Status m_task;
  
};
#endif // GAUDIGRIDSVC_GFALDATASTREAMTOOL_H
