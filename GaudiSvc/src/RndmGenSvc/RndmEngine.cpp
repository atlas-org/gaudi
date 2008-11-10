//====================================================================
//	Random Engine implementation
//--------------------------------------------------------------------
//
//	Package    : Gaudi/RndmGen ( The LHCb Offline System)
//	Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 29/10/99| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#define GAUDI_RANDOMGENSVC_RndmEngine_CPP

// STL include files
#include <cfloat>

// Framework include files
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "RndmEngine.h"

/// Standard Service constructor
RndmEngine::RndmEngine(const std::string& name, ISvcLocator* loc)
: Service(name, loc), m_pIncidentSvc(0)
{
}

/// Standard Service destructor
RndmEngine::~RndmEngine()   {
  if ( m_pIncidentSvc  ) m_pIncidentSvc->release();
}

/// Query interface
StatusCode RndmEngine::queryInterface(const InterfaceID& riid, void** ppvInterface)  {
  if ( IID_IRndmEngine == riid )   {
    *ppvInterface = (IRndmEngine*)this;
  }
  else if ( IID_ISerialize == riid )   {
    *ppvInterface = (ISerialize*)this;
  }
  else if ( IID_IIncidentListener == riid )   {
    *ppvInterface = (IIncidentListener*)this;
  }
  else  {
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/// Service override: initialisation
StatusCode RndmEngine::initialize()   {
  StatusCode status = Service::initialize();
  if ( status.isSuccess() )   {
    status = setProperties();
    if ( status.isSuccess() )   {
      status = serviceLocator()->service( "IncidentSvc", m_pIncidentSvc, true );
    }
  }
  return status;
}

/// Service override: finalisation
StatusCode RndmEngine::finalize()   {
  StatusCode status = Service::finalize();
  if ( m_pIncidentSvc )     {
    m_pIncidentSvc->release();
  }
  m_pIncidentSvc = 0;
  return status;
}

/** IRndmEngine interface implementation  */
/// Input serialisation from stream buffer. Restores the status of the generator engine.
StreamBuffer& RndmEngine::serialize(StreamBuffer& str)    {
  return str;
}

/// Output serialisation to stream buffer. Saves the status of the generator engine.
StreamBuffer& RndmEngine::serialize(StreamBuffer& str) const    {
  return str;
}

/// Single shot returning single random number
double RndmEngine::rndm() const   {
  return DBL_MAX;
}

/// Single shot returning single random number
void RndmEngine::handle (const Incident& /* inc */ )    {
}

/** Multiple shots returning vector with flat random numbers.
    @param  array    Array containing random numbers 
    @param  howmany  fill 'howmany' random numbers into array
    @param  start    ... starting at position start
    @return StatusCode indicating failure or success.
*/
StatusCode RndmEngine::rndmArray( std::vector<double>& array, long howmany, long start) const   {
  long cnt = start;
  array.resize(start+howmany);
  for ( long i = start, num = start+howmany; i < num; i++ )     {
    array[cnt++] = rndm();
  }
  return StatusCode::SUCCESS;
}

