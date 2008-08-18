// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/src/Lib/MsgStream.cpp,v 1.12 2007/04/04 08:22:27 hmd Exp $
//====================================================================
//	MsgStream.cpp
//--------------------------------------------------------------------
//
//	Package    : System ( The LHCb Offline System)
//
//  Description: Class to ease error logging to the message service
//
//	Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 29/10/98| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#define KERNEL_MSGSTREAM_CPP


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Message.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IMessageSvc.h"

#include <iostream>
#include <stdarg.h>
#include <stdio.h>

MsgStream::MsgStream(IMessageSvc* svc, int)
: m_service(svc),
  m_source(""),
  m_active(false)
{
  setLevel((0==svc) ? MSG::INFO : svc->outputLevel());
  m_useColors = (0==svc) ? false : svc->useColor();
}

MsgStream::MsgStream(IMessageSvc* svc, const std::string& source, int)
: m_service(svc),
  m_source(source),
  m_active(false)
{
  setLevel((0==svc) ? MSG::INFO : svc->outputLevel(source));
  m_useColors = (0==svc) ? false : svc->useColor();
}

MsgStream::~MsgStream()    {
}

MsgStream& MsgStream::doOutput()       {
  if ( isActive() )   {
    Message msg(m_source,m_currLevel,m_stream.str());
    if ( m_service != 0 )   {
      MSG::Level lvl = MSG::Level(m_service->outputLevel(m_source));
      m_service->setOutputLevel(m_source, m_currLevel);
      m_service->reportMessage (msg);
      m_service->setOutputLevel(m_source, lvl);
    }
    else     {
      std::cout << msg << std::endl;
    }
  }
  
  m_stream.str("");
  return *this;
}

void MsgStream::setColor(MSG::Color col) {
#ifndef _WIN32
  if ( m_useColors)   {
    int fc = 90 + col;
    m_stream << "[" << fc << ";1m";
  }
#endif
}

void MsgStream::setColor(MSG::Color fg, MSG::Color bg) {
#ifndef _WIN32
  if ( m_useColors )   {
    int fc = 90 + fg;
    m_stream << "[" << fc;
    int bc = 100 + bg;
      m_stream << ";" << bc;
    m_stream << ";1m";
  }
#endif
}

void MsgStream::resetColor() {
#ifndef _WIN32
  if ( m_useColors )   {
    m_stream << "[m" << m_service->getLogColor(m_currLevel);
  }
#endif
}

std::string format( const char* fmt, ... ) 
{
  const int buffsize = 2048;
  static char buffer[buffsize];
  va_list arguments;
  va_start( arguments, fmt );
  if( vsprintf(buffer, fmt, arguments) >= buffsize )
    throw GaudiException("Insufficient buffer size (2048) when formatting message",
                         "MsgStream", 0);
  return std::string(buffer);
}
