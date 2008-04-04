// $Id: DataCreator.cpp,v 1.5 2007/10/29 14:25:31 hmd Exp $

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "DataCreator.h"

// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(DataCreator)

// Constructor
//------------------------------------------------------------------------------
DataCreator::DataCreator(const std::string& name, ISvcLocator* ploc)
           : GaudiAlgorithm(name, ploc) {
//------------------------------------------------------------------------------
  declareProperty("Data",  m_data = "/Event/Unknown" );
}

//------------------------------------------------------------------------------
StatusCode DataCreator::initialize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initializing...." << endreq;
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode DataCreator::execute() {
//------------------------------------------------------------------------------
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "executing...." << endreq;
  put(new DataObject(), m_data);
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode DataCreator::finalize() {
//------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalizing...." << endreq;
  return StatusCode::SUCCESS;
}

