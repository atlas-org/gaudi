// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HRWNTupleCnv.h,v 1.3 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_HRWNTUPLECNV_H
#define HBOOKCNV_HRWNTUPLECNV_H 1


// Include files
#include "HNTupleCnv.h"


// Forward declarations
template <class TYPE> class CnvFactory;



//====================================================================
//  Row wise NTuple converter class definition
//--------------------------------------------------------------------
//
//  Package    : HbookCnv ( The LHCb Offline System)
//  Author     : M.Frank
//
//====================================================================


namespace HbookCnv {

  /** Row wise NTuple converter class definition

      Description:
      Definition of the converter for row wise Ntuples using HBOOK

      Dependencies:
      <UL>
      <LI> Base class: HNTupleCnv.h
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

  class HRWNTupleCnv : public HNTupleCnv   {

    /// Creator needs access to constructor/destructor
    friend class CnvFactory<HRWNTupleCnv>;

  public:
    /// Inquire class type
    static const CLID& classID()    {
      return CLID_RowWiseTuple;
    }
    /// Standard constructor
    HRWNTupleCnv( ISvcLocator* svc )  : HNTupleCnv(svc, classID())    {
    }
    /// Standard destructor
    virtual ~HRWNTupleCnv()   {
    }
  protected:
    /// Create the transient representation of an object.
    virtual StatusCode load( long id, INTuple*& refpObject );
    /// Book the N tuple
    virtual StatusCode book(long idh, const std::string& loc, INTuple* pObject);
    /// Write N tuple data
    virtual StatusCode writeData(long idh, INTuple* pObject);
    /// Read N tuple data
    virtual StatusCode readData(long idh, INTuple* pObject, long ievt);

  };

}    // namespace HbookCnv

 
#endif    // HBOOKCNV_HRWNTUPLECNV_H
