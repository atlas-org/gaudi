// $Id: MyAudAlgorithm.cpp,v 1.1 2007/01/22 16:06:13 hmd Exp $

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "IMyAudTool.h"
#include "MyAudAlgorithm.h"


// Static Factory declaration

DECLARE_ALGORITHM_FACTORY(MyAudAlgorithm);

// Constructor
//------------------------------------------------------------------------------
MyAudAlgorithm::MyAudAlgorithm(const std::string& name, ISvcLocator* ploc)
           : Algorithm(name, ploc), m_i(0) {
//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
StatusCode MyAudAlgorithm::initialize() {
//------------------------------------------------------------------------------

  MsgStream log(msgSvc(), name());
  StatusCode sc;
  log << MSG::INFO << "initializing...." << endreq;

  sc = toolSvc()->retrieveTool("MyAudTool", m_tool );
  if( sc.isFailure() ) {
    log << MSG::ERROR<< "Error retrieving the tool" << endreq;
  }

  log << MSG::INFO << "....initialization done" << endreq;

  log << MSG::INFO << "test " 
      << std::setw(4) 
      << std::setfill('0')
      << 10 << endreq;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode MyAudAlgorithm::execute() {
//------------------------------------------------------------------------------
  MsgStream         log( msgSvc(), name() );
  log << MSG::INFO << "executing...." << endreq;

  if (m_i%4 == 1) m_tool->doErr();
  if (m_i%4 == 2) m_tool->doFatal();
  if (m_i%5 == 0) log << MSG::ERROR << "testing AlgErrorAud" << endreq;

  log << MSG::ALWAYS << "This message should never be suppressed" << endreq;

  m_i ++;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode MyAudAlgorithm::finalize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalizing...." << endreq;

  log << MSG::ALWAYS << "none of the following 10 messages should be suppressed"
      << endreq;

  for (int i=0; i<10; ++i) {
    m_tool->doErr();
  }

  toolSvc()->releaseTool( m_tool ).ignore();

  return StatusCode::SUCCESS;
}
