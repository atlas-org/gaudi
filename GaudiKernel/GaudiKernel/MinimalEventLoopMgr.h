// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/MinimalEventLoopMgr.h,v 1.4 2008/07/15 12:48:18 marcocle Exp $
#ifndef GAUDIKERNEL_MINIMALEVENTLOOPMGR_H
#define GAUDIKERNEL_MINIMALEVENTLOOPMGR_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IEventProcessor.h"

// STL include files
#include <list>
#include <vector>

// Forward declarations
class IAlgorithm;
class IIncidentSvc;
class IIncidentListener;

/** @class MinimalEventLoopMgr
 *  This is the default processing manager of the application manager.
 *  This object handles the minimal requirements needed by the application manager.
 *  It also is capable of handling a bunch of algorithms and output streams.
 *  However, they list may as well be empty.
 *
 *  @author Markus Frank
 *  @version 1.0
 */
class MinimalEventLoopMgr : public Service, 
                            virtual public IEventProcessor
{
public:
  /// Creator friend class
  typedef std::list<IAlgorithm*>   ListAlg;
  typedef std::list<std::string>   ListName;
  typedef std::vector<std::string> VectorName;

protected:
  // enums
  enum State { OFFLINE, CONFIGURED, FINALIZED, INITIALIZED };
  /// Reference to the IAppMgrUI interface of the application manager
  IAppMgrUI*          m_appMgrUI;
  /// Reference to the indicent service
  IIncidentSvc*       m_incidentSvc;
  /// List of top level algorithms
  ListAlg             m_topAlgList;
  /// List of output streams
  ListAlg             m_outStreamList;
  /// Out Stream type
  std::string         m_outStreamType;
  /// List of top level algorithms names
  StringArrayProperty m_topAlgNames;
  /// List of output stream names
  StringArrayProperty m_outStreamNames;
  /// State of the object
  State               m_state;
  /// Scheduled stop of event processing
  bool                m_scheduledStop;
  /// Instance of the incident listener waiting for AbortEvent. 
  IIncidentListener*  m_abortEventListener;
  /// Flag signalling that the event being processedhas to be aborted
  /// (skip all following top algs). 
  bool                m_abortEvent;
  /// Source of the AbortEvent incident. 
  std::string         m_abortEventSource;

public:
  /// Standard Constructor
  MinimalEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~MinimalEventLoopMgr();

protected:

  /// Helper to release interface pointer
  template<class T> T* releaseInterface(T* iface)   {
    if ( 0 != iface ) iface->release();
    return 0;
  }

public:
  /// implementation of IInterface: queryInterface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  /// implementation of IService::initalize
  virtual StatusCode initialize();
  /// implementation of IService::start
  virtual StatusCode start();
  /// implementation of IService::stop
  virtual StatusCode stop();
  /// implementation of IService::finalize
  virtual StatusCode finalize();
  /// implementation of IService::reinitialize
  virtual StatusCode reinitialize();
  /// implementation of IService::restart
  virtual StatusCode restart();

  /// implementation of IEventProcessor::nextEvent
  virtual StatusCode nextEvent(int maxevt);
  /// implementation of IEventProcessor::executeEvent(void* par)
  virtual StatusCode executeEvent(void* par );
  /// implementation of IEventProcessor::executeRun( )
  virtual StatusCode executeRun(int maxevt);
  /// implementation of IEventProcessor::stopRun( )
  virtual StatusCode stopRun();

  /// Top algorithm List handler
  void topAlgHandler( Property& p);
  /// decodeTopAlgNameList & topAlgNameListHandler
  StatusCode decodeTopAlgs();
  /// Output stream List handler
  void outStreamHandler( Property& p);
  /// decodeOutStreamNameList & outStreamNameListHandler
  StatusCode decodeOutStreams();

};
#endif // GAUDIKERNEL_MINIMALEVENTLOOPMGR_H
