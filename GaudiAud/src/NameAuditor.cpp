// NameAuditor:
//  An auditor that prints the name of each algorithm method before
// and after it is called///

#include "NameAuditor.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/AudFactory.h"

DECLARE_AUDITOR_FACTORY(NameAuditor);

NameAuditor::NameAuditor(const std::string& name, ISvcLocator* pSvcLocator) :
  Auditor(name, pSvcLocator) 
{

  declareProperty("CustomEventTypes",m_types);

}

NameAuditor::~NameAuditor(){
}

void NameAuditor::beforeInitialize(INamedInterface* alg) {
  MsgStream log(msgSvc(), name()); 
  log << MSG::INFO << " About to Enter " << alg->name() << 
  " Initialization Method" << endreq;
}
void NameAuditor:: afterInitialize(INamedInterface* alg){
  MsgStream log(msgSvc(), name()); 
  log << MSG::INFO << "Just Exited " << alg->name() << 
  " Initialization Method" << endreq;
}

void NameAuditor::beforeReinitialize(INamedInterface *alg) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " About to Enter " << alg->name() <<
  " Reinitialization Method" << endreq;
}
void NameAuditor:: afterReinitialize(INamedInterface *alg){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Just Exited " << alg->name() <<
  " Reinitialization Method" << endreq;
}

void NameAuditor:: beforeExecute(INamedInterface* alg){
  MsgStream log(msgSvc(), name()); 
  log <<  MSG::INFO << "About to Enter " << alg->name() <<
  " Execute Method" << endreq;
}
void NameAuditor:: afterExecute(INamedInterface* alg, const StatusCode& ) {
  MsgStream log(msgSvc(), name()); 
  log <<  MSG::INFO << "Just Exited " << alg->name() << 
  " Execute Method" << endreq;
}

void NameAuditor::beforeBeginRun(INamedInterface *alg) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " About to Enter " << alg->name() <<
  " BeginRun Method" << endreq;
}
void NameAuditor:: afterBeginRun(INamedInterface *alg){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Just Exited " << alg->name() <<
  " BeginRun Method" << endreq;
}
void NameAuditor::beforeEndRun(INamedInterface *alg) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " About to Enter " << alg->name() <<
  " EndRun Method" << endreq;
}
void NameAuditor:: afterEndRun(INamedInterface *alg){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Just Exited " << alg->name() <<
  " EndRun Method" << endreq;
}

void NameAuditor:: beforeFinalize(INamedInterface* alg) {
  MsgStream log(msgSvc(), name()); 
  log <<  MSG::INFO << "About to Enter " << alg->name() <<
  " Finalize Method" << endreq;
}
void NameAuditor:: afterFinalize(INamedInterface* alg){
  MsgStream log(msgSvc(), name()); 
  log << MSG::INFO << "Just Exited " << alg->name() << 
  " Finalize Method" << endreq;
}

void
NameAuditor::before(CustomEventTypeRef evt, const std::string& caller) {

  if (m_types.value().size() != 0) {
    if ( (m_types.value())[0] == "none") {
      return;
    }
    
    if ( find(m_types.value().begin(), m_types.value().end(), evt) == 
	 m_types.value().end() ) {
      return;
    }
  }
  
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "About to Enter " << caller << " with auditor trigger "
      << evt << endreq;

}

void
NameAuditor::after(CustomEventTypeRef evt, const std::string& caller, const StatusCode&) {

  if (m_types.value().size() != 0) {
    if ( (m_types.value())[0] == "none") {
      return;
    }
    
    if ( find(m_types.value().begin(), m_types.value().end(), evt) == 
	 m_types.value().end() ) {
      return;
    }
  }
  
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "Just Exited " << caller << " with auditor trigger "
      << evt << endreq;

}
