// $Id: RndmGenerators.cpp,v 1.7 2006/09/13 15:25:15 hmd Exp $
#define GAUDI_RNDMGENSVC_RNDMGENGENERATORS_CPP

// Framework include files
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/GaudiException.h"

// Standard constructor
Rndm::Numbers::Numbers()
: m_generator(0)
{
}

// Copy constructor
Rndm::Numbers::Numbers(const Rndm::Numbers& copy )
: m_generator(copy.m_generator)   {
  if ( 0 != m_generator )   {
    m_generator->addRef();
  }
}

// Construct and initialize the generator
Rndm::Numbers::Numbers(IRndmGenSvc* svc, const IRndmGen::Param& par)  
: m_generator(0) 
{
  StatusCode status = initialize(svc, par);
  if (!status.isSuccess()) {
    throw GaudiException ("Initialization failed !", "Rndm::Numbers", status);
  }
}

// Standard destructor
Rndm::Numbers::~Numbers()    {
  finalize().ignore();
}

// Initialize the generator
StatusCode Rndm::Numbers::initialize(IRndmGenSvc* svc,
                                     const IRndmGen::Param& par)  {
  if ( 0 != svc && 0 == m_generator )   {
    StatusCode status = svc->generator( par, m_generator );
    return status;
  }
  return StatusCode::FAILURE;
}

// Initialize the generator
StatusCode Rndm::Numbers::finalize()   {
  if ( 0 != m_generator )   {
    m_generator->finalize().ignore();
    m_generator->release();
    m_generator = 0;
  }
  return StatusCode::SUCCESS;
}

