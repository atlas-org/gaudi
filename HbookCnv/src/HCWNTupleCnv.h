// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HCWNTupleCnv.h,v 1.3 2006/11/30 14:59:27 mato Exp $
#ifndef HBOOKCNV_HCWNTUPLECNV_H
#define HBOOKCNV_HCWNTUPLECNV_H 1


// Include files
#include "HNTupleCnv.h"


// Forward declarations
template <class TYPE> class CnvFactory;


//====================================================================
//  Column wise NTuple converter class definition
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

  class HCWNTupleCnv : public HNTupleCnv   {

    /// Creator needs access to constructor/destructor
    friend class CnvFactory<HCWNTupleCnv>;

  public:
    /// Inquire class type
    static const CLID& classID()    {
      return CLID_ColumnWiseTuple;
    }
    /// Standard constructor
    HCWNTupleCnv( ISvcLocator* svc ) : HNTupleCnv(svc, classID())    {
    }
    /// Standard destructor
    virtual ~HCWNTupleCnv()   {
    }
  protected:
    /// Create the transient representation of an object.
    virtual StatusCode load( long id, INTuple*& refpObject );
    /// Book the N tuple
    virtual StatusCode book(long idh, const std::string& location, INTuple* pObject);
    /// Declare the N tuple
    virtual StatusCode declare(long idh, INTuple* pObject);
    /// Write N tuple data
    virtual StatusCode writeData(long idh, INTuple* pObject);
    /// Read N tuple data
    virtual StatusCode readData(long idh, INTuple* pObject, long ievt);
    template <class T> 
    size_t saveItem(char* target, const T* src, size_t len)   {
      long* tar = (long*)target;
      for ( size_t i = 0; i < len; i++ )   {
        *(tar++) = long( *(src++));
      }
      return sizeof(long)*len;
    }
    template <class T> 
    size_t loadItem(char* src, T* tar, size_t len)   {
      long* s = (long*)src;
      for ( size_t i = 0; i < len; i++ )   {
        *(tar++) = T( *(s++));
      }
      return sizeof(long)*len;
    }
    size_t loadItem(char* src, bool* tar, size_t len)   {
      long* s = (long*)src;
      for ( size_t i = 0; i < len; i++ )   {
        *(tar++) = (*(s++)) ? true : false;
      }
      return sizeof(long)*len;
    }
    size_t loadItem(char* src, float* target, size_t len)   {
      memcpy(target, src, sizeof(float)*len);
      return sizeof(float)*len;
    }
    size_t loadItem(char* src, double* target, size_t len)   {
      memcpy(target, src, sizeof(double)*len);
      return sizeof(double)*len;
    }
    size_t loadItem(char* src, long* target, size_t len)   {
      memcpy(target, src, sizeof(long)*len);
      return sizeof(long)*len;
    }
    size_t loadItem(char* src, unsigned long* target, size_t len)   {
      memcpy(target, src, sizeof(unsigned long)*len);
      return sizeof(unsigned long)*len;
    }

    size_t saveItem(char* target, float* src, size_t len)   {
      memcpy(target, src, sizeof(float)*len);
      return sizeof(float)*len;
    }
    size_t saveItem(char* target, double* src, size_t len)   {
      memcpy(target, src, sizeof(double)*len);
      return sizeof(double)*len;
    }
    size_t saveItem(char* target, long* src, size_t len)   {
      memcpy(target, src, sizeof(long)*len);
      return sizeof(long)*len;
    }
    size_t saveItem(char* target, unsigned long* src, size_t len)   {
      memcpy(target, src, sizeof(unsigned long)*len);
      return sizeof(unsigned long)*len;
    }

  };

}    // namespace HbookCnv

 
#endif    // HBOOKCNV_HCWNTUPLECNV_H
