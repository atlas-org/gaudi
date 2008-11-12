#ifndef GAUDIEXAMPLES_INCIDENTLISTENERTEST_H_
#define GAUDIEXAMPLES_INCIDENTLISTENERTEST_H_


#include "GaudiKernel/IIncidentListener.h"

class ISvcLocator;
class IMessageSvc;
class IIncidentSvc;

/** @class IncidentListenerTest IncidentListenerTest.h 
 *  
 */
class IncidentListenerTest: virtual public IIncidentListener {

public:

  /// Constructor
  IncidentListenerTest( const std::string& name, ISvcLocator* svcloc, long shots = -1 );
  
  /// Destructor
  virtual ~IncidentListenerTest();

	/// Reimplements from IInterface
  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvInterface);

  /// Reimplements from IIncidentListener
  virtual void handle(const Incident& incident);
  
  /// Increment the reference count of Interface instance
  virtual unsigned long addRef();

  /// Release Interface instance
  virtual unsigned long release();
  
private:
	std::string m_name;
  ISvcLocator* m_svcloc;
  long m_shots;
  IMessageSvc *m_msgSvc;
  IIncidentSvc *m_incSvc;
  unsigned long m_refCount;
};

#endif /*GAUDIEXAMPLES_INCIDENTLISTENERTEST_H_*/
