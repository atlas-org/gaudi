#ifndef GAUDIKERNEL_IISSUELOGGER_H
#define GAUDIKERNEL_IISSUELOGGER_H

#ifndef GAUDIKERNEL_ISERVICE_H
 #include "GaudiKernel/IService.h"
#endif
#include "GaudiKernel/IssueSeverity.h"
#include <string>


class IIssueLogger: virtual public IService {

public:

  static const InterfaceID& interfaceID();

  virtual void report(IssueSeverity::Level level, const std::string& msg,
		      const std::string& origin) = 0;
  virtual void report(const IssueSeverity& err) = 0;

};

inline
const InterfaceID&
IIssueLogger::interfaceID() {

  static const InterfaceID m_IID("IssueLogger", 1, 0);

  return m_IID;
}

#endif
