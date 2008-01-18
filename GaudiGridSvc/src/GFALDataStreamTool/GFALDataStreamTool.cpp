// $Id: GFALDataStreamTool.cpp,v 1.3 2007/07/11 08:23:42 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ISvcManager.h"
#include "GaudiKernel/IAddressCreator.h"

// local
#include "GFALDataStreamTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GFALDataStreamTool
//
// 2006-09-29 : Andres Osorio
//            : Greig Cowan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GFALDataStreamTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GFALDataStreamTool::GFALDataStreamTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : DataStreamTool ( type, name , parent )
{

  declareProperty("BlockSize",m_blocksize = 1 );
  declareProperty("InitialStage",m_initstage = 1);
  
}

//=============================================================================
// Destructor
//=============================================================================
GFALDataStreamTool::~GFALDataStreamTool() 
{
  
  std::vector<GFALStageBlock*>::iterator itr = m_blocks.begin() ;
  while( (itr != m_blocks.end()) ) {
    if ( (*itr) ) delete (*itr);
    ++itr;
  }
  
} 

StatusCode GFALDataStreamTool::initialize() {

  MsgStream logger(msgSvc(), name());

  StatusCode status = DataStreamTool::initialize();
  if( !status.isSuccess() )  {
    logger << MSG::FATAL << "Error. Cannot initialize base class." << endreq;
    return status;
  }
  
  m_streamCount = 1;
  m_turlCount   = 0;
  
  m_atblock = 0;
  m_pos = 0;
  m_task = UNKNOWN;
  
  if (m_blocksize < 1) {
    logger << MSG::ERROR << "Wrong Block size" << endreq;
    logger << MSG::ERROR << "Prestaging disable" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode GFALDataStreamTool::addStreams(const StreamSpecs & inputs) {
  
  MsgStream logger(msgSvc(), name());
  
  StatusCode status = DataStreamTool::addStreams(inputs);
  
  ///////////////////////////
  ///initialize Stage  
  
  m_ntotblocks = 0;
  status = setBlocks();
    
  if ( ! status.isSuccess() ){
    logger << MSG::ERROR << "Unable to set Blocks" << endreq;
    return StatusCode::FAILURE;
  }
  
  status = firstBlockStage();
    
  if ( ! status.isSuccess() ) {
    logger << MSG::ERROR << "Unable stage first Block" << endreq;
    return StatusCode::FAILURE;
  }
  
  //Stage first block of files until all files in the block are STAGED 
  m_task = STGING;
  m_blockitr=m_blocks.begin();
  
  while ( ((*m_blockitr)->getStatus() != GFALStageBlock::STGED) && ((*m_blockitr)->getStatus() != GFALStageBlock::FAILED) ) {
    status = updateBlockStatus();
  }
  
  /////////////////////////////
  
  m_streamTurls = m_streamSpecs;

  if( status.isFailure() ){
    return status;
  }

  m_streamCount = 1;
  return updateStreamTurls();

}

StatusCode GFALDataStreamTool::finalize() {
  
  StatusCode iret, status = StatusCode::SUCCESS;
  iret.ignore();
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "finalise() starts now!" << endmsg;
  
  for ( StreamSpecs::const_iterator i = m_streamTurls.begin(); i != m_streamTurls.end(); i++ )    {
    EventSelectorDataStream * s  = DataStreamTool::getStream(*i);
    if ( NULL == s )   {
      if ( s->isInitialized() )    {
        iret = finalizeStream(s);
        if ( !iret.isSuccess() ) {
          log << MSG::ERROR << "Failed to finalize stream " << *i << endmsg;
          status = iret;
        }
      }
    }
    
    iret =  eraseStream( *i );
    
    if ( !iret.isSuccess() )    {
      log << MSG::ERROR << "Failed to disconnect stream " << *i << endmsg;
      status = iret;
    }
    
  }

  m_streamSpecs.clear(); //The input specification are not needed anymore
  
  log << MSG::DEBUG << "finalise() total files processed " << m_streamID << endmsg;

  status = DataStreamTool::finalize();
  
  return status;
  
}

StatusCode GFALDataStreamTool::getNextStream( const EventSelectorDataStream * & esds, size_type & dsid )
{
  
  EventSelectorDataStream * nextStream = getStream(dsid);
  if ( NULL == nextStream ) return StatusCode::FAILURE; //<-end of streams reached

  esds = nextStream;

  //Stage blocks request
  if ( m_task != DONE ) {
    
    if ( (*m_blockitr)->getStatus() == GFALStageBlock::STGED ) {
      nextBlockStage().ignore();
      updateStreamTurls().ignore();
    }
    updateBlockStatus().ignore();
  }
  //

  ++m_streamID;
  
  return StatusCode::SUCCESS;
  
}

/////////////////////////////
// Stage files functionality

StatusCode GFALDataStreamTool::setBlocks()
{
  
  MsgStream log (msgSvc(),name());
  
  GFALStageBlock * btmp = new GFALStageBlock(m_blocksize,msgSvc());
  
  log << MSG::DEBUG << "Setting up the blocks of files to stage. Total files: " 
      << m_streamSpecs.size()
      << endreq;
  
  log << MSG::DEBUG << "Each block is set to have "
      << m_blocksize << " files."
      << endreq;
  
  StatusCode fstatus = StatusCode::SUCCESS;
  
  int addedfiles(0);
  
  for(StreamSpecs::const_iterator i = m_streamSpecs.begin() ;
      i != m_streamSpecs.end(); ++i) {
    
    GFALStageFile * ftmp = new GFALStageFile(*i,msgSvc());

    ftmp->initialize().ignore();
    
    if (ftmp->isValid()) {
      
      if ( addedfiles != (int) m_blocksize ) {
        fstatus = btmp->addFile(ftmp);
        ++addedfiles;
      }
      
      if ( addedfiles == (int) m_blocksize  && (&(*i)!= &m_streamSpecs.back()) ) {
        addBlock(btmp);
        log << MSG::DEBUG << "adding block" << endreq;
        btmp = new GFALStageBlock(m_blocksize,msgSvc());
        addedfiles = 0;
      }
      
      if (&(*i)==&m_streamSpecs.back()) {
        log << MSG::DEBUG << "adding last block" << endreq;        
        addBlock(btmp);
      }
      
    }
    else {
      
      log << MSG::DEBUG <<" The file: " 
          << ftmp->rawFileName()
          << "] does not exist."
          << endreq;
      fstatus = StatusCode::FAILURE;
      
    }
  }
  
  log << MSG::DEBUG << "setBlocks done." << endreq;
  
  return fstatus;
  
}


StatusCode GFALDataStreamTool::firstBlockStage() {
  
  MsgStream log (msgSvc(),name());
  
  if (m_initstage==0) {
    log << MSG::DEBUG <<"No initial stage" << endreq;
    m_blockindex = 0 ;
  } else {
    log << MSG::DEBUG <<"Performing initial stage. Total number of blocks: " 
        << m_blocks.size() << endreq;
    m_blockitr = m_blocks.begin();

    log << MSG::DEBUG << "staging first block of files" << endreq;
    
    if ( m_blocks.size() > 0 ) {
      StatusCode status = (*m_blockitr)->Stage();
      if ( status.isFailure() ){
        log << MSG::ERROR << "firstBlockStage> unable to stage first block" << endreq;
        return status;
      }
    }
    else {
      log << MSG::DEBUG << "Block is empty: check input file format or grid certificate" << endreq;
      return StatusCode::FAILURE;
    }
    
  }
  
  log << MSG::DEBUG << "firstBlockStage> done." << endreq;
  
  return StatusCode::SUCCESS;
  
}

StatusCode GFALDataStreamTool::nextBlockStage() {
  
  //Initial stage for each block.  
  
  MsgStream log (msgSvc(),name());
  
  if ( (*m_blockitr)->getStatus() != GFALStageBlock::STGED )
    return StatusCode::SUCCESS;
  
  ++m_blockitr;
  
  if (m_blockitr == m_blocks.end() ) {

    log << MSG::DEBUG << "nextBlockStage> Last block reached" << endreq;
    m_task = DONE;
    return StatusCode::SUCCESS;
    
  }
  
  StatusCode status = (*m_blockitr)->Stage();
  if ( status.isFailure() ){
    log << MSG::ERROR << "nextBlockStage> unable to stage block" << endreq;
    return status;
  }
  
  ++m_blockindex;
  
  log << MSG::DEBUG << "nextBlockStage> done." << endreq;
  
  return StatusCode::SUCCESS;
  
}


StatusCode GFALDataStreamTool::updateBlockStatus( )
{
  
  MsgStream log (msgSvc(),name());

  if ( m_task == DONE ) return StatusCode::SUCCESS;
  
  log << MSG::DEBUG << "updateStatus> block" << endreq;
  
  (*m_blockitr)->updateStatus();
  
  return StatusCode::SUCCESS;
  
}

StatusCode GFALDataStreamTool::updateBlockStatus( GFALStageBlock * ablock )
{
  
  MsgStream log (msgSvc(),name());

  if ( m_task == DONE ) return StatusCode::SUCCESS;
  
  log << MSG::DEBUG << "updateStatus> block" << endreq;

  ablock->updateStatus();
  
  return StatusCode::SUCCESS;
  
}

void GFALDataStreamTool::addBlock(GFALStageBlock * b)
{
  
  m_blocks.push_back(b);
  m_ntotblocks++;
  
  return ;
  
}

StatusCode GFALDataStreamTool::updateStreamTurls() 
{

  MsgStream log (msgSvc(),name());
    
  StatusCode status;
  
  log << MSG::DEBUG << "updateStreamTurls>" << endreq;

  if ( m_task == DONE ) return StatusCode::SUCCESS;
    
  if ( (*m_blockitr)->getStatus() == GFALStageBlock::STGED) {
    
    std::vector<GFALStageFile*>::iterator stgfile = (*m_blockitr)->m_files.begin() ;
    
    while( (stgfile != (*m_blockitr)->m_files.end() ) ) {
      std::string turl = (*stgfile)->getTurl();
      if ( (*stgfile)->getStatus() == GFALStageFile::STGED  ) {
        int pos = m_streamTurls[m_turlCount].find("'",10);
        std::string tail = m_streamTurls[m_turlCount].erase(0,pos+1);
        std::string head;
        
        if ( ! (*stgfile)->isPFN() ) head = std::string("DATAFILE='gfal:") + turl + std::string("'");
        else {
          std::string pfnturl = turl.erase(0,8);
          head = std::string("DATAFILE='rfio:") + pfnturl + std::string("'");
        }
        
        m_streamTurls[m_turlCount] = head + tail;
        
        log << MSG::DEBUG << "Adding to m_streamTurl_" << m_turlCount << "\n"
            << m_streamTurls[m_turlCount] << endmsg;
        
        ////////////////////////////////////////////
        //Update the data stream list with the turl
        StatusCode fts = updateStreams( (*stgfile)->rawFileName(), m_streamTurls[m_turlCount] );
        
        if ( ! fts.isSuccess() ) log << MSG::DEBUG << "Failed to update Stream!" << endmsg;
        ++m_turlCount;
        
      }
      
      ++stgfile;
      
    }
    
  }
  
  return status;
  
}

StatusCode GFALDataStreamTool::updateStreams( const std::string & oldinfo, const std::string & uptinfo )
{
  
  Streams::iterator i = DataStreamTool::getStreamIterator(oldinfo);
  
  if ( i != m_streams.end() )   {
    (*i)->release();
    Streams::iterator p = m_streams.erase(i);
    char txt[32];
    std::string nam = name() + "_" + ::_itoa( ++m_streamCount, txt, 10);
    EventSelectorDataStream* s = 0;
    StatusCode status = DataStreamTool::createStream(nam, uptinfo, s);
    if ( status.isSuccess() )   {
      if ( 0 != s )   {
        s->addRef();
        m_streams.insert(p, s);
        return StatusCode::SUCCESS;
      }
    }
  }
  
  return StatusCode::FAILURE;
  
}
