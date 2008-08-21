// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiSvc/src/IncidentSvc/IncidentSvc.h,v 1.5 2007/05/24 17:05:07 marcocle Exp $
#ifndef IncidentSvc_IncidentSvc_H
#define IncidentSvc_IncidentSvc_H

// Include Files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <map>
#include <list>

// Forward declarations

//------------------------------------------------------------------
//
// ClassName:   IncidentSvc
//  
// Description: This service manages Auditors. 
//              Auditors can be inserted in the system to audit the Algorithms is 
//              in their functions.
//------------------------------------------------------------------
class IncidentSvc : public Service, virtual public IIncidentSvc {

public: 

  struct Listener {
    IIncidentListener* iListener;
    long priority;
    bool rethrow;
    bool singleShot;

    Listener(IIncidentListener* il, long pri, bool thr=false, bool single=false): 
      iListener(il), priority(pri), rethrow(thr), singleShot(single){}

  };
  
// Typedefs
//  typedef std::pair<IIncidentListener*,long> Listener;
  typedef std::list<Listener> ListenerList;
  typedef std::map<std::string, ListenerList*> ListenerMap;

// Inherited Service overrides:
//
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
    
// IIncidentSvc interfaces overwrite
// 
  virtual void addListener(IIncidentListener* lis, const std::string& type = "",
			                     long priority = 0, bool rethrow=false, bool singleShot=false);
  virtual void removeListener(IIncidentListener* lis, const std::string& type = "");
  virtual void fireIncident( const Incident& incident );
 
  // Standard Constructor.
  IncidentSvc( const std::string& name, ISvcLocator* svc );
  // Destructor.
  virtual ~IncidentSvc();

private:
  // List of auditor names
  ListenerMap  m_listenerMap;
  /// Incident being fired. It is used to know if we can safely remove a listener or
  /// we have to schedule its removal for later.
  const std::string *m_currentIncidentType;
};

#endif


