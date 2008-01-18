// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HFileCnv.h,v 1.4 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_HFILECNV_H
#define HBOOKCNV_HFILECNV_H 1


// Include files
#include "HDirectoryCnv.h"


// Forward declarations
class     ISvcLocator;

//====================================================================
//  NTuple converter class definition
//--------------------------------------------------------------------
//
//  Package    : HbookCnv ( The LHCb Offline System)
//  Author     : M.Frank
//
//====================================================================


namespace HbookCnv {

  /** NTuple file converter class definition

      Description:
      Definition of the converter to open HBOOK RZ files

      Dependencies:
      <UL>
      <LI> Base class: HDirectoryCnv.h
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

  class HFileCnv : public HDirectoryCnv   {

    friend class CnvFactory<HFileCnv>;

  public:
    /// Create the transient representation of an object.
    virtual StatusCode createObj  ( IOpaqueAddress* pAddress, DataObject*& refpObject );
    /// Convert the transient object to the requested representation.
    virtual StatusCode createRep( DataObject* pObject, IOpaqueAddress*& refpAddress );
    /// Convert the transient object to the requested representation.
    virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
    /// Inquire class type
    static const CLID& classID()    {
      return CLID_NTupleFile;
    }
    
    virtual StatusCode initialize() ;
    
    /// Standard constructor
    HFileCnv( ISvcLocator* svc ) 
      : HDirectoryCnv(svc, CLID_NTupleFile)    
      , m_LRECL ( 1024 )
      , m_NRECL ( -1   )
      , m_MODE  ( "P"  )
    {}
    /// Standard destructor
    virtual ~HFileCnv(){}
    
  private:
    int         m_LRECL ;
    int         m_NRECL ;
    std::string m_MODE  ;
  };

};    // namespace HbookCnv

 
#endif    // HBOOKCNV_HFILECNV_H
