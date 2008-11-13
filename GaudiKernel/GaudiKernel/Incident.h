#ifndef GAUDI_INCIDENT_H
#define GAUDI_INCIDENT_H

// Include files
#include <string>

/** @class Incident Incident.h GaudiKernel/Incident.h
 *
 *  Base class for all Incidents (computing events).
 *
 *  @author P. Mato
 *  @date   2001/01/19
 */

class Incident {

public:

  /// Default Constructor
  Incident ( const std::string& source, ///< Incident source (service or algorithm name)
             const std::string& type    ///< Incident type
             ) 
    : m_source ( source ), 
      m_type   ( type   ) { }

  /// Destructor
  virtual ~Incident() { }

  /** Access to the incident type
   *
   *  @return string descriptor for the incident type
   */
  const std::string& type()  const { return m_type; }

  /** Access to the source of the incident
   *
   *  @return service or algorithm name that initiated the incident
   */
  const std::string& source() const { return m_source; }

private:

  std::string m_source; ///< Incident source
  std::string m_type;   ///< incident type

};

/** @namespace IncidentType
 *  
 *  Namespace for pre-defined common incident types
 *
 *  @author P. Mato
 *  @date   2001/01/19
 */
namespace IncidentType 
{
  const std::string BeginEvent = "BeginEvent"; ///< Processing of a new event has started
  const std::string EndEvent   = "EndEvent";   ///< Processing of the last event has finished
  const std::string BeginRun   = "BeginRun";   ///< Processing of a new run has started
  const std::string EndRun     = "EndRun";     ///< Processing of the last run has finished
  const std::string EndStream  = "EndStream";  ///< Processing of the stream has finished

  const std::string AbortEvent = "AbortEvent"; ///< Stop processing the current event and pass to te next one
}

#endif //GAUDI_INCIDENT_H

