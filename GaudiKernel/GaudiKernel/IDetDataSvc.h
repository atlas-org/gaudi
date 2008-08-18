//$Id: IDetDataSvc.h,v 1.3 2006/01/25 16:02:48 hmd Exp $
#ifndef GAUDIKERNEL_IDETDATASVC_H
#define GAUDIKERNEL_IDETDATASVC_H 1

// Base class
#include "GaudiKernel/IInterface.h"

// IConditionDataSvc service ID (interface id, major version, minor version)
static const InterfaceID IID_IDetDataSvc ("IDetDataSvc", 1, 0);

// Forward declarations
namespace Gaudi { class Time; }

///---------------------------------------------------------------------------
/** @class IDetDataSvc IDetDataSvc.h GaudiKernel/IDetDataSvc.h

    Abstract interface for a DataSvc manipulating condition data
    (i.e. DataObjects implementing IValidity).

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IDetDataSvc : virtual public IInterface
{

 public:

  // Re-implemented from IInterface methods

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IDetDataSvc; }

 public:

  /// Check if the event time has been set 
  virtual const bool validEventTime() const = 0;

  /// Get the event time  
  virtual const Gaudi::Time& eventTime() const = 0;

  /// Set the event time  
  virtual void setEventTime( const Gaudi::Time& ) = 0;

};

#endif // GAUDIKERNEL_IDETDATASVC_H
