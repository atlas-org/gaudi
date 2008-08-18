// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IAuditorSvc.h,v 1.8 2008/04/03 14:40:19 marcocle Exp $
#ifndef GAUDIKERNEL_IAUDITORSVC_H
#define GAUDIKERNEL_IAUDITORSVC_H

#include "GaudiKernel/IAuditor.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/System.h"
#include <string>


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IAuditorSvc(304, 2 , 0); 

/** @class IAuditorSvc IAuditorSvc.h GaudiKernel/IAuditorSvc.h

    The interface implemented by the IAuditorSvc base class.

    @author Pere Mato
*/
class IAuditorSvc : virtual public IService,
                    virtual public IAuditor {
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAuditorSvc; }

  /// management functionality: retrieve an Auditor
  virtual IAuditor* getAuditor( const std::string& name ) = 0;
  
};

#endif // INTERFACES_IAUDITORSVC_H
