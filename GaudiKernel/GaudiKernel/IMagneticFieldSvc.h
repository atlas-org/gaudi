// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IMagneticFieldSvc.h,v 1.8 2006/05/29 08:46:06 hmd Exp $
#ifndef GAUDIKERNEL_IMAGNETICFIELDSVC_H
#define GAUDIKERNEL_IMAGNETICFIELDSVC_H


// Include files
#include "GaudiKernel/IService.h"
#include "GaudiKernel/StatusCode.h"


// Forward declarations
template <class TYPE> class SvcFactory;
namespace ROOT {
  namespace Math {
    class DefaultCoordinateSystemTag;
    
    // from Math/Point3Dfwd.h
    template<class CoordSystem, class Tag> class PositionVector3D;
    // from Math/Vector3Dfwd.h
    template<class CoordSystem, class Tag> class DisplacementVector3D;
    // from Math/Point3Dfwd.h
    template<typename T> class Cartesian3D;
    // from Math/Point3Dfwd.h
    typedef PositionVector3D< Cartesian3D<double>,DefaultCoordinateSystemTag> XYZPoint;
    // from Math/Vector3Dfwd.h
    typedef DisplacementVector3D< Cartesian3D<double>,DefaultCoordinateSystemTag > XYZVector;
  }
}

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMagneticFieldSvc(80, 1 , 0); 


/** @class IMagneticFieldSvc IMagneticFieldSvc.h GaudiKernel/IMagneticFieldSvc.h

    The interface to the MagneticFieldSvc

    @author Iain Last
*/
class IMagneticFieldSvc : virtual public IService   {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMagneticFieldSvc; }

  // Get the magnetic field vector at a given point in space.
  // Input: XYZPoint - Point at which magnetic field vector is to be given.
  // Output: XYZVector - Magnectic field vector.
  // Return: StatusCode SUCCESS if calculation was performed.
  virtual StatusCode fieldVector( const ROOT::Math::XYZPoint& xyz, 
				  ROOT::Math::XYZVector& fvec ) const = 0;

};

#endif  // GAUDIKERNEL_IMAGNETICFIELDSVC_H
