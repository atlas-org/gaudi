// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HNTupleCnv.h,v 1.1.1.1 2000/11/23 11:34:55 ranjard Exp $
#ifndef HBOOKCNV_HNTUPLECNV_H
#define HBOOKCNV_HNTUPLECNV_H 1


// Include files
#include "HConverter.h"


// Forward declarations
class ISvcLocator;
class NTUPLEINFO;
class INTupleItem;
class INTupleSvc;
class INTuple;


//====================================================================
//  NTuple converter class definition
//--------------------------------------------------------------------
//
//  Package    : HbookCnv ( The LHCb Offline System)
//  Author     : M.Frank
//
//====================================================================


namespace HbookCnv {

  /** NTuple converter class definition

      Description:
      Definition of the converter for Ntuples using HBOOK

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

  class HNTupleCnv : public HConverter   {

  public:
    /// Initialize the converter
    virtual StatusCode initialize();
    /// Finalize the converter
    virtual StatusCode finalize();
    /// Create the transient representation of an object.
    virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
    /// Update the transient object from the other representation.
    virtual StatusCode updateObj( IOpaqueAddress* pAddress, DataObject* refpObject );
    /// Convert the transient object to the requested representation.
    virtual StatusCode createRep( DataObject* pObject, IOpaqueAddress*& refpAddress );
    /// Update the converted representation of a transient object.
    virtual StatusCode updateRep( IOpaqueAddress* pAddress, DataObject* pObject );

  protected:
    /// Standard constructor
    HNTupleCnv( ISvcLocator* svc, const CLID& clid );
    /// Standard destructor
    virtual ~HNTupleCnv();
    /// Create the transient representation of an object.
    virtual StatusCode load( long id, INTuple*& refpObject ) = 0;
    /// Book the N tuple
    virtual StatusCode book( long idh, const std::string& loc, INTuple* pObject) = 0;
    /// Write N tuple data
    virtual StatusCode writeData(long idh, INTuple* pObject) = 0;
    /// Read N tuple data
    virtual StatusCode readData(long idh, INTuple* pObject, long ievt) = 0;


  protected:
    /// Reference to N tuple service
    INTupleSvc*  m_ntupleSvc;

  };

  /// Add an item of a given type to the N tuple
  template<class TYP>
  INTupleItem* createNTupleItem(NTUPLEINFO& tags, long i, INTuple* tuple, TYP minimum, TYP maximum, long& size);

  bool parseName(std::string full, std::string &blk, std::string &var);
 
}    // namespace HbookCnv


#endif    // HBOOKCNV_HNTUPLECNV_H
