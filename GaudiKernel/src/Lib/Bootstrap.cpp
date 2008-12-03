//$Id: Bootstrap.cpp,v 1.14 2007/12/12 16:02:32 marcocle Exp $

#include <iostream>

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IClassManager.h"

#include "Reflex/PluginService.h"
using ROOT::Reflex::PluginService;


namespace Gaudi 
{
  
/** @class BootSvcLocator

    A dual-stage boostrap mechanism is used to ensure an orderly startup
    of the ApplicationMgr. If this function is called before the singleton
    ApplicationMgr instance exists, a BootstrapAppMgr singleton instance is
    created. This responds to any subsequent requests for services by
    returning StatusCode::FAILURE, unless the ApplicationMgr singleton 
    instance has been created in the interim. In this case, the BootstrapAppMgr
    forwards the request to the ApplicationMgr instance. The motiviation for
    this is to handle static object instances where the constructor attempts
    to locate services and would otherwise instantiate the ApplicationMgr
    instance in an unorderly manner. This logic requires that the 
    ApplicationMgr instance is created explicitly.
    
*/
  class BootSvcLocator : virtual public ISvcLocator {
  public:
    BootSvcLocator();
    virtual ~BootSvcLocator();
    /// implmentation of IInterface::addRef
    virtual unsigned long addRef();
    /// implmentation of IInterface::release
    virtual unsigned long release();
    /// implementation of IInterface::queryInterface
    virtual StatusCode queryInterface(const InterfaceID& iid, void** pinterface);
    virtual StatusCode getService( const std::string& name, 
                                   IService*&  svc );
    virtual StatusCode getService( const std::string& name,
                                   const InterfaceID& iid,
                                   IInterface*& pinterface );
    virtual StatusCode getService( const std::string& name,
                                   IService*& svc,
                                   bool createIf );
    virtual const std::list<IService*>& getServices( ) const;
    virtual bool existsService( const std::string& name ) const;
  private:
    unsigned long m_refcount;    ///< Reference counter
  };
}


static ISvcLocator* s_svclocInstance = 0;
static IAppMgrUI*   s_appmgrInstance = 0;

//------------------------------------------------------------------------------
bool Gaudi::hasApplicationMgr()
//------------------------------------------------------------------------------
{
  //Return true if the applicationmgr has already been created
  if ( 0 == s_appmgrInstance ) {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
IAppMgrUI* Gaudi::createApplicationMgr(const std::string& dllname,
                                       const std::string& factname)
//------------------------------------------------------------------------------
{
  // Allow not for multiple AppManagers: If already instantiated then just
  // return it
  if ( 0 == s_appmgrInstance )    {
    s_appmgrInstance = createApplicationMgrEx(dllname, factname);
    StatusCode sc = s_appmgrInstance->queryInterface( IID_ISvcLocator,
                                                      pp_cast<void>(&s_svclocInstance) );
  }
  return s_appmgrInstance;
}

//------------------------------------------------------------------------------
IAppMgrUI* Gaudi::createApplicationMgrEx(const std::string& dllname,
                                         const std::string& factname)
//------------------------------------------------------------------------------
{
  StatusCode     status;
  IInterface*    iif;
  IAppMgrUI*     iappmgr;

  // Create an instance of the application Manager
  iif = Gaudi::createInstance( "ApplicationMgr", factname, dllname );
  if( iif == 0 ) {
    return 0;
  }
  // Locate few interfaces of the Application Manager
  status = iif->queryInterface( IID_IAppMgrUI, pp_cast<void>(&iappmgr) );
  if( status.isFailure() ) {
    return 0;
  }
  iif->release();
  return iappmgr;
}

//------------------------------------------------------------------------------
ISvcLocator* Gaudi::svcLocator()
//------------------------------------------------------------------------------
//
// A dual-stage boostrap mechanism is used to ensure an orderly startup
// of the ApplicationMgr. If this function is called before the singleton
// ApplicationMgr instance exists, a BootSvcLocator singleton instance is
// created. This responds to any subsequent requests for services by
// returning StatusCode::FAILURE, unless the ApplicationMgr singleton 
// instance has been created in the interim. In this case, the BootSvcLocator
// forwards the request to the ApplicationMgr instance. The motiviation for
// this is to handle static object instances where the constructor attempts
// to locate services and would otherwise instantiate the ApplicationMgr
// instance in an unorderly manner. This logic requires that the 
// ApplicationMgr instance is created explicitly.

{
  if( 0 == s_svclocInstance )   {
    IAppMgrUI* iappmgr = createApplicationMgr();
    if( iappmgr ) {
      StatusCode sc = iappmgr->queryInterface( IID_ISvcLocator,
                                               pp_cast<void>(&s_svclocInstance) );
      if( sc.isSuccess() ) {
        return s_svclocInstance;
      }
    }
    //---Reverted change to create a Minimal SvcLocator in case is requested before AppMgr is created
    //if( 0 == s_appmgrInstance ) {
    //  s_svclocInstance = new BootSvcLocator();
    //} else {
    //  StatusCode sc = s_appmgrInstance->queryInterface( IID_ISvcLocator,
    //                                                  pp_cast<void>(&s_svclocInstance) );
    //  if( sc.isSuccess() ) {
    //    return s_svclocInstance;
    //  }
    //}
  }
  return s_svclocInstance;
}

//------------------------------------------------------------------------------
ISvcLocator* Gaudi::setInstance(ISvcLocator* newInstance)
//------------------------------------------------------------------------------
{
  ISvcLocator* oldInstance = s_svclocInstance;
  s_svclocInstance = newInstance;
  if ( s_appmgrInstance )     {
    s_appmgrInstance->release();
    s_appmgrInstance = 0;
  }
  if ( s_svclocInstance )    {
    s_svclocInstance->queryInterface (IID_IAppMgrUI, pp_cast<void>(&s_appmgrInstance));
  }
  return oldInstance;
}

//------------------------------------------------------------------------------
IAppMgrUI* Gaudi::setInstance(IAppMgrUI* newInstance)
//------------------------------------------------------------------------------
{
  IAppMgrUI* oldInstance = s_appmgrInstance;
  s_appmgrInstance = newInstance;
  if ( s_svclocInstance )     {
    s_svclocInstance->release();
    s_svclocInstance = 0;
  }
  if ( s_appmgrInstance )    {
    s_appmgrInstance->queryInterface (IID_ISvcLocator,
                                      pp_cast<void>(&s_svclocInstance));
  }
  return oldInstance;
}

//------------------------------------------------------------------------------
IInterface* Gaudi::createInstance( const std::string& name, 
                                                const std::string& factname, 
                                                const std::string& dllname)
//------------------------------------------------------------------------------
{

  IInterface* ii = PluginService::Create<IInterface*>(factname,(IInterface*)0);
  if ( ii ) return ii;
  IService* is = PluginService::Create<IService*>(factname, name, (ISvcLocator*)0);
  if ( is ) return is;
  IAlgorithm* ia = PluginService::Create<IAlgorithm*>(factname, name, (ISvcLocator*)0);
  if ( ia ) return ia;

  StatusCode status;
  void* libHandle = 0;
  status = System::loadDynamicLib( dllname, &libHandle);
  if ( status.isSuccess() )   {
    IInterface* ii = PluginService::Create<IInterface*>(factname, (IInterface*)0);
    if ( ii ) return ii;
    IService* is = PluginService::Create<IService*>(factname, name, (ISvcLocator*)0);
    if ( is ) return is;
    IAlgorithm* ia = PluginService::Create<IAlgorithm*>(factname, name, (ISvcLocator*)0);
    if ( ia ) return ia;

    return 0;
  }
  else {
    // DLL library not loaded. Try in the local module
    std::cout << System::getLastErrorString() << std::endl;
    std::cout << "Gaudi::Bootstrap: Not found DLL " << dllname << std::endl;
    return 0; 
  }
}

namespace {
  class ShadowEntry    {
  public:
    std::string dllName;
    std::string facName;
    IFactory*   fac;
  public:
    ShadowEntry() {
    }
    ShadowEntry(const std::string& d, const std::string& n, const IFactory* f) {
      dllName = d;
      facName = n;
      fac     = const_cast<IFactory*>(f);
    }
    ShadowEntry(const ShadowEntry& copy)   {
      dllName = copy.dllName;
      facName = copy.facName;
      fac     = copy.fac;
    }
    ShadowEntry& operator=(const ShadowEntry& copy)   {
      dllName = copy.dllName;
      facName = copy.facName;
      fac     = copy.fac;
      return *this;
    }
  };
}

//------------------------------------------------------------------------------
IAppMgrUI* Gaudi::createApplicationMgr(const std::string& dllname )    {
//------------------------------------------------------------------------------
  return createApplicationMgr(dllname, "ApplicationMgr");
}

//------------------------------------------------------------------------------
IAppMgrUI* Gaudi::createApplicationMgr()    {
//------------------------------------------------------------------------------
  return createApplicationMgr("GaudiSvc", "ApplicationMgr");
}

//=======================================================================
// BootSvcLocator
//=======================================================================

static std::list<IService*> s_bootServices;
static IService*            s_bootService   = 0;
static IInterface*          s_bootInterface = 0;

using Gaudi::BootSvcLocator;

BootSvcLocator::BootSvcLocator() {
  m_refcount = 0;
}
BootSvcLocator::~BootSvcLocator() {
}
unsigned long BootSvcLocator::addRef() {
  m_refcount++;
  return m_refcount;
}
unsigned long BootSvcLocator::release() {
  unsigned long count = --m_refcount;
  if( count <= 0) {
    delete this;
  }
  return count;
}
StatusCode Gaudi::BootSvcLocator::queryInterface(const InterfaceID& iid, void** pinterface)
{
  if( iid == IID_IInterface ) {  
    *pinterface = (IInterface*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  else if ( iid == IID_ISvcLocator ) {
    *pinterface = (ISvcLocator*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  return StatusCode::FAILURE;
}
StatusCode Gaudi::BootSvcLocator::getService( const std::string& name, 
                                       IService*&  svc ) {
  StatusCode sc = StatusCode::FAILURE;
  if ( 0 != s_appmgrInstance ) {
    sc = s_svclocInstance->getService(name, svc );
  } else {
    svc = s_bootService;
  }
  return sc;
}
StatusCode Gaudi::BootSvcLocator::getService( const std::string& name,
                                       const InterfaceID& iid,
                                       IInterface*& pinterface ) {
  StatusCode sc = StatusCode::FAILURE;
  if ( 0 != s_appmgrInstance ) {
    sc = s_svclocInstance->getService(name, iid, pinterface );
  } else {
    pinterface = s_bootInterface;
  }
  return sc;
}
StatusCode Gaudi::BootSvcLocator::getService( const std::string& name,
                                       IService*& svc,
                                       bool createIf ) {
  StatusCode sc = StatusCode::FAILURE;
  if ( 0 != s_appmgrInstance ) {
    sc = s_svclocInstance->getService(name, svc, createIf );
  } else {
    svc = s_bootService;
  }
  return sc;
}
const std::list<IService*>& Gaudi::BootSvcLocator::getServices( ) const {
  StatusCode sc = StatusCode::FAILURE;
  if ( 0 != s_appmgrInstance ) {
    return s_svclocInstance->getServices( );
  } else {
    return s_bootServices;
  }
}
bool Gaudi::BootSvcLocator::existsService( const std::string& name ) const {
  bool result = false;
  if ( 0 != s_appmgrInstance ) {
    result = s_svclocInstance->existsService(name );
  }
  return result;
}
