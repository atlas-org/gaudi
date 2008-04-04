// $Id: MTEventLoopMgr.cpp,v 1.5 2006/11/30 10:35:27 mato Exp $
#define  GAUDISVC_MTEVENTLOOPMGR_CPP

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"

#include "src/ApplicationMgr/HistogramAgent.h"
#include "MTEventLoopMgr.h"

// Instantiation of a static factory class used by clients to create instances of this service

DECLARE_SERVICE_FACTORY(MTEventLoopMgr);

//--------------------------------------------------------------------------------------------
// Standard Constructor
//--------------------------------------------------------------------------------------------
MTEventLoopMgr::MTEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc)
: MinimalEventLoopMgr(nam, svcLoc)
{
  m_histoDataMgrSvc   = 0;
  m_histoPersSvc      = 0;
  m_incidentSvc       = 0;
  m_evtDataMgrSvc     = 0;
  m_evtDataSvc        = 0;
  m_evtSelector       = 0;
  m_evtCtxt           = 0;
  m_total_nevt        = 0;
  
  // Declare properties
  declareProperty("HistogramPersistency", m_histPersName = "");
  declareProperty( "EvtSel", m_evtsel );
}

//--------------------------------------------------------------------------------------------
// Standard Destructor
//--------------------------------------------------------------------------------------------
MTEventLoopMgr::~MTEventLoopMgr()   {
  if( m_histoDataMgrSvc ) m_histoDataMgrSvc->release();
  if( m_histoPersSvc ) m_histoPersSvc->release();
  if( m_incidentSvc ) m_incidentSvc->release();
  if( m_evtDataMgrSvc ) m_evtDataMgrSvc->release();
  if( m_evtDataSvc ) m_evtDataSvc->release();
  if( m_evtSelector ) m_evtSelector->release();
  if( m_evtCtxt ) delete m_evtCtxt;
}

//--------------------------------------------------------------------------------------------
// implementation of IAppMgrUI::initalize
//--------------------------------------------------------------------------------------------
StatusCode MTEventLoopMgr::initialize()    {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ---> MTEventLoopMgr = " << name() << " initializing " << endreq; 
  // initilaize the base class
  StatusCode sc = MinimalEventLoopMgr::initialize();

  if( sc.isFailure() ) {
    log << MSG::DEBUG << "Error Initializing base class MinimalEventLoopMgr." << endreq;
    return sc;
  }

  // Setup access to event data services
  sc = service("EventDataSvc", m_evtDataMgrSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving EventDataSvc interface IDataManagerSvc." 
        << endreq;
    return sc;
  }
  sc = service("EventDataSvc", m_evtDataSvc, false);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving EventDataSvc interface IDataProviderSvc." 
        << endreq;
    return sc;
  }

  // Get the references to the services that are needed by the ApplicationMgr itself
  sc = service("IncidentSvc", m_incidentSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving IncidentSvc" << endreq;
    return sc;
  }

  // Obtain the IProperty of the ApplicationMgr
  SmartIF<IProperty> prpMgr(IID_IProperty, serviceLocator());
  if ( ! prpMgr.isValid() )   {
    log << MSG::FATAL << "IProperty interface not found in ApplicationMgr." << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_appMgrProperty = prpMgr;
  }

  // We do not expect a Event Selector necessarily being declared
  setProperty(m_appMgrProperty->getProperty("EvtSel"));

  if( m_evtsel != "NONE" || m_evtsel.length() == 0) {
    sc = service( "EventSelector", m_evtSelector, true );
    if( sc.isSuccess() )     {
      // Setup Event Selector
      sc = m_evtSelector->createContext(m_evtCtxt);
      if ( !sc.isSuccess() )  {
        log << MSG::FATAL << "Failed to create EventSelector context." << endreq;
        return sc;
      }
    }
    else {
      log << MSG::FATAL << "EventSelector not found." << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    m_evtSelector = 0;
    m_evtCtxt = 0;
    log << MSG::WARNING << "Unable to locate service \"EventSelector\" " << endreq;    
    log << MSG::WARNING << "No events will be processed from external input." << endreq;    
  }

  // Setup access to histogramming services
  sc = service("HistogramDataSvc", m_histoDataMgrSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving HistogramDataSvc" << endreq;
    return sc;
  }
  // Setup histogram persistency
  sc = service("HistogramPersistencySvc", m_histoPersSvc, true);
  if( !sc.isSuccess() ) {
    log << MSG::WARNING << "Histograms cannot not be saved - though required." << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------------------------
// implementation of IService::reinitialize
//--------------------------------------------------------------------------------------------
StatusCode MTEventLoopMgr::reinitialize() {

  // initilaize the base class
  StatusCode sc = MinimalEventLoopMgr::reinitialize();

  MsgStream log(msgSvc(), name());
  if( sc.isFailure() ) {
    log << MSG::DEBUG << "Error Initializing base class MinimalEventLoopMgr." << endreq;
    return sc;
  }

  // Check to see whether a new Event Selector has been specified
  setProperty(m_appMgrProperty->getProperty("EvtSel"));
  if( m_evtsel != "NONE" || m_evtsel.length() == 0) {
    IEvtSelector* theEvtSel;
    IService*     theSvc;
    sc = service( "EventSelector", theEvtSel );
    sc = service( "EventSelector", theSvc );
    if( sc.isSuccess() && ( theEvtSel != m_evtSelector ) ) {
      // Setup Event Selector
      m_evtSelector = theEvtSel;
      if (theSvc->state() == IService::INITIALIZED) {
        sc = theSvc->reinitialize();
        if( sc.isFailure() ){
          log << MSG::ERROR << "Failure Reinitializing EventSelector "
              << theSvc->name( ) << endreq;
          return sc;
        }
      } else {
        sc = theSvc->initialize();
        if( sc.isFailure() ){
          log << MSG::ERROR << "Failure Initializing EventSelector "
              << theSvc->name( ) << endreq;
          return sc;
        }
      }
      sc = theEvtSel->createContext(m_evtCtxt);
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Can not create Context "
            << theSvc->name( ) << endreq;
        return sc;
      }
      log << MSG::INFO << "EventSelector service changed to "
          << theSvc->name( ) << endreq;
    }
  }
  else {
    m_evtSelector = 0;
    m_evtCtxt = 0;    
  }
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------------------
// implementation of IAppMgrUI::finalize
//--------------------------------------------------------------------------------------------
StatusCode MTEventLoopMgr::finalize()    {
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " Number of events processed : " << m_total_nevt << endreq;                                  

  // Finalize base class
  MinimalEventLoopMgr::finalize();

  // Save Histograms Now
  if ( 0 != m_histoPersSvc )    {
    HistogramAgent agent;
    sc = m_histoDataMgrSvc->traverseTree( &agent );
    if( sc.isSuccess() )   {
      IDataSelector* objects = agent.selectedObjects();
      // skip /stat entry!
      if ( objects->size() > 0 )    {
        IDataSelector::iterator i;
        for ( i = objects->begin(); i != objects->end(); i++ )    {
          IOpaqueAddress* pAddr = 0;
          StatusCode iret = m_histoPersSvc->createRep(*i, pAddr);
          if ( iret.isSuccess() )     {
            (*i)->registry()->setAddress(pAddr);
          }
          else  {
            sc = iret;
          }
        }
        for ( i = objects->begin(); i != objects->end(); i++ )    {
          IRegistry* reg = (*i)->registry();
          StatusCode iret = m_histoPersSvc->fillRepRefs(reg->address(), *i);
          if ( !iret.isSuccess() )    {
            sc = iret;
          }
        }
      }
      if ( sc.isSuccess() )    {
        log << MSG::INFO << "Histograms converted successfully according to request." << endreq;
      }
      else  {
        log << MSG::ERROR << "Error while saving Histograms." << endreq;
      }
    }
    else {
      log << MSG::ERROR << "Error while traversing Histogram data store" << endreq;
    }
  }
  if ( 0 != m_evtCtxt && 0 != m_evtSelector )  {
    m_evtSelector->releaseContext(m_evtCtxt);
    m_evtCtxt = 0;
  }
  // Release all interfaces...
  m_histoDataMgrSvc = releaseInterface(m_histoDataMgrSvc);
  m_histoPersSvc    = releaseInterface(m_histoPersSvc);

  m_evtSelector     = releaseInterface(m_evtSelector);
  m_incidentSvc     = releaseInterface(m_incidentSvc);
  m_evtDataSvc      = releaseInterface(m_evtDataSvc);
  m_evtDataMgrSvc   = releaseInterface(m_evtDataMgrSvc);

  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------------------------
// implementation of IAppMgrUI::nextEvent
//--------------------------------------------------------------------------------------------
StatusCode MTEventLoopMgr::nextEvent(int maxevt)   {
  DataObject*       pObject = 0;
  StatusCode        sc;

  // loop over events if the maxevt (received as input) if different from -1. 
  // if evtmax is -1 it means infinite loop
  for( int nevt = 0; (maxevt == -1 ? true : nevt < maxevt);  nevt++, m_total_nevt++) {
    // Clear the event store, if used in the event loop
    if( 0 != m_total_nevt ) {
      sc = m_evtDataMgrSvc->clearStore();
      if( !sc.isSuccess() )  {
        MsgStream log( msgSvc(), name() );
        log << MSG::DEBUG << "Clear of Event data store failed" << endreq;
      }
    }

    // Setup event in the event store
    if( m_evtCtxt ) {
      IOpaqueAddress* addr = 0;
      // Only if there is a EventSelector
      sc = getEventRoot(addr);
      if( !sc.isSuccess() )  {
        MsgStream log( msgSvc(), name() );
        log << MSG::INFO << "No more events in event selection " << endreq;
        break;
      }
      // Set root clears the event data store first
      sc = m_evtDataMgrSvc->setRoot ("/Event", addr);
      if( !sc.isSuccess() )  {
        MsgStream log( msgSvc(), name() );
        log << MSG::WARNING << "Error declaring event root address." << endreq;
        continue;
      }
      sc = m_evtDataSvc->retrieveObject("/Event", pObject);
      if( !sc.isSuccess() ) {
        MsgStream log( msgSvc(), name() );
        log << MSG::WARNING << "Unable to retrieve Event root object" << endreq;
        break;
      }
    }
    else {
      sc = m_evtDataMgrSvc->setRoot ("/Event", new DataObject());
      if( !sc.isSuccess() )  {
        MsgStream log( msgSvc(), name() );
        log << MSG::WARNING << "Error declaring event root DataObject" << endreq;
      } 
    }
    // Execute event for all required algorithms

    // Fire BeginEvent "Incident"
    m_incidentSvc->fireIncident(Incident(name(),IncidentType::BeginEvent));
    // Execute Algorithms
    StatusCode sc = executeEvent(NULL);
    // Fire EndEvent "Incident"
    m_incidentSvc->fireIncident(Incident(name(),IncidentType::EndEvent));

    if( !sc.isSuccess() ){
      MsgStream log( msgSvc(), name() );
      log << MSG::ERROR << "Terminating event processing loop due to errors" << endreq;
      break;
    }
  }

  return StatusCode::SUCCESS;
}


/// Create event address using event selector
StatusCode MTEventLoopMgr::getEventRoot(IOpaqueAddress*& refpAddr)  {
  refpAddr = 0;
  StatusCode sc = m_evtSelector->next(*m_evtCtxt);
  if ( !sc.isSuccess() )  {
    return sc;
  }
  // Create root address and assign address to data service
  sc = m_evtSelector->createAddress(*m_evtCtxt,refpAddr);
  if( !sc.isSuccess() )  {
    sc = m_evtSelector->next(*m_evtCtxt);
    if ( sc.isSuccess() )  {
      sc = m_evtSelector->createAddress(*m_evtCtxt,refpAddr);
      if ( !sc.isSuccess() )  {
        MsgStream log( msgSvc(), name() );
        log << MSG::WARNING << "Error creating IOpaqueAddress." << endreq;
      }
    }
  }
  return sc;
}
