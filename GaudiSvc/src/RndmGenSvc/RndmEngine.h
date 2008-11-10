//====================================================================
//	Random Engine definition
//--------------------------------------------------------------------
//
//	Package    : Gaudi/RndmGenSvc ( The LHCb Offline System)
//	Author     : M.Frank
//====================================================================
#ifndef GAUDI_RNDMGENSVC_RNDMENGINE_H
#define GAUDI_RNDMGENSVC_RNDMENGINE_H 1

// STL include files
#include <vector>

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/IIncidentListener.h"

// Forward declarations
class IIncidentSvc;

/** Random Generator engine definition

    Description:
    Definition of a interface for a generic random number generator
    giving randomly distributed numbers in the range [0...1]
    number generators.

    Dependencies:
    <UL>
    <LI> Generic Service interface: Gaudi/Interfaces/IRndmEngine.h
    </UL>

    <P> History    :
    <PRE>
    +---------+----------------------------------------------+--------+
    |    Date |                 Comment                      | Who    |
    +---------+----------------------------------------------+--------+
    | 21/04/99| Initial version.                             | MF     |
    +---------+----------------------------------------------+--------+
    </PRE>
    Author:  M.Frank
    Version: 1.0
*/
class RndmEngine: public Service, 
                  virtual public IRndmEngine, 
                  virtual public ISerialize,
                  virtual public IIncidentListener
{
protected:
  /// Reference to the incident service
  IIncidentSvc*        m_pIncidentSvc;

  /// Standard Constructor
  RndmEngine(const std::string& name, ISvcLocator* loc);
  /// Standard Destructor
  virtual ~RndmEngine();
public:
  /// Query interface
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
  /// Service override: initialisation
  virtual StatusCode initialize();
  /// Service override: finalisation
  virtual StatusCode finalize();
  /// Single shot returning single random number
  virtual double rndm() const;
  /** Multiple shots returning vector with flat random numbers.
      @param  array    Array containing random numbers 
      @param  howmany  fill 'howmany' random numbers into array
      @param  start    ... starting at position start
      @return StatusCode indicating failure or success.
  */
  virtual StatusCode rndmArray( std::vector<double>& array, long howmany, long start = 0) const;
  /** Input serialisation from stream buffer. Restores the status of the object.
      @param   str    Reference to Streambuffer to be filled
      @return  Reference to filled stream buffer
  */
  virtual StreamBuffer& serialize(StreamBuffer& str);
  /** Output serialisation to stream buffer. Saves the status of the object.
      @param   str    Reference to Streambuffer to be read
      @return  Reference to read stream buffer
  */
  virtual StreamBuffer& serialize(StreamBuffer& str) const;
  /// Inform that a new incident has occured
  virtual void handle(const Incident& inc);
};

#endif // GAUDI_RNDMGENSVC_RNDMENGINE_H
