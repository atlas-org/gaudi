// $Id: MyAlgorithm.cpp,v 1.10 2008/10/10 15:18:56 marcocle Exp $

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "IMyTool.h"
#include "MyAlgorithm.h"


// Static Factory declaration

DECLARE_ALGORITHM_FACTORY(MyAlgorithm);

// Constructor
//------------------------------------------------------------------------------
MyAlgorithm::MyAlgorithm(const std::string& name, ISvcLocator* ploc)
           : Algorithm(name, ploc) {
//------------------------------------------------------------------------------
  declareProperty("ToolWithName", m_privateToolType = "MyTool",
                  "Type of the tool to use (internal name is ToolWithName)");
}

//------------------------------------------------------------------------------
StatusCode MyAlgorithm::initialize() {
//------------------------------------------------------------------------------

  MsgStream log(msgSvc(), name());
  StatusCode sc;
  log << MSG::INFO << "initializing...." << endreq;

  sc = toolSvc()->retrieveTool("MyTool", m_publicTool );
  if( sc.isFailure() ) {
    log << MSG::ERROR<< "Error retrieving the public tool" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool("MyTool", m_privateTool, this );
  if( sc.isFailure() ) {
    log << MSG::ERROR<< "Error retrieving the private tool" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool("MyGaudiTool", m_publicGTool );
  if( sc.isFailure() ) {
    log << MSG::ERROR<< "Error retrieving the Gaudi public tool" << endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool("MyGaudiTool", m_privateGTool, this );
  if( sc.isFailure() ) {
    log << MSG::ERROR<< "Error retrieving the Gaudi private tool" << endreq;
    return sc;
  }
  
  sc = toolSvc()->retrieveTool(m_privateToolType, "ToolWithName",
                               m_privateToolWithName, this );
  if( sc.isFailure() ) {
    log << MSG::ERROR<< "Error retrieving the private tool with name" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("MyGaudiTool", m_privateOtherInterface, this );
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Error retrieving the Gaudi private tool with second interface" << endreq;
    return sc;
  }
  
  log << MSG::INFO << "....initialization done" << endreq;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode MyAlgorithm::execute() {
//------------------------------------------------------------------------------
  MsgStream         log( msgSvc(), name() );
  log << MSG::INFO << "executing...." << endreq;

  m_publicTool->doIt();
  m_privateTool->doIt();
  m_publicGTool->doIt();
  m_privateGTool->doIt();
  m_privateToolWithName->doIt();
  m_privateOtherInterface->doItAgain();

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode MyAlgorithm::finalize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalizing...." << endreq;

  toolSvc()->releaseTool( m_publicTool ).ignore();
  toolSvc()->releaseTool( m_privateTool ).ignore();
  toolSvc()->releaseTool( m_publicGTool ).ignore();
  toolSvc()->releaseTool( m_privateGTool ).ignore();
  toolSvc()->releaseTool( m_privateToolWithName ).ignore();
  toolSvc()->releaseTool( m_privateOtherInterface ).ignore();

  return StatusCode::SUCCESS;
}
