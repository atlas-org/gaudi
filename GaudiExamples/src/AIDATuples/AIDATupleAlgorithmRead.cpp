// $Id: AIDATupleAlgorithmRead.cpp,v 1.3 2006/11/27 09:53:05 hmd Exp $
// Include files 
#include "AIDATupleAlgorithmRead.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAIDATupleSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"


// Static factory declaration

DECLARE_ALGORITHM_FACTORY(AIDATupleAlgorithmRead)

//--------------------------------------------------------
AIDATupleAlgorithmRead::AIDATupleAlgorithmRead(const std::string& name,
                                 ISvcLocator* pSvcLocator)
                :Algorithm(name, pSvcLocator)
//--------------------------------------------------------
{
  tuple = 0;
 }


//--------------------------------------
StatusCode AIDATupleAlgorithmRead::initialize()
//--------------------------------------
{
  StatusCode status;
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "Initializing..." << endreq;

  //status = atupleSvc()->myTest();
  
  //Book N-tuple 1

  status = atupleSvc()->retrieveObject("MyTuples/1", tuple);

  log << MSG::INFO << "Tuple: " << long(tuple) << endreq;
  log << MSG::INFO << "Number of rows of the Tuple: " << tuple->rows() << endreq;
  if( status.isSuccess() ) {
    log << MSG::INFO << "Registering Tuple" << endreq;
    status = atupleSvc()->registerObject("MyTuples", "1", tuple);
  }
  
  log << MSG::INFO << "Finished booking NTuples" << endmsg;

  return status;
}


//-----------------------------------
StatusCode AIDATupleAlgorithmRead::execute()
//-----------------------------------
{
        
  //StatusCode status;
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "Executing..." << endreq;
    
  int i_px = tuple->findColumn( "px" );
  int i_py = tuple->findColumn( "py" );
  int i_pz = tuple->findColumn( "pz" );
  int i_mass = tuple->findColumn( "mass" );
  
  float px;
  float py;
  float pz;
  float mass;
 

  tuple->start();
  int i = 0;
  while( tuple->next() ) {
    i++;
    log << MSG::INFO << "COLUMN " << i ;
    px = tuple->getFloat(i_px);
    log << MSG::INFO << " px: " << px ;
    py = tuple->getFloat(i_py);
    log << MSG::INFO << " py: " << py ;
    pz = tuple->getFloat(i_pz);
    log << MSG::INFO << " pz: " << pz ;
    mass = tuple->getFloat(i_mass);
    log << MSG::INFO << " mass: " << mass << endreq;
  }

  log << MSG::INFO << "Reading mass>1" << endreq;
  atupleSvc()->setCriteria(tuple,"mass>1");

  tuple->start();
  i = 0;
  while( tuple->next() ) {
    i++;
    log << MSG::INFO << "COLUMN " << i ;
    px = tuple->getFloat(i_px);
    log << MSG::INFO << " px: " << px ;
    py = tuple->getFloat(i_py);
    log << MSG::INFO << " py: " << py ;
    pz = tuple->getFloat(i_pz);
    log << MSG::INFO << " pz: " << pz ;
    mass = tuple->getFloat(i_mass);
    log << MSG::INFO << " mass: " << mass << endreq;
  }
  
  return StatusCode::SUCCESS;
}


//------------------------------------
StatusCode AIDATupleAlgorithmRead::finalize()
//------------------------------------
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalizing..." << endmsg;

	return StatusCode::SUCCESS;
}
