#include "ColorMsgAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/AlgFactory.h"

// Static Factory declaration

DECLARE_ALGORITHM_FACTORY(ColorMsgAlg);


///////////////////////////////////////////////////////////////////////////

ColorMsgAlg::ColorMsgAlg( const std::string& name, ISvcLocator* pSvcLocator ) :
  Algorithm(name, pSvcLocator)
{

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ColorMsgAlg::initialize() {

  return StatusCode::SUCCESS;
  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ColorMsgAlg::execute() {

  MsgStream log(msgSvc(), name());

  log << MSG::FATAL << "THIS IS A FATAL MESSAGE" << endreq;
  log << MSG::ERROR << "THIS IS AN ERROR MESSAGE" << endreq;
  log << MSG::WARNING << "THIS IS A WARNING MESSAGE" << endreq;
  log << MSG::INFO << "THIS IS A INFO MESSAGE" << endreq;
  log << MSG::DEBUG << "THIS IS A DEBUG MESSAGE" << endreq;
  log << MSG::VERBOSE << "THIS IS A VERBOSE MESSAGE" << endreq;

  log << MSG::ERROR << "this is another";
  log.setColor(MSG::GREEN);
  log << " error ";
  log.resetColor();
  log << "message" << endreq;

  log << MSG::WARNING << "This is another";
  log.setColor(MSG::PURPLE);
  log << " warning ";
  log.resetColor();
  log << "message" << endreq;

  log << MSG::INFO << "testing colour" << endreq;

  log << MSG::INFO << "setting....";
  log.setColor(MSG::YELLOW);
  log << "this should be in yellow";
  log.setColor(MSG::RED);
  log << " and red";
  //  log.setColor(MSG::U1);
  //  log << "/n this should be in unknown";
  log << endreq;

  log << MSG::INFO << "multicolor: ";
  log.setColor(MSG::BLUE,MSG::GREEN);
  log << "this is blue on green";
  log.setColor(MSG::PURPLE,MSG::WHITE);
  log << " and purple on white" << endreq;
  
  log.setColor(MSG::GREEN);
  log << MSG::INFO << "This is green. ";
  log.setColor(MSG::YELLOW,MSG::BLUE);
  log << "This is in yellow on blue. ";
  log.resetColor();
  log << "This is back to normal" << endreq;


  return StatusCode::SUCCESS;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode ColorMsgAlg::finalize() {

  return StatusCode::SUCCESS;

}
