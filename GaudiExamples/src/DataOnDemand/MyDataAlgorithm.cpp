// $Id: MyDataAlgorithm.cpp,v 1.4 2006/11/30 10:35:26 mato Exp $

// Include files

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "MyDataAlgorithm.h"


// Static Factory declaration

DECLARE_ALGORITHM_FACTORY(MyDataAlgorithm);

// Constructor
//------------------------------------------------------------------------------
MyDataAlgorithm::MyDataAlgorithm(const std::string& name, ISvcLocator* ploc)
           : GaudiAlgorithm(name, ploc) {
//------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
StatusCode MyDataAlgorithm::initialize() {
//------------------------------------------------------------------------------

  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "initializing...." << endreq;


  
  log << MSG::INFO << "....initialization done" << endreq;

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode MyDataAlgorithm::execute() {
//------------------------------------------------------------------------------
  MsgStream         log( msgSvc(), name() );
  log << MSG::INFO << "executing...." << endreq;
  get<DataObject>("Rec/Muon/Digits");
  get<DataObject>("Rec/Muon/Foos");
  log << MSG::INFO << "....execution done" << endreq;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode MyDataAlgorithm::finalize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalizing...." << endreq;


  return StatusCode::SUCCESS;
}
