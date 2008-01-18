// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/H2DCnv.h,v 1.8 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_H2DCNV_H
#define HBOOKCNV_H2DCNV_H 1


// Include files
#include "HConverter.h"


// Forward declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;



//------------------------------------------------------------------------------
//
// ClassName:   HbookCnv::H2DCnv
//  
// Description: Converter of 2-dimensional histogram with fixed binning
//              into HBOOK format and back
//
// Author:      Pavel Binko
//
//------------------------------------------------------------------------------


namespace HbookCnv {
  
  class H2DCnv : public HConverter {
    friend class CnvFactory<H2DCnv>;
  public:
    /// Inquire class type
    static const CLID& classID()   {
      return CLID_H2D;
    }
    /// Create the transient representation of an object.
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
    /// Update the transient object from the other representation.
    virtual StatusCode updateObj(IOpaqueAddress* pAddr, DataObject* pObj);
    /// Update the converted representation of a transient object.
    virtual StatusCode updateRep(IOpaqueAddress* pAddr, DataObject* pObj);
    /// Standard constructor
    H2DCnv( ISvcLocator* svc ) : HConverter( classID(), svc )   { 
      m_prefix = "/stat";
      m_deleteAfterSave = true;
    }
    /// Standard destructor
    virtual ~H2DCnv()    { 
    }
  protected:
    /// Book 2 D histogram
    virtual StatusCode book(IOpaqueAddress* pAddr, DataObject* pObj);
  };

}    // namespace HbookCnv

#endif    // HBOOKCNV_H2DCNV_H
