// $Id: GFALStageFile.cpp,v 1.1.1.1 2007/01/15 12:22:56 hmd Exp $
// Include files

// local
#include "GFALStageFile.h"

// From Gaudi
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"



//-----------------------------------------------------------------------------
// Implementation file for class : GFALStageFile
//
// 2006-11-11 : Andres Felipe Osorio Oliveros
//              Greig COWAN
//
//
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GFALStageFile::GFALStageFile(  ) {

}

GFALStageFile::GFALStageFile(std::string name,IMessageSvc* ms)
{
  
  Tokenizer tok(true);
  
  m_MS=ms;
  
  m_rawfilename = name ;
  
  tok.analyse(m_rawfilename, " ", "", "", "=", "'", "'");
  for ( Tokenizer::Items::iterator i = tok.items().begin();
	i != tok.items().end();
	i++ )   {
    const std::string& tag = (*i).tag();
    const std::string& val = (*i).value();
    if (::toupper(tag[0])=='D') {
      name = val ;
    }
  }
  
  std::vector<std::string> prefixname;
  
  prefixname.push_back("PFN:");
  prefixname.push_back("LFN:");
  prefixname.push_back("lfn:");
  prefixname.push_back("guid:");
  prefixname.push_back("surl:");
  prefixname.push_back("srm:");
  prefixname.push_back("gfal:");
  prefixname.push_back("rfio:");
  //prefixname.push_back("castor:");
  
  m_isvalid = true;
  
  is_pfn  = false;
  is_guid = false;
  is_lfn  = false;
  is_surl = false;
  
  for(std::vector<std::string>::iterator i = prefixname.begin() ;
      i != prefixname.end();
      ++i) {
    size_t begpos = name.find(*i) ;
    if ( begpos != name.npos ) {
      size_t ntmp = i->length();
      name.erase(begpos,ntmp);
      if ((*i)        == prefixname[0] ) {
        is_pfn = true; 
      } else if ((*i) == prefixname[1] || (*i) == prefixname[2]) {
        is_lfn = true;
      } else if ((*i) == prefixname[3] ) {
        is_guid = true;
      } else if ((*i) == prefixname[4] || (*i) == prefixname[5]) {
        is_surl = true;
      }
      else { }
    }
  }
  
  
  m_filename = name ;
  m_status = UNKNOWN;
  m_MS=ms;
  
  
}

GFALStageFile::GFALStageFile(const GFALStageFile& rhs)
{
  m_filename    = rhs.m_filename;
  m_rawfilename = rhs.m_rawfilename;
  m_status      = rhs.m_status;
  m_reqid       = rhs.m_reqid;
  m_surl        = rhs.m_surl;
  m_turl        = rhs.m_turl;
  is_pfn        = rhs.is_pfn;
  is_lfn        = rhs.is_lfn;
  is_guid       = rhs.is_guid;
  is_surl       = rhs.is_surl;
  m_isvalid     = rhs.m_isvalid;
  m_MS          = rhs.m_MS;
  
}


GFALStageFile& GFALStageFile::operator=(const GFALStageFile& rhs)
{
  
  if (this != &rhs) {
    m_filename    = rhs.m_filename;
    m_rawfilename = rhs.m_rawfilename;
    m_status      = rhs.m_status;
    m_reqid       = rhs.m_reqid;
    m_surl        = rhs.m_surl;
    m_turl        = rhs.m_turl;
    is_pfn        = rhs.is_pfn;
    is_lfn        = rhs.is_lfn;
    is_guid       = rhs.is_guid;
    is_surl       = rhs.is_surl;
    m_isvalid     = rhs.m_isvalid;
    m_MS          = rhs.m_MS;

  }
  
  return *this ;
  
}

//=============================================================================
// Destructor
//=============================================================================
GFALStageFile::~GFALStageFile() {} 

//=============================================================================
StatusCode GFALStageFile::initialize()
{

  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << "initialise()" << endreq;
  
  char errbuf[512];
  int errbufsz = 512;
  char * ptr_errbuf = errbuf;
  char * ptr_guid;
  char * ptr_surl;
  
  if ( is_pfn ) {
    log << MSG::DEBUG << "File is PFN" << endreq;
    m_surl = std::string("srm://castorgridsc.cern.ch") + m_filename;
    m_status = EXIST;
  } 
  
  else if ( is_lfn ) {
    m_filename = std::string("/grid") + m_filename;
    log << MSG::DEBUG << "filename: " << m_filename << endreq;
    ptr_guid = guidfromlfn( m_filename.c_str(), ptr_errbuf, errbufsz );
    if (ptr_guid != NULL ) {
      ptr_surl = surlfromguid( ptr_guid, ptr_errbuf, errbufsz );
      if (ptr_surl == NULL ) {
        log << MSG::DEBUG << "Failed to get surl from guid" << endreq;
        log << MSG::ERROR << "surlfromguid> " << std::string(errbuf) << endreq;
        m_isvalid = false;
        return StatusCode::FAILURE;
      }
    } 
    else {
      log << MSG::DEBUG << "Failed to get surl from lfn" << endreq;
      m_isvalid = false;
      return StatusCode::FAILURE;
    }
    m_surl = std::string(ptr_surl);
    m_status = EXIST;
  }
  
  else if ( is_guid ) {
    ptr_surl = surlfromguid( m_filename.c_str(), ptr_errbuf, errbufsz );
    if (ptr_surl == NULL ) {
      log << MSG::DEBUG << "Failed to get surl from guid" << endreq;
      log << MSG::ERROR << "surlfromguid> " << std::string(errbuf) << endreq;
      m_isvalid = false;
      return StatusCode::FAILURE;
    }
    m_surl = std::string(ptr_surl);
    m_status = EXIST;
  } 
  
  else if ( is_surl ) {
    m_surl = std::string("srm:") + m_filename;
    m_status = EXIST;
  }
  
  else { 
    log << MSG::DEBUG << "initiliaze> failed to get SURL" << endreq;
    log << MSG::DEBUG << "initialize> type of input unknown" << endreq;
    m_isvalid = false;
  }
  
  log << MSG::DEBUG << "initiliaze> done." << endreq;
  
  return StatusCode::SUCCESS;
  
}

void GFALStageFile::setStatus(int status)
{ 
  if ( status == -1 ) {
    m_status = FAILED;
  }
  else if ( status == 0 ) {
    m_status = STGING;
  }
  else if ( status == 1 ) {
    m_status = STGED;
  }
  else m_status = UNKNOWN;
  
}

void GFALStageFile::setSurl(const std::string &surl)
{ 
  m_surl = surl;  
}

void GFALStageFile::setTurl(const std::string &turl)
{  
  m_turl = turl; 
}

void GFALStageFile::setRequestID( int reqid )
{  
  m_reqid = reqid; 
}

GFALStageFile::Status GFALStageFile::getStatus() const
{
  return m_status ;
}


bool GFALStageFile::isValid() const
{
  return m_isvalid ;
}

bool GFALStageFile::isPFN() const
{
  return is_pfn;
}


int GFALStageFile::getRequestID() const
{
  return m_reqid ;
}


std::string GFALStageFile::getSurl() const
{
  return m_surl ;
}


std::string GFALStageFile::getTurl() const
{
  return m_turl ;
}

IMessageSvc* GFALStageFile::msgSvc() const
{
  return m_MS;
}

