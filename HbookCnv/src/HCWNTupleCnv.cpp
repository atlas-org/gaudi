// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/HbookCnv/src/HCWNTupleCnv.cpp,v 1.6 2006/01/10 20:11:14 hmd Exp $
#define HBOOKCNV_HCWNTUPLECNV_CPP


#define ALLOW_ALL_TYPES

// Compiler include files
#include <cstdio>
#include <climits>

// Include files
#include "GaudiKernel/xtoa.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataTypeInfo.h"
#include "GaudiKernel/INTupleSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "HCWNTupleCnv.h"
#include "NTupleInfo.h"
#include "HbookDef.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  HbookCnv::HCWNTupleCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service
DECLARE_NAMESPACE_CONVERTER_FACTORY(HbookCnv,HCWNTupleCnv)

template <class T> void analyzeItem(const char typ, 
                                    const NTuple::_Data<T>* it, 
                                    std::string& desc,
				    std::string& block_name,
                                    long& size)    {

  std::string full_name, var_name;
  full_name = it->name();
  HbookCnv::parseName(full_name,block_name,var_name);

  long item_size = (sizeof(T) < 4) ? 4 : sizeof(T);
  long dimension = it->length();
  long ndim = it->ndim()-1;
  char text[132];
  //  desc += it->name();
  desc += var_name;
  if ( it->hasIndex() || it->length() > 1 )   {
    desc += '(';
  }
  for ( int i = 0; i < ndim; i++ )   {
    desc += ::_itoa(it->dim(i), text, 10);
    desc += ',';
  }
  if ( it->hasIndex() )   {
    std::string ind_blk, ind_var;
    std::string ind = it->index();
    HbookCnv::parseName(ind,ind_blk,ind_var);
    if (ind_blk != block_name) {
      std::cerr << "ERROR: Index for CWNT variable " << ind_var 
		<< " is in a differnt block: " << ind_blk << std::endl;
    }
    desc += ind_var;
  }
  else if ( it->dim(ndim) > 1 )   {
    desc += ::_itoa(it->dim(ndim), text, 10);
  }

  if ( it->hasIndex() || it->length() > 1 )   {
    desc += ")";
  }
  if ( typ != 'L' && typ != 'R' )   {
    desc += '[';
    desc += ::_itoa(long(it->range().lower()), text, 10);
    desc += ',';
    desc += ::_itoa(long(it->range().upper()), text, 10);
    desc += ']';
  }
  desc += ':';
  desc += typ;
  desc += '*';
  desc += ::_itoa(item_size, text, 10);
  size += item_size * dimension;
}


/// Book the N tuple
StatusCode HbookCnv::HCWNTupleCnv::declare(long idh, INTuple* nt)  {
  MsgStream log(msgSvc(), "HCWNTupleCnv");

  const INTuple::ItemContainer& cols = nt->items();
  //  char text[64];
  std::string desc = "";
  std::string block_name,var_name;
  long size = 0;
  size_t cnt =0;

  long cursize, oldsize = 0;
  std::vector<long> item_size;
  std::vector<std::pair<std::string,std::string> > item_name;

  try   {
    for (INTuple::ItemContainer::const_iterator i = cols.begin(); i != cols.end(); i++ )   {
      std::string item = "";
      switch( (*i)->type() )   {
      case DataTypeInfo::BOOL:              // bool
        analyzeItem('L', dynamic_cast<const NTuple::_Data<bool>*>(*i),item,
		    block_name,size);
        break;
      case DataTypeInfo::CHAR:              // char 
        analyzeItem('I', dynamic_cast<const NTuple::_Data<char>*>(*i),item,
		    block_name,size);
        break;
      case DataTypeInfo::INT:               // int 
        analyzeItem('I', dynamic_cast<const NTuple::_Data<int>*>(*i),item,
		    block_name,size);
        break;
      case DataTypeInfo::SHORT:             // short
        analyzeItem('I', dynamic_cast<const NTuple::_Data<short>*>(*i),item,
		    block_name,size);
        break;
      case DataTypeInfo::LONG:              // long 
        analyzeItem('I', dynamic_cast<const NTuple::_Data<long>*>(*i),item,
		    block_name,size);
        break;
      case DataTypeInfo::UCHAR:             // unsigned char 
        analyzeItem('U', dynamic_cast<const NTuple::_Data<unsigned char>*>(*i),
		    item,block_name,size);
        break;
      case DataTypeInfo::USHORT:            // unsigned short
        analyzeItem('U', dynamic_cast<const NTuple::_Data<unsigned short>*>(*i),
		    item,block_name,size);
        break;
      case DataTypeInfo::UINT:              // unsigned int 
        analyzeItem('U', dynamic_cast<const NTuple::_Data<unsigned int>*>(*i),
		    item,block_name,size);
        break;
      case DataTypeInfo::ULONG:             // unsigned long 
        analyzeItem('U', dynamic_cast<const NTuple::_Data<unsigned long>*>(*i),
		    item,block_name,size);
        break;
      case DataTypeInfo::DOUBLE:            // double 
        analyzeItem('R', dynamic_cast<const NTuple::_Data<double>*>(*i),item,
		    block_name,size);
        break;
      case DataTypeInfo::FLOAT:             // float 
        analyzeItem('R', dynamic_cast<const NTuple::_Data<float>*>(*i),item,
		    block_name,size);
      default:
        break;
      }
      desc += item;
      if ( ++cnt < cols.size() )   {
        desc += ", ";
      }

      item_name.push_back(std::make_pair(block_name,item));
      cursize = size - oldsize;
      item_size.push_back(size-cursize);
      oldsize = size;
    }
    char* buff = new char[size];
    nt->setBuffer(buff);

    //    const unsigned int MAX_BLOCK_NAME = 1300;
    std::string block_desc = "";
    char *buf_pos = buff;

    std::vector<std::pair<std::string,std::string> >::const_iterator itr,end;
    end = item_name.end();
    int pos = 0;

    /// Loop over items, add them to ntuple blocks

    for (itr=item_name.begin(); itr!=end; ++itr) {
      buf_pos = buff + item_size[pos++]; 
      block_desc = itr->second;

      ::HBNAME(idh, itr->first.c_str(), buf_pos, block_desc.c_str());
      
      log << MSG::DEBUG << "CWNT " << idh << ": added " 
	  << itr->first << " : " << block_desc << endreq;

    }
    
    return StatusCode::SUCCESS;
  }
  catch (...)   {
  }
  return StatusCode::FAILURE;
}


/// Book the N tuple
StatusCode HbookCnv::HCWNTupleCnv::book(long idh, const std::string& loc, INTuple* nt)  {
  MsgStream log(msgSvc(), "HCWNTupleCnv");
  //  log << MSG::DEBUG << "in HCWNTupleCnv::book" << endreq;
  if ( !::HEXIST( idh ) )   {
    NTUPLEINFO tags;
    memset(&tags,0, sizeof(tags));
    tags.id = idh;
    strncpy(tags.rzdir, loc.data()+2, loc.length()-2);
    strncpy(tags.title, nt->title().data(), nt->title().length());
    StatusCode status = ::CWNT_BOOK(tags);
    if ( status.isSuccess() )   {
      status = declare(idh, nt);
      if ( status.isSuccess() )  {
        log << MSG::INFO << "Booked Column wise HBOOK N tuple with ID:" << idh
            << " \"" << nt->title() 
            << "\" in directory <" << loc << ">" << endreq;
        log << MSG::DEBUG;
        if ( log.isActive() ) ::HPRINT (idh);
        return StatusCode::SUCCESS;
      }
    }
    log << MSG::ERROR << "Column wise HBOOK N tuple " << idh
        << "\"" << nt->title() 
        << "\" cannot be booked"
        << " in directory <" << loc << ">" << endreq;
    return status;
  }
  log << MSG::ERROR << "Column wise HBOOK N tuple " << idh << "\"" << nt->title() 
      << "\" already exists in directory <" << loc << ">" << endreq;
  return StatusCode::FAILURE;
}


/// Write N tuple data
StatusCode HbookCnv::HCWNTupleCnv::writeData(long idh, INTuple* nt)   {
  const INTuple::ItemContainer& cols = nt->items();
  MsgStream log(msgSvc(), "HCWNTupleCnv");
  //  log << MSG::DEBUG << "in HCWNTupleCnv::writeData" << endreq;
  char * tar = nt->buffer();
  for (INTuple::ItemContainer::const_iterator i = cols.begin(); i != cols.end(); i++ )   {
    switch( (*i)->type() )   {
    case DataTypeInfo::BOOL:              // bool
      tar += saveItem(tar, (bool*)(*i)->buffer(),   (*i)->length());
      break;
    case DataTypeInfo::CHAR:              // char 
      tar += saveItem(tar, (char*)(*i)->buffer(),   (*i)->length());
      break;
    case DataTypeInfo::SHORT:             // short
      tar += saveItem(tar, (short*)(*i)->buffer(),  (*i)->length());
      break;
    case DataTypeInfo::INT:             // short
      tar += saveItem(tar, (int*)(*i)->buffer(),    (*i)->length());
      break;
    case DataTypeInfo::LONG:             // short      
      tar += saveItem(tar, (long*)(*i)->buffer(),   (*i)->length());
      break;
    case DataTypeInfo::UCHAR:             // unsigned char 
      tar += saveItem(tar, (unsigned char*)(*i)->buffer(),  (*i)->length());
      break;
    case DataTypeInfo::USHORT:            // unsigned short
      tar += saveItem(tar, (unsigned short*)(*i)->buffer(), (*i)->length());
      break;
    case DataTypeInfo::UINT:            // unsigned short
      tar += saveItem(tar, (unsigned int*)(*i)->buffer(),   (*i)->length());
      break;
    case DataTypeInfo::ULONG:            // unsigned short
      tar += saveItem(tar, (unsigned long*)(*i)->buffer(),  (*i)->length());
      break;
    case DataTypeInfo::FLOAT:            // unsigned short
      tar += saveItem(tar, (float*)(*i)->buffer(), (*i)->length());
      break;
    case DataTypeInfo::DOUBLE:            // unsigned short
      tar += saveItem(tar, (double*)(*i)->buffer(), (*i)->length());
      break;
    default:
      break;
    }
  }
  ::HFNT ( idh );
  nt->reset();
  return StatusCode::SUCCESS;
}


/// Read N tuple data
StatusCode HbookCnv::HCWNTupleCnv::readData(long idh, INTuple* ntup, long ievt)    {
  MsgStream log(msgSvc(), "HCWNTupleCnv");

  long istat = 0;
  ievt++;
  ::HGNT(idh, ievt, istat);
  if ( istat == 0 )   {
    INTuple::ItemContainer& cols = ntup->items();
    char * src = ntup->buffer();
    for (INTuple::ItemContainer::iterator i = cols.begin(); i != cols.end(); i++ )   {
      switch( (*i)->type() )   {
      case DataTypeInfo::BOOL:              // bool
        src += loadItem(src, (bool*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::CHAR:              // char 
        src += loadItem(src, (char*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::SHORT:             // short
        src += loadItem(src, (short*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::INT:             // short
        src += loadItem(src, (int*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::LONG:             // short      
        src += loadItem(src, (long*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::UCHAR:             // unsigned char 
        src += loadItem(src, (unsigned char*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::USHORT:            // unsigned short
        src += loadItem(src, (unsigned short*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::UINT:            // unsigned short
        src += loadItem(src, (unsigned int*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::ULONG:            // unsigned short
        src += loadItem(src, (unsigned long*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::FLOAT:            // unsigned short
        src += loadItem(src, (float*)(*i)->buffer(), (*i)->length());
        break;
      case DataTypeInfo::DOUBLE:            // unsigned short
        src += loadItem(src, (double*)(*i)->buffer(), (*i)->length());
        break;
      default:
        break;
      }
    }
    return StatusCode::SUCCESS;
  }

  log << MSG::ERROR << "Error reading data from N tuple ID:" << idh
      << " " << ntup->title() 
      << endreq;
  return StatusCode::FAILURE;
}


/// Create the transient representation of an object.
StatusCode HbookCnv::HCWNTupleCnv::load(long idh, INTuple*& refpObject)   {

  MsgStream log(msgSvc(), "HCWNTupleCnv");

  NTUPLEINFO tags;

  memset(&tags,0,sizeof(tags));

  std::vector<std::string> blkname;
  std::vector<unsigned long> bufpos;
  unsigned long totsize = 0;

  tags.id = idh;
  StatusCode status = ::CWNT_INFO(tags);

  if ( status.isSuccess() )     {
    NTuple::Tuple* pObj = 0;
    status = m_ntupleSvc->create(CLID_ColumnWiseTuple, tags.title, pObj);
    INTuple* ntup = dynamic_cast<INTuple*>(pObj);
    try {
      if ( status.isSuccess() && 0 != ntup )   {
        long total_size = 0;
        for ( long size = 0, i = 0; i < tags.numVar; i++, size=0 )   {
          INTupleItem* item = 0;
          switch( tags.type[i] )    {
          case 'L':
            item = createNTupleItem(tags, i, ntup, NTuple::Range<bool>::min(), NTuple::Range<bool>::max(), size);
          break;
          case 'I':
            // if ( tags.size[i] <= sizeof(unsigned char)*CHAR_BIT )
            //   item = createNTupleItem(tags, i, ntup, NTuple::Range<char>::min(), NTuple::Range<char>::max(), size);
            // else if ( tags.size[i] <= sizeof(unsigned short)*CHAR_BIT )
            //   item = createNTupleItem(tags, i, ntup, NTuple::Range<short>::min(), NTuple::Range<short>::max(), size);
            // else if ( tags.size[i] <= sizeof(unsigned int)*CHAR_BIT )
            //   item = createNTupleItem(tags, i, ntup, NTuple::Range<int>::min(), NTuple::Range<int>::max(), size);
            // else 
            item = createNTupleItem(tags, i, ntup, NTuple::Range<long>::min(), NTuple::Range<long>::max(), size);
            break;
          case 'U':
            // if ( tags.size[i] <= sizeof(unsigned char)*CHAR_BIT )
            //   item = createNTupleItem(tags, i, ntup, NTuple::Range<unsigned char>::min(), NTuple::Range<unsigned char>::max(), size);
            // else if ( tags.size[i] <= sizeof(unsigned short)*CHAR_BIT )
            //   item = createNTupleItem(tags, i, ntup, NTuple::Range<unsigned short>::min(), NTuple::Range<unsigned short>::max(), size);
            // else if ( tags.size[i] <= sizeof(unsigned int)*CHAR_BIT )
            //   item = createNTupleItem(tags, i, ntup, NTuple::Range<unsigned int>::min(), NTuple::Range<unsigned int>::max(), size);
            // else 
            item = createNTupleItem(tags, i, ntup, NTuple::Range<unsigned long>::min(), NTuple::Range<unsigned long>::max(), size);
            break;
          case 'R':
            if ( (size_t)tags.size[i] <= sizeof(float)*CHAR_BIT )
              item = createNTupleItem(tags, i, ntup, NTuple::Range<float>::min(), NTuple::Range<float>::max(), size);
            else
              item = createNTupleItem(tags, i, ntup, NTuple::Range<double>::min(), NTuple::Range<double>::max(), size);
            break;
          default:
            break;
    }
    if ( item ) {
      long ii;
      ntup->add(item);
      total_size += size;
      log << MSG::DEBUG << "Create Var[" << i << "]: " 
	  << item->typeName() << "[";
      for ( ii = 0; ii < item->ndim()-1; ii++ )    {
	log << item->dim(ii) << ", ";
      }
      if ( item->hasIndex() ) log << item->index() << "]";
      else                    log << item->dim(item->ndim()) << "]";
    } else {
      log << MSG::FATAL << "Cannot deduce type for Var[" << i << "]: ";
    }
    

    // add sizes up
    blkname.push_back(tags.block[i]);
    bufpos.push_back(totsize);
    totsize += size;


    log << " Name:"  << tags.name[i]
	<< " Block: " << tags.block[i]
        << " Index:<" << tags.index[i] <<">"
        << " Dim:"   << tags.dim[i]
        << " Typ:"   << tags.type[i]
        << " Size:"  << tags.size[i]
        << " [";
    if ( tags.type[i] == 'I' || tags.type[i] == 'U' || tags.type[i] == 'L' )
      log << tags.irange[i][0] << "," << tags.irange[i][1];
    else if ( tags.type[i] == 'R' )
      log << tags.frange[i][0] << "," << tags.frange[i][1];
    log << "]" << endreq;
  }
  log << MSG::DEBUG << "Total buffer size of NTuple: " << total_size << " Bytes." << endreq; 

  ntup->setBuffer(new char[total_size]);
  long ievt = 0;

  ::HBNAME(idh," ", &ievt, "$CLEAR");

  log << MSG::DEBUG << "Loading block " << blkname[0] << " at offset "
      << bufpos[0] << endreq;

  ::HBNAME(idh,blkname[0].c_str(),ntup->buffer(),"$SET");

  std::string oldblock = blkname[0];
  for (unsigned int ib=1; ib<bufpos.size(); ++ib) {
    if (blkname[ib] != oldblock) {
      log << MSG::DEBUG << "Loading block " << blkname[ib] << " at offset "
	  << bufpos[ib] << endreq;
      ::HBNAME(idh,blkname[ib].c_str(),(ntup->buffer())+bufpos[ib],"$SET");
      oldblock = blkname[ib];
    }
  }
    
  refpObject = ntup;
  return StatusCode::SUCCESS;
      }
    }
    catch (...) {
    }
  }
  log << MSG::ERROR << "Error loading N tuple ID:" << idh << endreq;
  return StatusCode::FAILURE;
}
