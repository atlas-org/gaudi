//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::PersSvc
//
// Author :                   Pavel Binko
//
//------------------------------------------------------------------------------
// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/PersSvc.cpp,v 1.13 2006/01/10 20:11:14 hmd Exp $
#define HBOOKCNV_PERSSVC_CPP

// Include files
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ISvcLocator.h"

#include "HbookDef.h"
#include "PersSvc.h"

#include <iostream>
#include <string>

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(HbookCnv,PersSvc)
 
static std::string stat_dir = "/stat";
static std::string undefFileName = "UndefinedHbookOutputFileName";


static long s_dataTypePolicy = FLOAT_ONLY;
long dataTypePolicy()   {
  return s_dataTypePolicy;
}

/// Initialize the service.
StatusCode HbookCnv::PersSvc::initialize()     {
  StatusCode status = ConversionSvc::initialize();
  if ( status.isSuccess() ) {
    MsgStream log( messageService(), name() );

    // Report on size properties
    log << MSG::INFO << "Setting /PAWC/ common block size to "
        << m_NPAWC << endreq;
    if (m_NREC > 0) {
        log << MSG::INFO << "Setting IQUEST(10) to " << m_NREC << endreq;
    }

    if (m_RECL != 1024) {
        log << MSG::INFO << "Setting Record Length to " << m_RECL << endreq;
    }

    // Return an error if output file has not been specified
    if( undefFileName == m_defFileName ) {
      log << MSG::WARNING << "HBOOK output file name missing!!" << endreq;
      INIT_HBOOK( m_NPAWC );
    }
    else {
      INIT_HBOOK( stat_dir.substr(1,stat_dir.size()), m_defFileName,
                  m_NPAWC, m_NREC, m_RECL );
    }

    if ( ::toupper(m_rowWisePolicy[0]) == 'U' )   {
      s_dataTypePolicy = USE_DATA_TYPES;
    }
    else if ( ::toupper(m_rowWisePolicy[0]) == 'F' )   {
      if ( ::toupper(m_rowWisePolicy[1]) == 'O' )   {
        s_dataTypePolicy = FORTRAN_STYLE;
      }
      else    {
        s_dataTypePolicy = FLOAT_ONLY;
      }
    }
    else if ( ::toupper(m_rowWisePolicy[0]) == 'H' )   {
      s_dataTypePolicy = HUNGARIAN_STYLE;
    }
  }
  return status;
}

/// Stop the service.
StatusCode HbookCnv::PersSvc::finalize()   {
  // Close HBOOK only if the HBOOK output file name is defined
  if( undefFileName != m_defFileName ) {
    CLOSE_HBOOK( stat_dir.substr(1,stat_dir.size()), m_defFileName );
  }
  return ConversionSvc::finalize();
}

/// Convert a transient data object into its persistent representation
StatusCode HbookCnv::PersSvc::createRep(DataObject* pObject,
                                        IOpaqueAddress*& refpAddr)   {

  StatusCode sc = StatusCode::SUCCESS;

  // Convert histograms only if HBOOK output file name is defined
  if( undefFileName != m_defFileName ) { // Valid HBOOK output file name

    // Manipulate global output level to switch on/off histogram printing
    IProperty* msgProp;
    msgSvc()->queryInterface( IID_IProperty, (void**)&msgProp ); 
    std::string dfltLevel;
    StatusCode scl = msgProp->getProperty( "OutputLevel", dfltLevel );
    if ( m_histDo && scl.isSuccess() ) {
      msgProp->setProperty( "OutputLevel", "3" );
    }
    else {
      msgProp->setProperty( "OutputLevel", "4" );
    }

    //Save the histograms
    sc = ConversionSvc::createRep(pObject, refpAddr);

    // Reset the global output level to its previous value
    if( scl.isSuccess() ) msgProp->setProperty( "OutputLevel", dfltLevel );

  }
  return sc;
}

/// Update references of the persistent data representation
StatusCode HbookCnv::PersSvc::fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObject)   {
  // Convert histograms only if HBOOK output file name is defined
  if( undefFileName != m_defFileName ) { // Valid HBOOK output file name
    return ConversionSvc::fillRepRefs(pAddr, pObject);
  }
  return StatusCode::SUCCESS;
}

/// Standard Constructor
HbookCnv::PersSvc::PersSvc(const std::string& name, ISvcLocator* svc)
: ConversionSvc(name, svc, HBOOK_StorageType)
{
  
  declareProperty("OutputFile", m_defFileName = undefFileName);
  declareProperty("PersistencySvc", m_persistencySvc = "HistogramPersistencySvc");
  declareProperty("RowWiseNtuplePolicy", m_rowWisePolicy = "FLOAT_ONLY");
  declareProperty("PrintHistos", m_histDo = false );
  declareProperty("NPAWC",m_NPAWC = 250000);  // size of PAWC common block
  declareProperty("IQUEST10",m_NREC = -1);    // IQUEST(10)
  declareProperty("RecordLength",m_RECL = 1024);  // Record length in HROPEN
}

/// Standard Destructor
HbookCnv::PersSvc::~PersSvc()                                                 {
}
