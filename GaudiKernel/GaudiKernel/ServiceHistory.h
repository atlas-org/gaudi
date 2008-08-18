// $Id: ServiceHistory.h,v 1.1.4.1 2008/06/19 21:30:47 leggett Exp $

#ifndef GAUDIKERNEL_SERVICEHISTORY_H
#define GAUDIKERNEL_SERVICEHISTORY_H

#include "GaudiKernel/HistoryObj.h"

#include <string>
#include <vector>

class IService;
class Property;
class JobHistory;

/** @class ServiceHistory ServiceHistory.h
 * 
 *  ServiceHistory class definition
 *
 *  @author: Charles Leggett
 *
 */

class ServiceHistory: public HistoryObj {

public:  // typedefs

  // List of properties. This may change.
  typedef std::vector<Property*> PropertyList;

public:
  ServiceHistory();
  ServiceHistory(const IService*, const JobHistory* ) ;
  ServiceHistory(const IService&, const JobHistory* ) ;

  virtual ~ServiceHistory();

  // Class IDs
  virtual const CLID& clID() const { return classID(); }
  static const CLID& classID();

  // Return the service properties.
  const PropertyList& properties() const { return m_properties; }

  std::string name() const { return m_name; }
  std::string type() const { return m_type; }
  const IService* service() const { return m_pService; }

  const JobHistory* jobHistory() const { return m_jobHistory; }

private:

  const IService* m_pService;
  const JobHistory* m_jobHistory;
  std::string m_name,m_type;
  PropertyList m_properties;

};

// Output stream.
std::ostream& operator<<(std::ostream& lhs, const ServiceHistory& rhs);

#endif
