// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/DirectoryCnv.h,v 1.5 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_DIRECTORYCNV_H
#define HBOOKCNV_DIRECTORYCNV_H 1

// Include files
#include "HDirectoryCnv.h"
#include "GaudiKernel/DataObject.h"

//------------------------------------------------------------------------------
//
// ClassName:   HbookCnv::DirectoryCnv
//  
// Description: Histogram directory converter
//
// Author:      Pavel Binko
//
//------------------------------------------------------------------------------
namespace HbookCnv {

  class DirectoryCnv : public HDirectoryCnv {
    friend class CnvFactory<DirectoryCnv>;
  public:
    /// Inquire class type
    static const CLID& classID()      {
      return CLID_DataObject;
    }
    /// Standard constructor
    DirectoryCnv( ISvcLocator* svc ) : HDirectoryCnv(svc, CLID_DataObject)   {
      m_prefix = "/stat";
    }
    /// Standard destructor
    virtual ~DirectoryCnv() {
    }
  };
}    // namespace HbookCnv

#endif    // HBOOKCNV_DIRECTORYCNV_H 
