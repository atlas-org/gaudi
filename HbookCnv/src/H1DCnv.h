// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/H1DCnv.h,v 1.8 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_H1DCNV_H
#define HBOOKCNV_H1DCNV_H 1


// Include files
#include "HConverter.h"


// Forward declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;



//------------------------------------------------------------------------------
//
// ClassName:   HbookCnv::H1DCnv
//  
// Description: Converter of 1-dimensional histogram
//              (with fixed or variable binning)
//              into HBOOK format and back
//
// Author:      Pavel Binko
// Author:      Grigori Rybkine
//
//------------------------------------------------------------------------------
namespace HbookCnv {

  class H1DCnv : public HConverter {
    friend class CnvFactory<H1DCnv>;
  public:
    /// Inquire class type
    static const CLID& classID()     {
      return CLID_H1D;
    }
    /// Create the transient representation of an object.
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
    /// Update the transient object from the other representation.
    virtual StatusCode updateObj(IOpaqueAddress* pAddr, DataObject* pObj);
    /// Update the converted representation of a transient object.
    virtual StatusCode updateRep(IOpaqueAddress* pAddr, DataObject* pObj);

    /// Standard constructor
    H1DCnv( ISvcLocator* svc ) : HConverter( classID(), svc )  { 
      m_prefix = "/stat";
      m_deleteAfterSave = true;
    }
    /// Standard constructor
    H1DCnv( const CLID& clid, ISvcLocator* svc ) : HConverter( clid, svc )  { 
      m_prefix = "/stat";
      m_deleteAfterSave = true;
    }
    /// Standard destructor
    virtual ~H1DCnv()  { 
    }
  protected:
    /// Book 1 D histogram
    virtual StatusCode book(IOpaqueAddress* pAddr, DataObject* pObj);
  };
}    // namespace HbookCnv
#endif    // HBOOKCNV_H1DCNV_H
