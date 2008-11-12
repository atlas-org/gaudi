// $Id: TESSerializer.cpp,v 1.2 2008/11/12 23:39:47 marcocle Exp $ 

#include "GaudiPython/TESSerializer.h"

// Framework include files
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/GaudiException.h"

// ROOT include files
#include "TROOT.h"
#include "TClass.h"
#include "TInterpreter.h"
#include "TBufferFile.h"

#include <map>

namespace {
  struct DataObjectPush {
    DataObjectPush(DataObject*& p) {
      Gaudi::pushCurrentDataObject(&p);
    }
    ~DataObjectPush() {
      Gaudi::popCurrentDataObject();
    }
  };
}

using namespace std;


bool GaudiPython::TESSerializer::analyse(IRegistry* dir, int level)   {
  if ( level < m_currentItem->depth() )   {
    if ( dir->object() != 0 )   {
      m_objects.push_back(dir->object());
      return true;
    }
  }
  return false;
}

GaudiPython::TESSerializer::TESSerializer( IDataProviderSvc* svc)
  : m_TES(svc) 
{
  m_TESMgr = dynamic_cast<IDataManagerSvc*>(svc);
}


TBufferFile* GaudiPython::TESSerializer::dumpBuffer() {
  StatusCode status;
  DataObject* obj;

  static TBufferFile buffer(TBuffer::kWrite);	/* create a buffer */
  static map<string, TClass*> classes;	/* create an STL map to relate string and TClass */

  // Clear current selection 
  m_objects.erase(m_objects.begin(), m_objects.end());
  

  // Traverse the tree and collect the requested objects
  for ( Items::iterator i = m_itemList.begin(); i != m_itemList.end(); i++ )    {
    m_currentItem = (*i);
    status = m_TES->retrieveObject(m_currentItem->path(), obj);
    if ( status.isSuccess() )  {
      m_TESMgr->traverseSubTree(obj, this);
    }
    else  {
      string text("Cannot serialize mandatory object(s) ");
      throw GaudiException(text + m_currentItem->path(), "", status);
    }
  }
  // Traverse the tree and collect the requested objects (tolerate missing itmes here)
  for ( Items::iterator i = m_optItemList.begin(); i != m_optItemList.end(); i++ )    {
    m_currentItem = (*i);
    status= m_TES->retrieveObject(m_currentItem->path(), obj);
    if ( status.isSuccess() )  {
      m_TESMgr->traverseSubTree(obj, this);
    }
  }

  buffer.Reset();
  buffer.WriteInt(m_objects.size());

  for(Objects::iterator i = m_objects.begin(); i != m_objects.end(); ++i) {    
    DataObject* pObj = (*i);	/* define pointer !pObj! to a data object */
    DataObjectPush p(pObj);		/* add the data object to the list... */

    // We build a map so gROOT has to access the whole class database as little as possible
    TClass* cl;						/* announce a TClass */
    const type_info& objClass = typeid(*pObj);		/* get the type of the data object */
    string objClassName = System::typeinfoName(objClass);	/* and then get the descriptive string from System */

    /* Since we're dealing with a list of objects, we map each one, but if objects are the same, we note
       that they've already been mapped (ie the first 'if case below') */
    if (classes[objClassName])   {
      cl=classes[objClassName];
    } else {
      /* if a class has not been mapped: */
      /* pull the class name from the objects c_str() method */
      const char* clName = objClassName.c_str();
      /* and find the relevant Tclass (cl) in gROOT */
      cl = gROOT->GetClass(clName);
      /* ... and fill in the map entry */
      classes[objClassName]=cl;
    }

    /* but what if the object is not mapped because gROOT->GetClass cannot find clName? */
    if (cl==0){
      throw GaudiException("gROOT->GetClass cannot find clName", objClassName, StatusCode::FAILURE);      
    }

    /* note that pObj is a pointer to the data object in question in this for loop over objects */        
    /* so go to the registry->identifier method to retrieve a string representing the location of the object */
    std::string loc=pObj->registry()->identifier();
    /* and write this string (with it's c_str() method) to the Raw Buffer */
    buffer.WriteString(loc.c_str());
    /* using the TClass method GetName, write the name string to the buffer */
    buffer.WriteString(cl->GetName());
    /* and finally, stream the object into the buffer using the TClass method Streamer */
    //buffer.WriteObjectAny(pObj,cl);
    cl->Streamer(pObj,buffer);    
    /* Now the object may contain link data which we don't want to lose; so we have to create an instance of 
       LinkManager from the Data Object's linkMgr function to take care of this data */
    LinkManager* linkMgr = pObj->linkMgr();
    /* get the count of links... */
    int numLinks = linkMgr->size();
    /* put it in the buffer */
    buffer.WriteInt(numLinks);
    /* now link over the number of links and pull the string for each link from the linkMgr tool and write to buffer */
    for (int it = 0; it != numLinks; it++)        {
      const string& link = linkMgr->link(it)->path();
      buffer.WriteString(link.c_str());
    }
  }
  return &buffer;
}

/* ----------- define loads --------------------- */
void GaudiPython::TESSerializer::loadBuffer(TBufferFile* in_buffer) {
  
  // reverse mechanism of dumps
  // buffer is: length of DataObjects vector, location string, type name string, the object itself, num of links, list of links
  
  // constructor from Root
  TBufferFile buffer(TBuffer::kRead, in_buffer->Length(), in_buffer->Buffer(), false);	/* create a buffer */

  int nObjects;
  buffer.ReadInt(nObjects);
  
  for (int i=0; i<nObjects; ++i) {        
    char text[4096];
    buffer.ReadString(text,sizeof(text));
    string location(text);
    buffer.ReadString(text,sizeof(text));
    TClass* cl = gROOT->GetClass(text);
    
    /// The next is equivalent to ReadObjectAny(cl) except of the 'magic!!'        
    DataObject* obj = (DataObject*)cl->New();
    DataObjectPush push(obj); // This is magic!
    
    cl->Streamer(obj, buffer);
    
    if ( obj ) { // Restore links
      int nlink = 0;
      LinkManager* lnkMgr = obj->linkMgr();
      buffer.ReadInt(nlink);
      
      for (int i=0; i<nlink; ++i) {
        buffer.ReadString(text,sizeof(text));
        lnkMgr->addLink(text,0);
      }
    }
    StatusCode sc;
    sc = m_TES->registerObject(location, obj);
    if (sc.isFailure()) {
      if ( location == "/Event" ) {
        sc = m_TESMgr->setRoot(location, obj);
        if(sc.isFailure()) {
          string text("Cannot set root at location ");
          throw GaudiException(text+location,"", sc);
        } 
      }
      else {
        string text("Cannot register object at location ");
        throw GaudiException(text+location,"", sc);
      }      
    }
  }
}

// Add item to output streamer list
void GaudiPython::TESSerializer::addItem(Items& itms, const std::string& path, int level)   {
  DataStoreItem* item = new DataStoreItem(path, level);
  itms.push_back( item );
}

// Add item to output streamer list
void GaudiPython::TESSerializer::addItem(const std::string& path, int level)   {
  addItem( m_itemList, path, level);
}

// Add item to output streamer list
void GaudiPython::TESSerializer::addOptItem(const std::string& path, int level)   {
  addItem( m_optItemList, path, level);
}
