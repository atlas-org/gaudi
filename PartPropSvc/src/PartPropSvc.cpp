//Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PathResolver.h"

#include "PartPropSvc.h"

#include "HepPDT/TableBuilder.hh"

#include <iostream>
#include <cstdlib>
#include <fstream>

// Instantiation of a static factory class used by clients to create
//  instances of this service
DECLARE_SERVICE_FACTORY(PartPropSvc)

//*************************************************************************//

PartPropSvc::PartPropSvc( const std::string& name, ISvcLocator* svc )
  : Service( name, svc ), m_pdt(0) {

  declareProperty( "InputType", m_inputType="PDG");
  declareProperty( "InputFile", m_pdtFiles);

  if (m_pdtFiles.empty() ) {
    m_pdtFiles.push_back("PDGTABLE.MeV");
  }

}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

PartPropSvc::~PartPropSvc() {
  if (m_pdt != 0) {
    delete m_pdt;
    m_pdt = 0;
  }
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

const InterfaceID& PartPropSvc::type() const { 
  return IID_IPartPropSvc; 
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

StatusCode PartPropSvc::initialize() {

  MsgStream log( msgSvc(), name() );
  std::vector<std::string>::const_iterator itr;

  StatusCode status = Service::initialize();

  log << MSG::INFO << "PDT file(s): " << endreq;
  for (itr=m_pdtFiles.begin(); itr!=m_pdtFiles.end(); ++itr) {
    log << MSG::INFO << "    " << *itr << endreq;
  }
  log << MSG::INFO << "Type:     " << m_inputType << endreq;

  if ( status.isFailure() ) {
    log << MSG::ERROR << "Could not initialize main svc" << endreq;
    return StatusCode::FAILURE;
  }

  bool (*pF)  (std::istream &, 
	       HepPDT::TableBuilder &);

  
  // Determine type of input
  if (m_inputType == "PDG") {
    pF = &HepPDT::addPDGParticles;
  } else if (m_inputType == "Pythia") {
    pF = &HepPDT::addPythiaParticles;
  } else if (m_inputType == "EvtGen") {
    pF = &HepPDT::addEvtGenParticles;
  } else if (m_inputType == "Herwig") {
    pF = &HepPDT::addHerwigParticles;
  } else if (m_inputType == "IsaJet") {
    pF = &HepPDT::addIsajetParticles;
  } else if (m_inputType == "QQ") {
    pF = &HepPDT::addQQParticles;
  } else {
    log << MSG::ERROR << "Unknown Particle Data file type: \"" 
	<< m_inputType << "\"" << endreq;
    return StatusCode::FAILURE;
  }

  // Make sure we have at least one file
  if (m_pdtFiles.size() == 0) {
    log << MSG::ERROR << "Need at least 1 PDT file" << endreq;
    log << MSG::ERROR << "Check value of property \"InputFile\"" << endreq;
    return StatusCode::FAILURE;
  }

  m_pdt = new HepPDT::ParticleDataTable;

  {
    // Construct table builder
    HepPDT::TableBuilder  tb( *m_pdt );

    // read the input 
    int good(0);
    for (itr=m_pdtFiles.begin(); itr!=m_pdtFiles.end(); ++itr) {

      std::string rfile = System::PathResolver::find_file(*itr,"DATAPATH");
      if (rfile == "") {
	log << MSG::ERROR << "Could not find PDT file: \"" << *itr
	    << "\" in $DATAPATH" << endreq;
	continue;
      }

      std::ifstream pdfile( rfile.c_str() );
      if (!pdfile) {
	log << MSG::ERROR << "Could not open PDT file: \"" << rfile
	    << "\"" << endreq;
	continue;
      }

      if ( ! pF(pdfile,tb) ) {
	log << MSG::ERROR << "Error reading PDT file: \"" << rfile
	    << "\"" << endreq;
	return StatusCode::FAILURE;
      } 
      ++good;
    }
    if (0 == good) {
      log << MSG::ERROR << "Unable to access any PDT file" <<endreq;
      return StatusCode::FAILURE;
    }

  }   // the tb destructor fills datacol


  
  return status;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

StatusCode PartPropSvc::finalize() {

  MsgStream log( msgSvc(), name() );
  StatusCode status = Service::finalize();

  if ( status.isSuccess() )
    log << MSG::INFO << "Service finalised successfully" << endreq;
  
  return status;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//

StatusCode PartPropSvc::queryInterface( const InterfaceID& riid, 
					void** ppvInterface ) {
  StatusCode sc = StatusCode::FAILURE;
  if ( ppvInterface ) {
    *ppvInterface = 0;
    
    if ( riid == IID_IPartPropSvc ) {
      *ppvInterface = static_cast<IPartPropSvc*>(this);
      sc = StatusCode::SUCCESS;
      addRef();
    }
    else
      sc = Service::queryInterface( riid, ppvInterface );    
  }
  return sc;
}

//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *//
