// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HConverter.h,v 1.8 2005/10/03 15:42:55 hmd Exp $
#ifndef HBOOKCNV_HCONVERTER_H
#define HBOOKCNV_HCONVERTER_H 1

#include <map>

// Include files
#include "GaudiKernel/Converter.h"

// Forward declarations
class ISvcLocator;

//====================================================================
//  HConverter class definition
//--------------------------------------------------------------------
//
//  Package    : HbookCnv ( The LHCb Offline System)
//  Author     : M.Frank
//
//====================================================================


namespace HbookCnv {

  /** Generic HBook converter class definition

  Description:
  Definition of the converter using HBOOK

  Dependencies:
  <UL>
  <LI> Base class: Gaudi/Kernel/Converter.h
  </UL>

  <P> History    :
  <PRE>
  +---------+----------------------------------------------+--------+
  |    Date |                 Comment                      | Who    |
  +---------+----------------------------------------------+--------+
  | 21/10/99| Initial version.                             | MF     |
  +---------+----------------------------------------------+--------+
  </PRE>
  Author:  M.Frank
  Version: 1.0
  */

  class HConverter : public Converter   {

  public:
    /// Book the histogram/N-tuple object
    virtual StatusCode book(IOpaqueAddress* /* pAddr */, DataObject* /* pObj*/)    {
      return StatusCode::FAILURE;
    }
    /// Convert the transient object to the requested persistent representation.
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& refpAddress);
    /// Update the converted representation of a transient object.
    virtual StatusCode writeObject(const std::string& loc, long id);
    /// Create the transient representation of an object.
    StatusCode readObject(const std::string& loc, long id);
    /// Create address of the transient object according to the requested representation.
    StatusCode createAddress(const std::string& rzdir, const CLID& clid, const std::string& title, IOpaqueAddress*& refpAddress);
    /// Create address of the transient object according to the requested representation.
    StatusCode createAddress(const std::string& rzdir, const CLID& clid, long id, IOpaqueAddress*& refpAddress);
    /// Retrieve name of disk directory from location
    std::string diskDirectory(const std::string& loc);
    /// Retrieve name of memory directory from location
    std::string directory(const std::string& loc);
    /// Set RZ sub directories if they do not exist according to the store location
    void setDirectory(const std::string& loc);
    /// Set RZ sub directories if they do not exist according to the store location
    void setDiskDirectory(const std::string& loc);
    /// Create RZ directory if not present
    StatusCode createDirectory(const std::string& loc);
    /// Create disk based RZ directory if not present
    StatusCode createDiskDirectory(const std::string& loc);
    /// Create RZ sub directories if they do not exist according to the store location
    StatusCode createSubDirs(const std::string& top, const std::string& loc);

    /// Inquire storage type
    static long storageType();
    long repSvcType() const {
      return i_repSvcType();
    }

  protected:
    /// Standard constructor
    HConverter( const CLID& clid, ISvcLocator* svc );
    /// Standard destructor
    virtual ~HConverter();
    /// Helper to fix persistent RZ directories
    std::string m_prefix;
    /// Flag to indicate that object should be deleted after saving
    bool        m_deleteAfterSave;
    
  };

};    // namespace HbookCnv


#endif    // HBOOKCNV_HCONVERTER_H
