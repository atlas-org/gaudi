// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HDirectoryCnv.h,v 1.4 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_HDIRECTORYCNV_H
#define HBOOKCNV_HDIRECTORYCNV_H 1

// Include files
#include "HConverter.h"

// Forward declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;


//------------------------------------------------------------------------------
//
// ClassName:   HbookCnv::HDirectoryCnv
//  
// Description: Histogram directory converter
//
// Author:      M.Frank
//
//------------------------------------------------------------------------------


namespace HbookCnv {

  /** Create persistent and transient representations of 
      data store directories
  */

  class HDirectoryCnv : public HConverter {

    friend class CnvFactory<HDirectoryCnv>;

  public:
    /// Create the transient representation of an object.
    virtual StatusCode createObj( IOpaqueAddress* pAddress, DataObject*& refpObject );

    /// Update the transient object from the other representation.
    virtual StatusCode fillObjRefs( IOpaqueAddress* pAddress, DataObject* refpObject );

    /// Convert the transient object to the requested representation.
    virtual StatusCode createRep( DataObject* pObject, IOpaqueAddress*& refpAddress );

    /// Convert the transient object to the requested representation.
    virtual StatusCode updateRep( IOpaqueAddress* pAddress, DataObject* pObject );

    /// Inquire class type
    static const CLID& classID();

    /// Standard constructor
    HDirectoryCnv( ISvcLocator* svc );

    /// Standard constructor
    HDirectoryCnv( ISvcLocator* svc, const CLID& clid );

    /// Standard destructor
    virtual ~HDirectoryCnv();
  };

}    // namespace HbookCnv


#endif    // HBOOKCNV_HDIRECTORYCNV_H 
