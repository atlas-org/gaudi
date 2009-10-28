#ifndef GAUDIKERNEL_IPARTPROPSVC_H
#define GAUDIKERNEL_IPARTPROPSVC_H

// Include Files
#include <string>
#include "GaudiKernel/IService.h"

namespace HepPDT {
  class ParticleDataTable;
  class ProcessUnknownID;
}

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IPartPropSvc(1070, 1 , 0); 

/* @class IPartPropSvc IPartPropSvc.h GaudiKernel/IPartPropSvc.h

    This class is an interface to the PartPropSvc.

    @author Charles Leggett

*/
class IPartPropSvc : virtual public IService {

public:
  // Typedefs for container type

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IPartPropSvc; }

  virtual HepPDT::ParticleDataTable* PDT() = 0;
  virtual void setUnknownParticleHandler( HepPDT::ProcessUnknownID*,
					  const std::string& ) = 0;

};

#endif

