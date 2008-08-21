// $Id: AppMgrRunable.h,v 1.3 2006/11/30 14:57:02 mato Exp $
#ifndef GAUDISVC_APPMGRRUNABLE_H
#define GAUDISVC_APPMGRRUNABLE_H 1

// Framework includes
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"

// Forward declarations
class IAppMgrUI;
template <class T> class SvcFactory;

/** Class definition of AppMgrRunable.
    This is the default runnable of the application manager.
    The runable handles the actual run() implementation of the
    ApplicationMgr object.
    This implementation just calls ApplicationMgr::nextEvent(...).

    <B>History:</B>
    <PRE>
    +---------+----------------------------------------------+---------+
    |    Date |                 Comment                      | Who     |
    +---------+----------------------------------------------+---------+
    |13/12/00 | Initial version                              | M.Frank |
    +---------+----------------------------------------------+---------+
    </PRE>
   @author Markus Frank
   @version 1.0
*/
class AppMgrRunable : public Service, virtual public IRunable    {
  /// Creator friend
  friend class SvcFactory<AppMgrRunable>;

protected:
  /// Reference to application manager UI
  IAppMgrUI*    m_appMgrUI;
  /// Number of events to be processed
  int           m_evtMax;

public:
  /// Standard Constructor
  AppMgrRunable(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~AppMgrRunable();

  // IInterface implementation : queryInterface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  /// IService implementation: initialize the service
  virtual StatusCode initialize();
  /// IService implementation: finalize the service
  virtual StatusCode finalize();
  /// IRunable implementation : Run the class implementation
  virtual StatusCode run();
};
#endif // GAUDISVC_APPMGRRUNABLE_H
