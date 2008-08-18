
#ifndef GAUDIKERNEL_ISTATUSCODESVC_H
#define GAUDIKERNEL_ISTATUSCODESVC_H

#include "GaudiKernel/IService.h"

#include <string>

class IStatusCodeSvc: virtual public IService {

public:

  static const InterfaceID& interfaceID();

  virtual void regFnc(const std::string &fnc, 
		      const std::string &lib) = 0;

  virtual void list() const = 0;

  virtual bool suppressCheck() const = 0;

};

inline
const InterfaceID& 
IStatusCodeSvc::interfaceID() {
    static const InterfaceID m_IID("StatusCodeSvc", 1, 0);
    return m_IID;
}

#endif
