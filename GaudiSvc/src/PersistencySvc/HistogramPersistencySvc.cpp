//====================================================================
//	HistogramPersistencySvc.cpp
//--------------------------------------------------------------------
//
//	Package    : System ( The LHCb Offline System)
//
//  Description: implementation of the Event data persistency service
//               This specialized service only deals with event related 
//               data
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
#define  PERSISTENCYSVC_HISTOGRAMPERSISTENCYSVC_CPP

// Implementation specific definitions
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "HistogramPersistencySvc.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_SERVICE_FACTORY(HistogramPersistencySvc)

/// Finalize the service.
StatusCode HistogramPersistencySvc::finalize()     {
  StatusCode status = PersistencySvc::finalize();
  return status;
}

/// Initialize the service.
StatusCode HistogramPersistencySvc::initialize()     {
  StatusCode status = PersistencySvc::initialize();
  if ( status.isSuccess() )   {
    status = reinitialize();
  }
  return status;
}

/// Reinitialize the service.
StatusCode HistogramPersistencySvc::reinitialize()
{
  MsgStream log(msgSvc(), name());
  // Obtain the IProperty of the ApplicationMgr
  SmartIF<IProperty> prpMgr(serviceLocator());
  if ( !prpMgr.isValid() )   {
    log << MSG::FATAL << "IProperty interface not found in ApplicationMgr." << endreq;
    return StatusCode::FAILURE;
  }
  else {
    setProperty(prpMgr->getProperty("HistogramPersistency")).ignore();
  }

  // To keep backward compatibility, we set the property of conversion service
  // into JobOptions catalogue
  if( m_outputFile != "" ) {
    IJobOptionsSvc* joptsvc;
    if( serviceLocator()->service("JobOptionsSvc", joptsvc ).isSuccess() ) {
      StringProperty p("OutputFile", m_outputFile);
      if ( m_histPersName == "ROOT" ) {
        joptsvc->addPropertyToCatalogue("RootHistSvc", p).ignore();
      } else if (m_histPersName == "HBOOK" ) {
        joptsvc->addPropertyToCatalogue("HbookHistSvc", p).ignore();
      }
      joptsvc->release();
    }
  }

  // Load the Histogram persistency service that's required as default
  setConversionSvc(0).ignore();
  if ( m_histPersName == "ROOT" ) {
    setConversionSvc(service("RootHistSvc")).ignore();
    if ( !conversionSvc() ) {
      return StatusCode::FAILURE;
    }
    enable(true);
  }
  else if ( m_histPersName == "HBOOK" ) {
    setConversionSvc(service("HbookHistSvc")).ignore();
    if ( !conversionSvc() ) {
      return StatusCode::FAILURE;
    }
    enable(true);
  }
  else if ( m_histPersName == "NONE" ) {
    enable(false);
    log << MSG::WARNING << "Histograms saving not required." << endreq;
  }
  else {
    setConversionSvc(service(m_histPersName)).ignore();
    if ( !conversionSvc() ) {
      return StatusCode::FAILURE;
    }
    enable(true);
    log << MSG::WARNING << "Unknown Histogram Persistency Mechanism " << m_histPersName << endreq;
  }
  return StatusCode::SUCCESS;
}

/// Convert the transient object to the requested representation.
StatusCode HistogramPersistencySvc::createRep(DataObject* pObj, IOpaqueAddress*& refpAddr)  {
  if ( m_histPersName != "NONE" ) {
    enable(true);
  }
  return PersistencySvc::createRep(pObj, refpAddr);
}

/// Standard Constructor
HistogramPersistencySvc::HistogramPersistencySvc(const std::string& name, ISvcLocator* svc)
 :  PersistencySvc(name, svc)
{
  std::vector<std::string> defServices;
  defServices.push_back("HbookHistSvc");
  defServices.push_back("RootHistSvc");
  m_svcNames.set(defServices);
  declareProperty("HistogramPersistency", m_histPersName = "");
  declareProperty("OutputFile", m_outputFile = "");
}

/// Standard Destructor
HistogramPersistencySvc::~HistogramPersistencySvc()   {
}
