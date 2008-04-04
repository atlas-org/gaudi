#include "MTHelloWorld.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "MTMessageSvc.h"


DECLARE_ALGORITHM_FACTORY(MTHelloWorld);

/////////////////////////////////////////////////////////////////////////////

MTHelloWorld::MTHelloWorld(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_myInt(0), m_myBool(0), m_myDouble(0)
{
  
  // Part 2: Declare the properties
  declareProperty("MyInt", m_myInt);
  declareProperty("MyBool", m_myBool);
  declareProperty("MyDouble", m_myDouble);

  declareProperty("MyStringVec",m_myStringVec);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initialize()" << endreq;

  MTMessageSvc* tmp_msgSvc = dynamic_cast<MTMessageSvc*> (msgSvc()); 
  if(tmp_msgSvc != 0) {
    log << MSG::INFO << " Algorithm = " << name() << " is connected to Message Service = "
        << tmp_msgSvc->name() << endreq;
  }

  // Part 2: Print out the property values
  log << MSG::INFO << "  MyInt =    " << m_myInt << endreq;
  log << MSG::INFO << "  MyBool =   " << (int)m_myBool << endreq;
  log << MSG::INFO << "  MyDouble = " << m_myDouble << endreq;
  
  for (unsigned int i=0; i<m_myStringVec.size(); i++) {
    log << MSG::INFO << "  MyStringVec[" << i << "] = " << m_myStringVec[i] 
	<< endreq;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::execute() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;

  // Part 1: Print out the different levels of messages
  log << MSG::DEBUG << "A DEBUG message" << endreq;
  log << MSG::INFO << "An INFO message" << endreq;
  log << MSG::WARNING << "A WARNING message" << endreq;
  log << MSG::ERROR << "An ERROR message" << endreq;
  log << MSG::FATAL << "A FATAL error message" << endreq;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorld::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize()" << endreq;

  return StatusCode::SUCCESS;
}
