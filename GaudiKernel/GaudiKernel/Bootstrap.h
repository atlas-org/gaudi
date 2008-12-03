// $Header: /local/reps/Gaudi/GaudiKernel/GaudiKernel/Bootstrap.h,v 1.8 2006/11/30 14:43:12 mato Exp $
#ifndef GAUDIKERNEL_BOOTSTRAP_H
#define GAUDIKERNEL_BOOTSTRAP_H

// Include files
#include <string>

// Framework include files
#include "GaudiKernel/Kernel.h"

// Forward declarations
class IAppMgrUI;
class ISvcLocator;
class IInterface;
class IFactory;

/** @class Gaudi Bootstrap.h GaudiKernel/Bootstrap.h

    This is a number of static methods for bootstrapping the Gaudi framework

    @author Pere Mato
*/ 

namespace  Gaudi {
  
  IAppMgrUI* createApplicationMgrEx(const std::string& dllname , 
				    const std::string& factname );
  IAppMgrUI* createApplicationMgr(const std::string& dllname , 
				  const std::string& factname );
  IAppMgrUI* createApplicationMgr(const std::string& dllname );
  IAppMgrUI* createApplicationMgr();
  IInterface* createInstance(const std::string& name, 
			     const std::string& factname, 
			     const std::string& ddlname);
  ISvcLocator* svcLocator();
  /** Set new instance of service locator.
      @param    newInstance  Current instance of service locator.
      @return   Previous instance of service locator object.
  */
  ISvcLocator* setInstance(ISvcLocator* newInstance);
  /** Set new instance of service locator.
      @param    newInstance  Current instance of service locator.
      @return   Previous instance of service locator object.
  */
  IAppMgrUI* setInstance(IAppMgrUI* newInstance);
  bool hasApplicationMgr();

}

#endif // GAUDIKERNEL_BOOTSTRAP_H
