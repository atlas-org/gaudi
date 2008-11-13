#ifndef GAUDI_DATAINCIDENT_H
#define GAUDI_DATAINCIDENT_H

// Include files
#include "GaudiKernel/Incident.h"
#include <string>

// Forward declarations

/**
 * @class DataSvc DataSvc.h GaudiKernel/DataSvc.h
 *
 * Data service incident class.
 *
 * @author Markus Frank
 * @version 1.0
 */
class DataIncident : public Incident {
public:
  /// Initializing Constructor
  /** @param source   [IN]  Incident source (service or algorithm name)
    * @param type     [IN]  Incident type
    * @param tag      [IN]  Incident tag
    *
    * @return Reference to DataIncident object.
    */
  DataIncident(const std::string& source,
               const std::string& type,
               const std::string& tag)
               : Incident(source, type), m_tag(tag)
  {
  }

  /// Standard destructor
  virtual ~DataIncident() { }

  /// Accesssor to the tag value
  const std::string& tag()  const { return m_tag; }
private:
  std::string m_tag;
};
#endif //GAUDI_DATAINCIDENT_H

