// $Id: PoolDbIOHandler.cpp,v 1.10 2006/05/23 17:53:33 hmd Exp $
//====================================================================
//
//	Package    : System (The POOL project)
//
//  Description: Generate neccessary infrastructure for the
//               SEAL plugin manager modules
//
//	@author      M.Frank
//====================================================================
#include "GaudiPoolDb/PoolDbIOHandler.h"
#include "GaudiKernel/ObjectContainerBase.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "Reflex/Reflex.h"
#include "RootStorageSvc/IOHandlerStreamer.h"
#include <stdexcept>
#include <iostream>
#include "TROOT.h"
#include "TClass.h"
#include "TStreamerInfo.h"
#include "Cintex/Cintex.h"

static DataObject*       s_objPtr = 0;
static DataObject**      s_currObj = &s_objPtr;
static const DataObject* last_link_object = 0;
static int last_link_hint = -1;

namespace pool  {
  const std::string typeName(const std::type_info& typ);
}

static std::vector<DataObject**>& currentObject() {
  static std::auto_ptr<std::vector<DataObject**> > s_current;
  if ( 0 == s_current.get() )  {
    s_current = std::auto_ptr<std::vector<DataObject**> >(new std::vector<DataObject**>());
  }
  return *(s_current.get());
}

void pushCurrentDataObject(DataObject** pobjAddr) {
  static std::vector<DataObject**>& c = currentObject();
  c.push_back(pobjAddr);
  s_currObj = pobjAddr ? pobjAddr : &s_objPtr;
  last_link_object = 0;
  last_link_hint   = -1;
}
void popCurrentDataObject() {
  static std::vector<DataObject**>& c = currentObject();
  last_link_object = 0;
  last_link_hint   = -1;
  switch(c.size())  {
    case 0:
      s_currObj = c.back();
      c.pop_back();
      break;
    default:
      s_currObj = &s_objPtr;
      break;
  }
}

template <>
void PoolDbIOHandler<SmartRefBase>::onReadUpdate(void* obj) {
  SmartRefBase* ref = (SmartRefBase*)obj;
  // std::cout << "Link:" << ref->m_hintID << " " << ref->m_linkID << std::endl;
  //if ( int(ref->m_hintID) == -1 )  {
  //  std::cout << "PoolDbIOHandler<SmartRefBase>::onRead> "
  //            << "Found invalid smart reference."
  //            << std::endl;
  //}
  switch( ref->objectType() ) {
   case SmartRefBase::DATAOBJECT:
    {
      SmartRef<DataObject>* r = (SmartRef<DataObject>*)obj;
      (*r)(*s_currObj);
    }
    break;
  case SmartRefBase::CONTAINEDOBJECT: 
   {
     SmartRef<ContainedObject>* r = (SmartRef<ContainedObject>*)obj;
     (*r)(*s_currObj);
   }
   break;
  default:
    std::cout << "Hit uninitialized smartRef!!!!" << std::endl;
    break;
  }
}

template <>
void PoolDbIOHandler<SmartRefBase>::onWriteUpdate(void* obj) {
  SmartRefBase* ref = (SmartRefBase*)obj;
  SmartRef<DataObject>* r1 = (SmartRef<DataObject>*)ref;
  DataObject* curr = *s_currObj;
  DataObject* pDO  = r1->data();
  int hint = r1->hintID();
  int link = r1->linkID();
  if ( pDO )  {
    switch( ref->objectType() ) {
    case SmartRefBase::CONTAINEDOBJECT:
      {
        SmartRef<ContainedObject>* r2 = (SmartRef<ContainedObject>*)ref;
        ContainedObject* p = r2->data();
        if ( p )  {
          const ObjectContainerBase* parent = p->parent();
          if ( parent )  {
            link = p->index();
            pDO  = const_cast<ObjectContainerBase*>(parent);
            break;
          }
        }
      }
      pDO = 0;
      std::cout << "PoolDbIOHandler<SmartRefBase>::onWrite> "
                << "Found invalid smart reference with object "
                << "having no parent."
                << std::endl;
      throw std::runtime_error("PoolDbIOHandler<SmartRefBase>::onWrite> "
                               "Found invalid smart reference with object "
                               "having no parent.");
      break;
    case SmartRefBase::DATAOBJECT: 
      link = StreamBuffer::INVALID;
      break;
    default:
      break;
    }
    //if ( pDO == last_link_object && last_link_hint == -1 )  {
    //  std::cout << "PoolDbIOHandler<SmartRefBase>::onWrite> "
    //            << "Found invalid smart reference."
    //            << std::endl;
    //}
    if ( pDO == last_link_object )  {
      ref->set(curr, last_link_hint, link);
      return;
    }
    else if ( pDO ) {
      LinkManager* mgr = curr->linkMgr();
      IRegistry*   reg = pDO->registry();
      if ( reg && mgr )  {
        hint = mgr->addLink(reg->identifier(), pDO);
        last_link_hint   = hint;
        last_link_object = pDO;
      }
    }
    else {
      hint = link = StreamBuffer::INVALID;
    }
  }
  //if ( hint == StreamBuffer::INVALID )  {
  //  std::cout << "PoolDbIOHandler<SmartRefBase>::onWrite> "
  //            << "Found invalid smart reference."
  //            << std::endl;
  //}
  ref->set(curr, hint, link);
}

template <>
void PoolDbIOHandler<ContainedObject>::onReadUpdate(void* obj) {
  ContainedObject* p = (ContainedObject*)obj;
  p->setParent((ObjectContainerBase*)*s_currObj);
}

template <>
void PoolDbIOHandler<ContainedObject>::onWriteUpdate(void* /* obj */) {
}

template <class T> static bool makeStreamer(MsgStream& log)  {
  std::string cl_name = pool::typeName(typeid(T));
  ROOT::Reflex::Type t = ROOT::Reflex::Type::ByName(cl_name);
  if ( t )  {
    TClass* c = gROOT->GetClass(cl_name.c_str());
    if ( c ) {
      pool::IIOHandler* h = new PoolDbIOHandler<T>(t);
      TClassStreamer*   s = new pool::IOHandlerStreamer(t, c, h);
      c->AdoptStreamer(s);
      log << MSG::DEBUG << "Installed IOHandler for class " << cl_name << endmsg;
      return true;
    }
    else if ( !c )  {
      log << MSG::ERROR << "[No ROOT TClass] Failed to install IOHandler for class " << cl_name << endmsg;
    }
  }
  else  {
    log << MSG::ERROR << "[No Reflection Type] Failed to install IOHandler for class " << cl_name << endmsg;
  }
  return false;
}

#include "RootStorageSvc/CINTTypedefs.h"
#include "RootStorageSvc/ICINTStreamer.h"
#include "RootStorageSvc/ICINTStreamerFactory.h"
#include "RootStorageSvc/RefStreamer.h"
#include "RootStorageSvc/IOHandlerCustomStreamer.h"
#include "RootStorageSvc/IOHandlerStreamer.h"
#include "RootStorageSvc/CustomStreamer.h"
#include "StorageSvc/IIOHandlerFactory.h"
#include "StorageSvc/IIOHandler.h"
#include "StorageSvc/DbReflex.h"


using namespace ROOT::Reflex;

namespace {
  /// get access to the user supplied streamer function (if any)
  pool::ICINTStreamer* streamer(Type& cl)  {
    // First look for custom streamers; 
    // if there are none, check for standard streamers
    if ( cl )   {
      if ( !cl.IsFundamental() )  {
	pool::ICINTStreamerFactory* f = pool::ICINTStreamerFactory::get();
        if ( f ) {
          std::string typ = pool::DbReflex::fullTypeName(cl);
          if ( typ.substr(0,2) == "::" ) typ.replace(0,2,"");
          std::string nam = "CINTStreamer<"+typ+">";
          size_t occ;
          // Replace namespace "::" with "__"
          while ( (occ=nam.find("::")) != std::string::npos )    {
            nam.replace(occ, 2, "__");
          }
          return f->create(nam, cl);
        }
      }
    }
    return 0;
  }

  pool::IIOHandler* ioHandler(Type& cl)  {
    pool::IIOHandlerFactory* f = pool::IIOHandlerFactory::get();
    if ( f ) {
      size_t idx;
      std::string typ = pool::DbReflex::fullTypeName(cl);
      if ( typ.substr(0,2) == "::" ) typ.replace(0,2,"");
      std::string nam = "IIOHandler<"+typ+">";
      // Replace namespace "::" with "__"
      while ( (idx=nam.find("::")) != std::string::npos )    {
        nam.replace(idx, 2, "__");
      }
      return f->create(nam, cl);
    }
    return 0;
  }
}

namespace ROOT { namespace Cintex  {
  bool IsSTLinternal(const std::string& nam);
  bool IsSTL(const std::string& nam);
  bool IsSTLext(const std::string& nam);
  bool IsTypeOf(Type& typ, const std::string& base_name);
  Type CleanType(const Type& t);
  /// Retrieve CINT class name (in Type.cpp)
  std::string CintName(const Type&);
  std::string CintName(const std::string&);
}}
using namespace ROOT::Cintex;

namespace GaudiPoolDb  {
  bool patchStreamers(MsgStream& s)  {
    static bool first = true;
    if ( first ) {
      first = false;
      //Cintex::setDebug(2);
      for ( Type_Iterator i=Type::Type_Begin(); i != Type::Type_End(); ++i)   {
        std::auto_ptr<TClassStreamer> str;
        Type typ = *i;
        if ( !(typ.IsStruct() || typ.IsClass()) )  continue;
	std::string nam = typ.Name(SCOPED);
        TClass* root_class = 0;
	if ( IsTypeOf(typ,"pool::Reference") )  {
	  root_class = gROOT->GetClass(nam.c_str());
	  str = std::auto_ptr<TClassStreamer>(new pool::RefStreamer(typ, root_class, 1));
	}
	else if ( IsTypeOf(typ,"pool::Token") )  {
	  root_class = gROOT->GetClass(nam.c_str());
	  str = std::auto_ptr<TClassStreamer>(new pool::RefStreamer(typ, root_class, 2));
	}
	else   {
	  pool::ICINTStreamer* s = streamer(typ);
	  pool::IIOHandler*    h = ioHandler(typ);
	  if ( s && h )  {
	    root_class = gROOT->GetClass(nam.c_str());
	    str = std::auto_ptr<TClassStreamer>(new pool::IOHandlerCustomStreamer(typ, root_class, h, s));
	  }
	  else if ( s )  {
	    root_class = gROOT->GetClass(nam.c_str());
	    str = std::auto_ptr<TClassStreamer>(new pool::CustomStreamer(typ, root_class, s));
	  }
	  else if ( h )  {
	    root_class = gROOT->GetClass(nam.c_str());
	    str = std::auto_ptr<TClassStreamer>(new pool::IOHandlerStreamer(typ, root_class, h));
	  }
	  else   {
	    //	    root_class->SetBit(TClass::kIsForeign);
	  }
	}
	/*
	for (size_t i = 0; i < typ.DataMemberSize(); i++) {
	  Type t = CleanType(typ.DataMemberAt(i).TypeOf());
	  if ( (t.IsClass() || t.IsStruct()) && t.IsAbstract() )  {
	    for (size_t j=0, n=Type::TypeSize(); j<n; j++) {
	      Type t1 = Type::TypeAt(j);
	      if ( (t1.IsClass() || t1.IsStruct()) && !t1.IsAbstract() )  {
		if ( t1.HasBase(t) )  {
		  std::string s = CintName(t1);
		  TClass* c = gROOT->GetClass(s.c_str(), kTRUE);
		  if ( c )  {
		    c->GetStreamerInfo();
		    break;
		  }
		}
	      }
	    }
	  }
	}
	*/
	if ( str.get() )  {
	  if ( !root_class ) {
	    continue;
	  }
	  root_class->AdoptStreamer(str.release());
	}
      }
      ROOT::Cintex::Cintex::Enable();      
      bool b2 = makeStreamer<SmartRefBase>(s);
      bool b3 = makeStreamer<ContainedObject>(s);
      return b2 && b3;
    }
    return true;
   }
}
