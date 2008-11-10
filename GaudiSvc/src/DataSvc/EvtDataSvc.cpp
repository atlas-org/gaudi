//====================================================================
//	EvtDataSvc.cpp
//--------------------------------------------------------------------
//
//	Package    : System ( The LHCb Offline System)
//
//  Description: implementation of the Transient event data service.
//
//	Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 29/10/98| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#define  DATASVC_EVTDATASVC_CPP

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"

#include "EvtDataSvc.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(EvtDataSvc)

/// Service initialisation
StatusCode EvtDataSvc::initialize()    {
  // Nothing to do: just call base class initialisation
  StatusCode      status  = DataSvc::initialize();
  ISvcLocator*    svc_loc = serviceLocator();

  // Attach data loader facility
  status = svc_loc->service("EventPersistencySvc", m_cnvSvc, true);
  status = setDataLoader( m_cnvSvc );
  return status;
}
/// Service reinitialisation
StatusCode EvtDataSvc::reinitialize()    {
  // Do nothing for this service
  return StatusCode::SUCCESS;
}
/// Service finalization
StatusCode EvtDataSvc::finalize()    {

  if( m_cnvSvc ) m_cnvSvc->release();
  m_cnvSvc = 0;
  DataSvc::finalize().ignore();
  return StatusCode::SUCCESS ;
}

/// Standard Constructor
EvtDataSvc::EvtDataSvc(const std::string& name,ISvcLocator* svc) 
: DataSvc(name,svc)   {
  m_cnvSvc = 0;
}

/// Standard Destructor
EvtDataSvc::~EvtDataSvc()  {
}
