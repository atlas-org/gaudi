// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IIncidentListener.h,v 1.4 2004/07/19 06:47:55 mato Exp $
#ifndef GAUDIKERNEL_IINCIDENTLISTENER_H
#define GAUDIKERNEL_IINCIDENTLISTENER_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Incident.h"
#include <string>


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IIncidentListener(311, 1 , 0); 

/** @class IIncidentListener IIncidentListener.h GaudiKernel/IIncidentListener.h
    The interface implemented by any class wanting to listen to Incidents.
    @author Pere Mato
*/
class IIncidentListener : virtual public IInterface {
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IIncidentListener; }
  /// Inform that a new incident has occured
  virtual void handle(const Incident&) = 0;
};

#endif // GAUDIKERNEL_IINCIDENTLISTENER_H
