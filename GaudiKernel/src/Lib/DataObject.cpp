// $Id: DataObject.cpp,v 1.8 2005/01/19 18:31:15 mato Exp $

// Experiment specific include files
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IInspector.h"
#include "GaudiKernel/IRegistry.h"

static std::string _sDataObjectCppNotRegistered("NotRegistered");

/// Standard Constructor
DataObject::DataObject()
 : m_refCount(0),
   m_version(0),
   m_pRegistry(0)
{
  m_pLinkMgr = LinkManager::newInstance();
}

/// Standard Constructor
DataObject::DataObject(const DataObject&)
 : m_refCount(0),
   m_version(0),
   m_pRegistry(0)
{
  m_pLinkMgr = LinkManager::newInstance();
}

/// Standard Destructor
DataObject::~DataObject()   {
  // Issue a warning if the object is being deleted and the reference
  // count is non-zero.
  if ( m_refCount > 0 ) {
    // Insert warning here
  }
  if ( m_pLinkMgr ) delete m_pLinkMgr;
  m_pLinkMgr = 0;
}

/// Decrease reference count
unsigned long DataObject::release()  {
  unsigned long cnt = --m_refCount;
  if ( 0 == m_refCount )   {
    delete this;
  }
  return cnt;
}

/// Add reference to object
unsigned long DataObject::addRef()  {
  return ++m_refCount;
}

/// Retrieve Pointer to class defininition structure
const CLID& DataObject::clID() const   {
  return CLID_DataObject;
}

/// Retrieve Pointer to class defininition structure
const CLID& DataObject::classID()    {
  return CLID_DataObject;
}

/// Retreive DataObject name. It is the name when included in the store.
const std::string& DataObject::name() const {
  if( m_pRegistry != 0) {
    return m_pRegistry->name();
  }
  else {
    return _sDataObjectCppNotRegistered;
  }
}

/// ISerialize implementation: Serialize the object for reading
StreamBuffer& DataObject::serialize(StreamBuffer& s)   {
  return s >> m_version;
}

/// ISerialize implementation: Serialize the object for writing
StreamBuffer& DataObject::serialize(StreamBuffer& s)  const    {
  return s << m_version;
}
