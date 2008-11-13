#ifndef GAUDIKERNEL_SERVICELOCATORHELPER_H
#define GAUDIKERNEL_SERVICELOCATORHELPER_H

#include <string>
#include <GaudiKernel/StatusCode.h>
class ISvcLocator;
class InterfaceID;
class MsgStream;

/** @class ServiceLocatorHelper
 *  @brief an helper to share the implementation of service() among the
 *         various kernel base classes
 * $Id: 
 */
class ServiceLocatorHelper {
public:
  ServiceLocatorHelper(ISvcLocator& svcLoc, 
		 MsgStream& log, //use requestor msg level
		 const std::string& requestorName):
    m_svcLoc(svcLoc), m_msgLog(log), m_requestorName(requestorName) {}

  StatusCode getService(const std::string& name,
			bool createIf,
			const InterfaceID& iid,
			void** ppSvc) const {
    return (createIf ? 
	    createService(name, iid, ppSvc) : 
	    locateService(name, iid, ppSvc, true));
  }

  StatusCode locateService(const std::string& name,
			   const InterfaceID& iid,
			   void** ppSvc, 
			   bool quiet=false) const;

  StatusCode createService(const std::string& name,
			   const InterfaceID& iid,
			   void** ppSvc) const;
  
  StatusCode createService(const std::string& type,
			   const std::string& name,
			   const InterfaceID& iid,
			   void** ppSvc) const;
private:
  std::string threadName() const;
  std::string threadedName(const std::string& name) const;
  bool isInThread() const;
  ISvcLocator* serviceLocator() const { return &m_svcLoc; }
  MsgStream& log() const { return m_msgLog; }
  const std::string& requestorName() const { return m_requestorName; }
  ISvcLocator& m_svcLoc;
  MsgStream& m_msgLog;
  std::string m_requestorName;
};
#endif
