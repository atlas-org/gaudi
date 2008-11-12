#include "GaudiKernel/SvcFactory.h"
#include "ServiceA.h"


// Static Factory declaration

DECLARE_SERVICE_FACTORY(ServiceA);

ServiceA::ServiceA(const std::string& nam, ISvcLocator* svcLoc) :
 Service(nam, svcLoc)
{
}

StatusCode
ServiceA::initialize() {
  Service::initialize().ignore();
  IService *pService(0);
  const bool CREATENOW(true);
  return service("ServiceB", pService, CREATENOW);
}

const InterfaceID&
ServiceA::interfaceID() {
  static const InterfaceID _IID("ServiceA", 1, 0);
  return _IID;
}

StatusCode
ServiceA::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if ( interfaceID().versionMatch(riid) )    {
     *ppvInterface = (ServiceA*)this;
   } else  {
     // Interface is not directly available: try out a base class
     return Service::queryInterface(riid, ppvInterface);
   }
   addRef();
   return StatusCode::SUCCESS;
}

