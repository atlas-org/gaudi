#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/ServiceLocatorHelper.h"

std::string
ServiceLocatorHelper::threadName() const {
  return getGaudiThreadIDfromName(requestorName());
}

std::string
ServiceLocatorHelper::threadedName(const std::string& name) const {
  return ( isInThread() ? name + threadName() : name );
}

bool
ServiceLocatorHelper::isInThread() const {
  return isGaudiThreaded(requestorName());
}

StatusCode 
ServiceLocatorHelper::locateService(const std::string& name, 
				    const InterfaceID& iid, 
				    void** ppSvc,
				    bool quiet) const {
  IService* theSvc(0);
  StatusCode sc(StatusCode::FAILURE);
  if (isInThread()) {
    //first we look for  a thread-specific version of the service
    sc=serviceLocator()->getService(name + threadName(), theSvc, false);
  };
  // if not, try to find the common, single-threaded version of the service
  if (!sc.isSuccess()) sc=serviceLocator()->getService(name, theSvc,false);
  
  if ( sc.isSuccess() ) {
    if (!quiet) log() << MSG::VERBOSE 
	  << "ServiceLocatorHelper::locateService: found service " 
	  << name <<endreq;
    sc = theSvc->queryInterface(iid, ppSvc);
    if (!sc.isSuccess()) {
      *ppSvc=0;
      if (!quiet) log() << MSG::ERROR 
	    << "ServiceLocatorHelper::locateService: wrong interface id " 
	    << iid << " for service " << name << endreq;
    }
  } else {
    // if not return an error
    if (!quiet) {
      log() << MSG::ERROR 
	    << "ServiceLocatorHelper::locateService: can not locate service " 
	    << name;
      if (isInThread()) log() << MSG::ERROR << " or " << name + threadName();
      log() << MSG::ERROR << endreq;
    }
    *ppSvc = 0;
  }
  return sc;
}

StatusCode 
ServiceLocatorHelper::createService(const std::string& name,
				   const InterfaceID& iid, 
				   void** ppSvc) const {

  IService* theSvc(0);
  StatusCode sc(StatusCode::FAILURE);
  if (isInThread()) {
    //first we look for  a thread-specific version of the service
    sc=serviceLocator()->getService(threadedName(name), theSvc, true);
  };
  // if not, try to find the common, single-threaded version of the service
  if (!sc.isSuccess()) sc=serviceLocator()->getService(name, theSvc, true);
  
  if ( sc.isSuccess() ) {
    sc = theSvc->queryInterface(iid, ppSvc);
    if (!sc.isSuccess()) {
      *ppSvc=0;
    }
  } else {
    *ppSvc = 0;
  }

  if (sc.isSuccess()) { 
#ifndef NDEBUG
    log() << MSG::VERBOSE 
	  << "ServiceLocatorHelper::createService: found service " 
	  << threadedName(name) <<endreq;
#endif
  } else {
    log() << MSG::ERROR 
 	  << "ServiceLocatorHelper::createService: can not create service " 
 	  << threadedName(name) << endreq;
  }
  return sc;
}

StatusCode 
ServiceLocatorHelper::createService(const std::string& type,
				    const std::string& name,
				    const InterfaceID& iid, 
				    void** ppSvc) const {


  IService* theSvc(0);
  StatusCode sc(StatusCode::FAILURE);

  //first we need to declare it
  try { 
    dynamic_cast<ISvcManager&>(*serviceLocator()).declareSvcType(threadedName(name),type).ignore(); 
  } 
  catch(...) {
    log() << MSG::ERROR  
          << "ServiceLocatorHelper::createService: can not declare service " 
          << threadedName(name) << " of type " << type << endreq;
    return StatusCode::FAILURE;
  }

  if (isInThread()) {
    //first we look for  a thread-specific version of the service
    sc=serviceLocator()->getService(threadedName(name), theSvc, true);
  };
  // if not, try to find the common, single-threaded version of the service
  if (!sc.isSuccess()) sc=serviceLocator()->getService(name, theSvc, true);


  if ( sc.isSuccess() ) {
    sc = theSvc->queryInterface(iid, ppSvc);
    if (!sc.isSuccess()) {
      *ppSvc=0;
    }
  } else {
    *ppSvc = 0;
  }

  if (sc.isSuccess()) { 
#ifndef NDEBUG
    log() << MSG::VERBOSE 
	  << "ServiceLocatorHelper::createService: found service " 
	  << threadedName(name) <<endreq;
#endif
  } else {
    log() << MSG::ERROR 
 	  << "ServiceLocatorHelper::createService: can not create service " 
 	  << threadedName(name) << " of type " << type << endreq;
  }
  return sc;

}

