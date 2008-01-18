// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/PersSvc.h,v 1.7 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_PERSSVC_H
#define HBOOKCNV_PERSSVC_H 1


// Include files
#include "GaudiKernel/ConversionSvc.h"


// Forward declarations
template <class TYPE> class SvcFactory;


//------------------------------------------------------------------------------
//
// ClassName:   HbookCnv::PersSvc
//  
// Description: Persistency service - to store histograms in HBOOK format
//
// Author:      Pavel Binko
//
//------------------------------------------------------------------------------


namespace HbookCnv {
  
  class PersSvc : public ConversionSvc {
    
    friend class SvcFactory<PersSvc>;

  public:
    /// Initialise the service
    virtual StatusCode initialize();

    /// Finalise the service
    virtual StatusCode finalize();

    /// Convert a transient data object into its persistent representation
    virtual StatusCode createRep( DataObject* pObject,
                                  IOpaqueAddress*& refpAddr);
    /// Update references of the persistent data representation
    virtual StatusCode fillRepRefs( IOpaqueAddress* pAddr,
                                    DataObject* pObject);
  
    /// Standard constructor
    PersSvc( const std::string& name, ISvcLocator* svc );

    /// Standard destructor
    virtual ~PersSvc();

  private:
    std::string m_persistencySvc;
    std::string m_defFileName;
    std::string m_rowWisePolicy;
    int m_NPAWC;
    int m_NREC;
    int m_RECL;
    
    /// Flag to switch on printing of histograms to standard output
    bool m_histDo;
  };

}    // namespace HbookCnv


#endif    // HBOOKCNV_PERSSVC_H
