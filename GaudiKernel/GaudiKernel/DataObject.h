// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/DataObject.h,v 1.10 2008/11/12 23:39:47 marcocle Exp $
#ifndef GAUDIKERNEL_DATAOBJECT_H
#define GAUDIKERNEL_DATAOBJECT_H

// Framework include files
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/StatusCode.h"

// STL includes
#include <string>
#include <ostream>

// Forward declarations
class IOpaqueAddress;
class StreamBuffer;
class LinkManager;
class IRegistry;


// Definition of the CLID for this class
static const CLID CLID_DataObject = 1;

/** @class DataObject DataObject.h GaudiKernel/DataObject.h

    A DataObject is the base class of any identifyable object
    on any data store.
    The base class supplies the implementation of data streaming.

    @author M.Frank
*/
class DataObject   {
private:
  /// Reference count
  unsigned long       m_refCount;
  /// Version number
  unsigned char       m_version;
  /// Pointer to the Registry Object
  IRegistry*          m_pRegistry;
  /// Store of symbolic links
  LinkManager*        m_pLinkMgr;

public:
  /// Standard Constructor
  DataObject();
  /// Copy Constructor
  DataObject(const DataObject&);
  /// Standard Destructor
  virtual ~DataObject();
  /// Add reference to object
  virtual unsigned long addRef();
  /// release reference to object
  virtual unsigned long release();
  /// Retrieve reference to class definition structure
  virtual const CLID& clID() const;
  /// Retrieve reference to class definition structure (static access)
  static const CLID& classID();
  /// Retreive DataObject name. It is the name when registered in the store.
  const std::string& name() const;

  /// Serialization mechanism: Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);
  /// Serialization mechanism: Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s)  const;

  /**@name inline code of class DataObject    */
  /// Set pointer to Registry
  void setRegistry(IRegistry* pRegistry)    {
    m_pRegistry = pRegistry;
  }
  /// Get pointer to Registry
  IRegistry* registry() const   {
    return m_pRegistry;
  }
  /// Retrieve Link manager
  LinkManager* linkMgr()    const   {
    return m_pLinkMgr;
  }
  /// Retrieve version number of this object representation
  unsigned char version()    const  {
    return m_version;
  }
  /// Set version number of this object representation
  void setVersion(unsigned char vsn)    {
    m_version = vsn;
  }
  /// Return the refcount
  unsigned long refCount() const {
    return m_refCount;
  }
  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream( std::ostream& s ) const {
    s << "DataObject at " << std::hex << this;
    return s;
  }
  /// Output operator (ASCII)
  friend std::ostream& operator<< ( std::ostream& s, const DataObject& obj ) {
    return obj.fillStream(s);
  }
};

// Additional functions to support the Serialialization of objects in the transient store 

namespace Gaudi 
{  
  void pushCurrentDataObject(DataObject** pobjAddr);
  void popCurrentDataObject();
  DataObject* getCurrentDataObject();
}


#endif // GAUDIKERNEL_DATAOBJECT_H
