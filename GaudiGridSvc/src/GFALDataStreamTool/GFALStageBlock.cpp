// $Id: GFALStageBlock.cpp,v 1.2 2007/09/28 07:52:33 marcocle Exp $
// Include files

// local
#include "GFALStageBlock.h"

//From Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/System.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GFALStageBlock
//
// 2006-11-11 : Andres Felipe OSORIO OLIVEROS
//              Greig COWAN
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GFALStageBlock::GFALStageBlock(  ) {

}

GFALStageBlock::GFALStageBlock(size_t nmaxfiles,IMessageSvc* ms)
{

  m_MS = ms ;
  m_status = UNKNOWN ;
  m_nmaxfiles = nmaxfiles ;
  m_nfiles = 0 ;
  updateFillStatus();
  m_protocols.push_back(std::string("rfio"));
  m_protocols.push_back(std::string("gsidcap"));
  m_protocols.push_back(std::string("gridftp"));  
  
}

//=============================================================================
// Destructor
//=============================================================================
GFALStageBlock::~GFALStageBlock() {
  
  std::vector<GFALStageFile*>::iterator itr = m_files.begin() ;
  while( (itr != m_files.end()) ) {
    if ( (*itr) ) delete (*itr);
    ++itr;
  }
  
}

//=============================================================================
StatusCode GFALStageBlock::stageInitialize()
{
  
  StatusCode stc = StatusCode::SUCCESS ;
  
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "stageInitialize> preparing files for stage process" << endreq;
  
  if (m_nfiles == 0) {
    return StatusCode::FAILURE;
  }
  
  //get for each GFALStageFile its surl
  int k=0;

  m_nmaxfiles = m_files.size();
  
  for(std::vector<GFALStageFile*>::iterator itr = m_files.begin();
      itr != m_files.end();
      ++itr)
  {
    
    if( (*itr)->isValid() ) {
      std::string fsurl = (*itr)->getSurl();
      m_surls.push_back(fsurl);
      ptr_surls[k] = (char*) fsurl.c_str();
      ++k;
    }
    else {
      //GFALStageFile set to status=unknown
      log << MSG::DEBUG << "stageInitialize> could not get surl from file"  << endreq;
      stc = StatusCode::FAILURE;
    }
  }    
  
  //protocols
  k = 0;
  for(std::vector<std::string>::iterator itr = m_protocols.begin();
      itr != m_protocols.end();
      ++itr) {
    std::string protocol = (*itr);
    ptr_protocols[k] = (char*) protocol.c_str();
    ++k;
  }
  
  m_nprotocols = m_protocols.size();
  
  //Not fully implemented (in gfal)
  m_reqid    = 0;
  //m_tokens   = 0;
  m_errbuf   = NULL;
  m_errbufsz = 0;

  //Timeout default value
  m_timeout  = 100;
  
  log << MSG::DEBUG << "stageInitialize> done" << endreq;
  
  return stc;
  
}


StatusCode GFALStageBlock::Stage()
{
  
  StatusCode stc = StatusCode::SUCCESS ;
  
  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << "Stage> starts here." << endreq;

  if ( stageInitialize()==StatusCode::FAILURE) {
    stc = StatusCode::FAILURE ;
    return stc ;
  }
  
  if ( stageFinalize()==StatusCode::FAILURE) {
    stc = StatusCode::FAILURE ;
  }
  
  log << MSG::DEBUG << "invoking srm_getx()" << endreq;
  
  int ret = srm_getx( m_nmaxfiles, const_cast<const char **>(ptr_surls),
                      m_nprotocols, ptr_protocols, &m_reqid,
                      &m_filestatuses, m_errbuf, m_errbufsz, m_timeout);
  
  log << MSG::DEBUG << "srm_getx() returned: " << ret << endreq;
  log << MSG::DEBUG << "checking now filestatus: " << endreq;

  if ( ret == -1 ) return StatusCode::FAILURE;
    
  struct srm_filestatus *fs = m_filestatuses;
  
  std::vector<GFALStageFile*>::iterator itr = m_files.begin();

  int pos = 0;
  int nstaging = 0;
  int nstaged = 0;
  
  for( int i = 0; i < (int)m_nmaxfiles; ++i) {
    
    log << MSG::DEBUG << "file status: " << fs->status << endreq;
    (*itr)->setStatus( fs->status );
    
    if( fs->status == -1 ) {
      log << MSG::DEBUG << "File is FAILED" << endreq;
      m_failedAt.push_back( pos );
    }
    else if ( fs->status == 0 ) {
      log << MSG::DEBUG << "File is STGING" << endreq; 
      (*itr)->setTurl(std::string("PENDING"));
      (*itr)->setRequestID( fs->fileid );
      ++nstaging;
    }
    else if ( fs->status == 1 ) {
      log << MSG::DEBUG << "File is STGED" << endreq;
      (*itr)->setTurl(std::string( fs->turl ));
      (*itr)->setRequestID( fs->fileid );
      log << MSG::DEBUG << "turl: \n" 
          << std::string( fs->turl ) << endreq;
      ++nstaged;
    }
    else {
      log << MSG::DEBUG << "status UNKNOWN" << endreq;
    }
    
    ++itr;
    fs++;
  }
  
  if ( nstaging > 0 ) m_status = STGING;
  else if (nstaged == (int)m_nmaxfiles) {
    log << MSG::DEBUG << "block set to STGED" << endreq;
    m_status = STGED;
  }
  else {
    log << MSG::DEBUG << "block set to FAILED" << endreq;
    m_status = FAILED;
  }
  
  log << MSG::DEBUG << " Stage> done" << endreq;
  
  return stc ;

}


StatusCode GFALStageBlock::stageFinalize()
{
  
  StatusCode stc = StatusCode::SUCCESS ;
  return stc ;

}


StatusCode GFALStageBlock::addFile(GFALStageFile * stgfile)
{
  
  MsgStream log( msgSvc(), name() );
    
  if (getFillStatus()==FULL) {
    log << MSG::DEBUG << "addFile> Failed. block is full" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_files.push_back(stgfile);
  m_nfiles++;
  updateFillStatus();
  
  return StatusCode::SUCCESS;
  
}

void GFALStageBlock::updateFillStatus()
{
  if(m_nfiles == 0) {
    m_fillstatus = EMPTY ;
  }
  else if(m_nfiles != 0){
    m_fillstatus = FREE ;
  }
  else if(m_nfiles == (m_nmaxfiles-1)) {
    m_fillstatus = FULL ;
  }
  
  else {}
  
  return ;
  
}

void GFALStageBlock::updateStatus()
{

  MsgStream log( msgSvc(), name() );
  
  int ret = srm_get( m_nmaxfiles,const_cast<const char**>(ptr_surls),
                     m_nprotocols, ptr_protocols, &m_reqid, m_tokens,
                     &m_filestatuses, m_timeout);
  
  log << MSG::DEBUG << "srm_get() returned: " << ret << endreq;
  log << MSG::DEBUG << "checking now filestatus: " << endreq;

  if ( ret == -1 ) return;
    
  struct srm_filestatus *fs = m_filestatuses;
  
  std::vector<GFALStageFile*>::iterator itr = m_files.begin();
  
  int pos = 0;
  int nstaged = 0;
  int nfailed = 0;
    
  for( int i = 0; i < (int)m_nmaxfiles; ++i) {

    log << MSG::DEBUG << "file status: " << fs->status << endreq;
    (*itr)->setStatus( fs->status );
    
    if( fs->status == -1 ) {
      log << MSG::DEBUG << "File is FAILED" << endreq;
      m_failedAt.push_back( pos );
      ++nfailed;
    }
    else if ( fs->status == 0 ) {
      log << MSG::DEBUG << "File is STGING" << endreq;
      (*itr)->setTurl(std::string("PENDING"));
      (*itr)->setRequestID( fs->fileid );
    }
    else if ( fs->status == 1 ) {
      log << MSG::DEBUG << "File is STGED" << endreq;
      log << MSG::DEBUG << "turl:" 
          << std::string( fs->turl ) << endreq;
      (*itr)->setTurl(std::string( fs->turl ));
      (*itr)->setRequestID( fs->fileid );
      ++nstaged;
    }
    else {
      log << MSG::DEBUG << "UNKNOWN status" << endreq;
    }
    
    ++itr;
    fs++;
    ++pos;
  }
  
  if (nstaged == ( (int) m_nmaxfiles - nfailed) && nstaged != 0 ) m_status = STGED ;
  else if ( nfailed == (int) m_nmaxfiles ) m_status = FAILED;
  else  m_status = STGING;
  
  return ;

}

GFALStageBlock::Status GFALStageBlock::getStatus() const
{
  return m_status ;
}


GFALStageBlock::FillStatus GFALStageBlock::getFillStatus() const
{
  return m_fillstatus ;
}
