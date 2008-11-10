// $Id: OutputFSRStream.cpp,v 1.1 2008/11/04 22:49:25 marcocle Exp $
#define GAUDISVC_PERSISTENCYSVC_OUTPUTFSRSTREAM_CPP

// Framework include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "OutputFSRStream.h"

// Define the algorithm factory for the standard output data writer
DECLARE_ALGORITHM_FACTORY(OutputFSRStream)

// Standard Constructor
OutputFSRStream::OutputFSRStream(const std::string& name, ISvcLocator* pSvcLocator)
 : OutputStream(name, pSvcLocator)
{
  m_FSRItemNames.clear();
  m_FSROptItemNames.clear();

  declareProperty( "FSRItemList",    m_FSRItemNames,    "Mandatory items to be written as FSRs" );
  declareProperty( "FSROptItemList", m_FSROptItemNames, "Optional  items to be written as FSRs" );
}

// Standard Destructor
OutputFSRStream::~OutputFSRStream()   {
}

StatusCode OutputFSRStream::stop() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Set up File Summary Record" << endmsg;

  // Clear last real event from TES, and prepare it to receive FSR objects
  StatusCode sc = m_pDataManager->setRoot ("/Event", new DataObject());
  if( !sc.isSuccess() )  {
    log << MSG::WARNING << "Error declaring event root DataObject" << endmsg;
  }
  return OutputStream::stop();
}

StatusCode OutputFSRStream::finalize() {

  MsgStream log(msgSvc(), name());

  // Clear any previously existing item list
  clearSelection();

  // Clear the list with optional items
  clearItems(m_optItemList);
  // Clear the item list
  clearItems(m_itemList);

  ItemNames::iterator i;
  // Take the new item list from the FSRItemList properties
  for(i = m_FSRItemNames.begin(); i != m_FSRItemNames.end(); i++)   {
    addItem( m_itemList, *i );
  }
  for(i = m_FSROptItemNames.begin(); i != m_FSROptItemNames.end(); i++)   {
    addItem( m_optItemList, *i );
  }

  // And write them out
  log << MSG::INFO << "Write File Summary Record" << endmsg;
  StatusCode sc = writeObjects();
  if( sc.isFailure() ) {
    // Log the error, but don't fail finalize!
    log << MSG::ERROR << "Unable to write File Summary Record" << endmsg;
  }

  return OutputStream::finalize();
}
