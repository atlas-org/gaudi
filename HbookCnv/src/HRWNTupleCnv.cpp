// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HRWNTupleCnv.cpp,v 1.5 2006/01/10 20:11:14 hmd Exp $
#define HBOOKCNV_HRWNTUPLECNV_CPP


#define ALLOW_ALL_TYPES

// Compiler include files
#include <cstdio>

// Include files
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/INTupleSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "NTupleInfo.h"
#include "HRWNTupleCnv.h"
#include "HbookDef.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::HRWNTupleCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------


// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_CONVERTER_FACTORY(HbookCnv,HRWNTupleCnv)

/// Book the N tuple
StatusCode HbookCnv::HRWNTupleCnv::book(long idh, const std::string& loc, INTuple* nt)  {
  MsgStream log(msgSvc(), "HbookRWNTupleCnv");
  if ( !::HEXIST(idh) )    {
    const INTuple::ItemContainer& items = nt->items();
    NTUPLEINFO tags;
    float buffer[512];
    memset(&tags,0, sizeof(tags));
    tags.id = idh;
    tags.numVar = items.size();
    strncpy(tags.rzdir, loc.data(), loc.length());
    strncpy(tags.title, nt->title().data(), nt->title().length());
    for (size_t length = items.size(), i = 0; i < length; i++ )    {
      long len = items[i]->name().length();
      long nam_len = (len > 8) ? 8 : len;
      long pad_len=sizeof(tags.name[i])-nam_len-1;
      ::strncpy(tags.name[i], items[i]->name().data(), nam_len);
      ::memset(&tags.name[i][nam_len], ' ', pad_len);
      buffer[i] = float(items[i]->type());
    }
    StatusCode status = ::RWNT_BOOK(tags);
    if ( status.isSuccess() )   {
      log << MSG::INFO << "Booked Row wise HBOOK N tuple with ID:" << idh
          << " \"" << nt->title() << "\" in directory <" << loc << ">" << endreq;
      if ( dataTypePolicy() == USE_DATA_TYPES )   {
        ::HFN(idh, buffer);
      }
      log << MSG::DEBUG;
      if ( log.isActive() ) ::HPRINT( idh );
      return StatusCode::SUCCESS;
    }
    else    {
      log << MSG::ERROR << "HBOOK N tuple " << idh
          << "\"" << nt->title() 
          << "\" cannot be booked in directory <" << loc << ">" << endreq;
    }
  }
  log << MSG::ERROR << "HBOOK N tuple " << idh << " \"" << nt->title() 
      << "\" already exists." << endreq;
  return StatusCode::FAILURE;
}


/// Write N tuple data
StatusCode HbookCnv::HRWNTupleCnv::writeData(long idh, INTuple* nt)   {
  const INTuple::ItemContainer& items = nt->items();
  INTuple::ItemContainer::const_iterator i;
  float buffer[512], *buff = buffer;
  for (i = items.begin(); i != items.end(); i++)    {
    switch( (*i)->type() )   {
    case DataTypeInfo::BOOL:              // bool 
      *buff++ = float( *(bool*)(*i)->buffer() );
      break;
    case DataTypeInfo::CHAR:              // char 
      *buff++ = float( *(char*)(*i)->buffer() );
      break;
    case DataTypeInfo::SHORT:             // short
      *buff++ = float( *(short*)(*i)->buffer() );
      break;
    case DataTypeInfo::INT:               // int 
      *buff++ = float( *(int*)(*i)->buffer() );
      break;
    case DataTypeInfo::LONG:              // long 
      *buff++ = float( *(long*)(*i)->buffer() );
      break;
    case DataTypeInfo::UCHAR:             // unsigned char 
      *buff++ = float( *(unsigned char*)(*i)->buffer() );
      break;
    case DataTypeInfo::USHORT:            // unsigned short
      *buff++ = float( *(unsigned short*)(*i)->buffer() ); 
      break;
    case DataTypeInfo::UINT:              // unsigned int 
      *buff++ = float( *(unsigned int*)(*i)->buffer() );
      break;
    case DataTypeInfo::ULONG:             // unsigned long 
      *buff++ = float( *(unsigned long*)(*i)->buffer() );
      break;
    case DataTypeInfo::FLOAT:             // float 
      *buff++ = *(float*)((*i)->buffer());
      break;
    case DataTypeInfo::DOUBLE:            // double 
      *buff++ = float( *(double*)(*i)->buffer() );
      break;
    default:
      *buff++ = 0.0;
      break;
    }
    (*i)->reset();
  }
  ::HFN(idh, buffer);
  return StatusCode::SUCCESS;
}


/// Read N tuple data
StatusCode HbookCnv::HRWNTupleCnv::readData(long id, INTuple* pObject, long ievt)    {
  long istat = 0, numevt = 0, idn = 0;
  float buffer[512], *buff = buffer;
  StatusCode status = StatusCode::SUCCESS;
  INTuple::ItemContainer& items = pObject->items();
  ievt++;
  if ( dataTypePolicy() == USE_DATA_TYPES )   {
    ievt++;
  }
//  ::HGNF(id, ievt, buffer, istat);
  ::HGN(id, idn, ievt, buffer, istat);
  ::HNOENT( id, numevt);
  if ( istat == 0 && ievt <= numevt )   {
    INTuple::ItemContainer::iterator i;
    for (i = items.begin(); i != items.end(); i++)    {
      try   {
        long item_type = (*i)->type();
        switch( item_type )   {
        case DataTypeInfo::BOOL:              // bool 
          (dynamic_cast< NTuple::_Item<bool>* >(*i))->set((*buff++ < 0.000000001) ? false : true);
          break;
        case DataTypeInfo::CHAR:              // char 
          (dynamic_cast< NTuple::_Item<char>* >(*i))->set(char(*buff++));
          break;
        case DataTypeInfo::SHORT:             // short
          (dynamic_cast< NTuple::_Item<short>* >(*i))->set(short(*buff++));
          break;
        case DataTypeInfo::INT:               // int 
          (dynamic_cast< NTuple::_Item<int>* >(*i))->set(int(*buff++));
          break;
        case DataTypeInfo::LONG:              // long 
          (dynamic_cast< NTuple::_Item<long>* >(*i))->set(long(*buff++));
          break;
        case DataTypeInfo::UCHAR:             // unsigned char 
          (dynamic_cast< NTuple::_Item<unsigned char>* >(*i))->set((unsigned char)(*buff++));
          break;
        case DataTypeInfo::USHORT:            // unsigned short
          (dynamic_cast< NTuple::_Item<unsigned short>* >(*i))->set((unsigned short)(*buff++));
          break;
        case DataTypeInfo::UINT:              // unsigned int 
          (dynamic_cast< NTuple::_Item<unsigned int>* >(*i))->set((unsigned int)(*buff++));
          break;
        case DataTypeInfo::ULONG:             // unsigned long 
          (dynamic_cast< NTuple::_Item<unsigned long>* >(*i))->set((unsigned long)(*buff++));
          break;
        case DataTypeInfo::FLOAT:             // float 
          (dynamic_cast< NTuple::_Item<float>* >(*i))->set(float(*buff++));
          break;
        case DataTypeInfo::DOUBLE:            // double 
          (dynamic_cast< NTuple::_Item<double>* >(*i))->set(double(*buff++));
          break;
        case DataTypeInfo::OBJECT_ADDR:
          (dynamic_cast< NTuple::_Item<IOpaqueAddress*>* >(*i))->set(0);
          break;
        default:
          buff++;
          break;
        }
      }
      catch (...)     {
        status = StatusCode::FAILURE;
      }
    }
    return status;
  }
  return StatusCode::FAILURE;
}


#define CREATE_RWNTUPLE_COL(i,tuple,TYP) {                            \
item = createNTupleItem(tags, i, tuple, NTuple::Range<TYP>::min(), NTuple::Range<TYP>::max(), size); }

/// Create the transient representation of an object.
StatusCode HbookCnv::HRWNTupleCnv::load(long idh, INTuple*& refpObject)   {
  MsgStream log(msgSvc(), "HbookNTupleCnv");
  NTUPLEINFO tags;

  memset(&tags,0,sizeof(tags));
  tags.id = idh;
  StatusCode status = ::RWNT_INFO(tags);
  if ( status.isSuccess() && tags.numEnt > 0 )     {
    NTuple::Tuple* pObj = 0;
    status = m_ntupleSvc->create(CLID_RowWiseTuple, tags.title, pObj);
    INTuple* ntup = dynamic_cast<INTuple*>(pObj);
    if ( status.isSuccess() && 0 != ntup)   {
      long size = 0, istat = 0;
      std::string typ = "NONE";
      float buffer[512];
      std::string call = "HRWNTupleCnv::load";
      ::HGNPAR( idh, call.data(), call.length());
      // Make info compatible with column wise N-tuples
      for ( int i = 0; i < tags.numVar; i++ )   {
        ::strcpy(tags.block[i],"AUTO_BLK");
      }
      if ( dataTypePolicy() == USE_DATA_TYPES )   {
        ::HGNF(idh, 1, buffer, istat);
      }
      else if ( dataTypePolicy() == FORTRAN_STYLE )   {
        for ( int i = 0; i < tags.numVar; i++ )   {
          char c1 = ::toupper(tags.name[i][0]);
          if ( c1 >= 'I' && c1 <= 'N' )
            buffer[i] = DataTypeInfo::INT;
          else
            buffer[i] = DataTypeInfo::FLOAT;
        }
      }
      else if ( dataTypePolicy() == HUNGARIAN_STYLE )   {
        for ( int i = 0; i < tags.numVar; i++ )   {
          char c1 = ::toupper(tags.name[i][0]);
          char c2 = ::toupper(tags.name[i][1]);
          switch(c1)    {
          case 'B':   buffer[i] = DataTypeInfo::BOOL;             break;
          case 'C':   buffer[i] = DataTypeInfo::CHAR;             break;
          case 'S':   buffer[i] = DataTypeInfo::SHORT;            break;
          case 'I':   buffer[i] = DataTypeInfo::INT;              break;
          case 'L':   buffer[i] = DataTypeInfo::LONG;             break;
          case 'F':   buffer[i] = DataTypeInfo::FLOAT;            break;
          case 'D':   buffer[i] = DataTypeInfo::DOUBLE;           break;
          case 'O':   buffer[i] = DataTypeInfo::OBJECT_ADDR;      break;
          case 'U':
            switch(c2)    {
            case 'C':   buffer[i] = DataTypeInfo::UCHAR;          break;
            case 'S':   buffer[i] = DataTypeInfo::USHORT;         break;
            case 'I':   buffer[i] = DataTypeInfo::UINT;           break;
            case 'L':   buffer[i] = DataTypeInfo::ULONG;          break;
            default:
            log << MSG::ERROR << "Cannot Create Item for Var[" << i << "]: " 
                              << " Name:" << tags.name[i] 
                              << " Unknown data type" << endreq;
            return StatusCode::FAILURE;
            }
            break;
          default:
            log << MSG::ERROR << "Cannot Create Item for Var[" << i << "]: " 
                              << " Name:" << tags.name[i] 
                              << " Unknown data type" << endreq;
            return StatusCode::FAILURE;
          }
        }
      }
      else    {
        for ( int i = 0; i < tags.numVar; i++ )   {
          buffer[i] = float(DataTypeInfo::FLOAT);
        }
      }
      if ( istat == 0 )   {
        for ( int i = 0; i < tags.numVar; i++ )   {
          INTupleItem* item = 0;
          switch( long(buffer[i]) )   {
            case DataTypeInfo::BOOL:              // bool 
              CREATE_RWNTUPLE_COL(i,ntup, bool)
              break;
            case DataTypeInfo::CHAR:              // char 
              CREATE_RWNTUPLE_COL(i,ntup, char)
              break;
            case DataTypeInfo::SHORT:             // short
              CREATE_RWNTUPLE_COL(i,ntup, short)
              break;
            case DataTypeInfo::INT:               // int 
              CREATE_RWNTUPLE_COL(i,ntup, int)
              break;
            case DataTypeInfo::LONG:              // long 
              CREATE_RWNTUPLE_COL(i,ntup, long)
              break;
            case DataTypeInfo::UCHAR:             // unsigned char 
              CREATE_RWNTUPLE_COL(i,ntup, unsigned char)
              break;
            case DataTypeInfo::USHORT:            // unsigned short
              CREATE_RWNTUPLE_COL(i,ntup, unsigned short)
              break;
            case DataTypeInfo::UINT:              // unsigned int 
              CREATE_RWNTUPLE_COL(i,ntup, unsigned int)
              break;
            case DataTypeInfo::ULONG:             // unsigned long 
              CREATE_RWNTUPLE_COL(i,ntup, unsigned long)
              break;
            case DataTypeInfo::FLOAT:             // float 
              CREATE_RWNTUPLE_COL(i,ntup, float)
              break;
            case DataTypeInfo::DOUBLE:            // double 
              CREATE_RWNTUPLE_COL(i,ntup, double)
              break;
            case DataTypeInfo::OBJECT_ADDR:
              //CREATE_RWNTUPLE_COL(i,ntup, IOpaqueAddress*)
              break;
            default:
              break;
          }
          if ( item )    {
            ntup->items().push_back(item);
            log << MSG::DEBUG << "Create Var[" << i << "]: " 
                << item->typeName() << "[1]"
                << " Name:" << tags.name[i]
                << "[" << tags.frange[i][0] << "," << tags.frange[i][1] 
                << "]" << endreq;
          }
          else    {
            log << MSG::ERROR << "Cannot Create Item for Var[" << i << "]: " 
                              << " Name:" << tags.name[i] << endreq;
          }
        }
        log << MSG::DEBUG << "Total buffer size of NTuple: " << size << " Bytes." << endreq; 
        refpObject = ntup;
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}
