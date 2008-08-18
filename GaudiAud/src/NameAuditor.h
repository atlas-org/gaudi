#ifndef ATLASAUDITOR_NAMEAUDITOR_H
#define ATLASAUDITOR_NAMEAUDITOR_H  

// ClassName: NameAuditor
//
// Description:  Prints the name of each algorithm before entering
// the algorithm and after leaving it
//
// Author: M. Shapiro, LBNL
//
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/Property.h"

class INamedInterface;

class NameAuditor:public  Auditor {

public:
  NameAuditor(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~NameAuditor();
  virtual void beforeInitialize(INamedInterface* alg);
  virtual void afterInitialize(INamedInterface* alg);
  virtual void beforeReinitialize(INamedInterface* alg);
  virtual void afterReinitialize(INamedInterface* alg);
  virtual void beforeExecute(INamedInterface* alg);
  virtual void afterExecute(INamedInterface* alg, const StatusCode& );
  virtual void beforeBeginRun(INamedInterface* alg);
  virtual void afterBeginRun(INamedInterface *alg);
  virtual void beforeEndRun(INamedInterface* alg);
  virtual void afterEndRun(INamedInterface *alg);
  virtual void beforeFinalize(INamedInterface* alg);
  virtual void afterFinalize(INamedInterface* alg);

  virtual void before(CustomEventTypeRef evt, const std::string& caller);
  virtual void after(CustomEventTypeRef evt, const std::string& caller, const StatusCode& );

private:

  StringArrayProperty m_types;

};

#endif
