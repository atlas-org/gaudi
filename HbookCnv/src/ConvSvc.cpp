// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/ConvSvc.cpp,v 1.4 2006/01/10 20:11:13 hmd Exp $
#define HBOOKCNV_ConvSvc_CPP


// Include files
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ConversionSvc.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::ConvSvc
//
// Author :                   Pavel Binko
//
//------------------------------------------------------------------------------


 

namespace HbookCnv {
  class ConvSvc   : public ConversionSvc    {
  public:
    /// Standard Constructor
    ConvSvc(const std::string& name, ISvcLocator* svc)
    : ConversionSvc(name, svc, HBOOK_StorageType)  {
    }
    /// Standard Destructor
    virtual ~ConvSvc()   {
    }
  };
};    // namespace HbookCnv
// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_SERVICE_FACTORY(HbookCnv,ConvSvc)
