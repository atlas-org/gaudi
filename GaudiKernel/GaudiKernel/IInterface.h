// $Id: IInterface.h,v 1.14 2008/01/29 08:28:14 marcocle Exp $
#ifndef GAUDIKERNEL_IINTERFACE_H
#define GAUDIKERNEL_IINTERFACE_H

// Include files 

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include <ostream>

/**
 * @class InterfaceID Kernel.h GaudiKernel/Kernel.h
 *
 * Interface ID class. It consists of several fields: interface number, 
 * major version number and minor version number. Interface number should be
 * allocated globally. The major and minor version numbers are used for
 * checking compatibility between interface versions.
 *
 * @author Pere Mato
 * @author Sebastien Ponce
 */
class InterfaceID {
public:
  /// constructor from a pack long
  InterfaceID( unsigned long lid ) : m_id( lid & 0xFFFF ),
                                     m_major_ver( (lid & 0xFF000000)>>24 ),
                                     m_minor_ver( (lid & 0xFF0000)>> 16 ) { }
  /// constructor from components
  InterfaceID( unsigned long id, unsigned long major, unsigned long minor = 0)
    : m_id( id ), m_major_ver( major ), m_minor_ver( minor ) { }
  /// constructor from components
  InterfaceID( const char* name, unsigned long major, unsigned long minor = 0)
    : m_id( hash32(name) ), m_major_ver( major ), m_minor_ver( minor ) { }
  /// destructor
  ~InterfaceID() { }
  /// conversion to unsigned long
  operator unsigned long() const {
    return (m_major_ver << 24) + (m_minor_ver << 16) + m_id;
  }
  /// get the interfac identifier
  unsigned long id() const { return m_id; }
  /// get the major version of the interface
  unsigned long majorVersion() const { return m_major_ver; }
  /// get the minor version of the interface 
  unsigned long minorVersion() const { return m_minor_ver; }
  /** check compatibility. The major version is check and the minor one
   *  should be bigger or equal
   */
  bool versionMatch( const InterfaceID& iid ) const { 
    return ( id() == iid.id() &&
             majorVersion() == iid.majorVersion() &&
             minorVersion() >= iid.minorVersion() );
  }
  /// check full compatibility.
  bool fullMatch( const InterfaceID& iid ) const { 
    return ( id() == iid.id() &&
             majorVersion() == iid.majorVersion() &&
             minorVersion() == iid.minorVersion() );
  }
  /// compare operator
  bool operator == (const InterfaceID& iid ) const { return fullMatch(iid); }
  /// one-at-time hash function
  unsigned int hash32(const char* key) {
    unsigned int hash;
    const char* k;
    for (hash = 0, k = key; *k; k++) {
      hash += *k; hash += (hash << 10); hash ^= (hash >> 6); 
    }
    hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15);
    return hash;
  }
private:
  friend std::ostream& operator << ( std::ostream&, const InterfaceID&);
  unsigned long m_id;
  unsigned long m_major_ver;
  unsigned long m_minor_ver;
};

/// Typedef for backward compatibility on existing code.
// typedef InterfaceID IID; removed: it collides with windows system typename

// Interface ID
static const InterfaceID IID_IInterface(1,0,0);

/** @class IInterface IInterface.h GaudiKernel/IInterface.h
    Definition of the basic interface

    @author Pere Mato
    @author Markus Frank
*/
class IInterface	{
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IInterface; }

  /// destructor
  virtual ~IInterface() { }

  /** Query interfaces of Interface
      @param riid         ID of Interface to be retrieved
      @param ppvInterface Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvInterface) = 0;

  /// Increment the reference count of Interface instance
  virtual unsigned long addRef() = 0;

  /// Release Interface instance
  virtual unsigned long release() = 0;

  /// Return status 
  enum Status   {
    /// Normal successful completion
    SUCCESS = 1,
    /// Requested interface is not available
    NO_INTERFACE,
    /// Requested interface version is incompatible
    VERSMISMATCH,
    /// Last error
    LAST_ERROR
  };
};

/** Templated function that throws an exception if the version if the interface 
    implemented by the object behind the interface is incompatible. This is the
    case if the major version differs or that the minor version of the client is
    bigger than of the implementer.

    @author Pere Mato
*/
template <class I> 
bool isValidInterface( I* i) {
  void* ii;
  StatusCode sc =  i->queryInterface( I::interfaceID(), &ii);
  return sc.isSuccess();
}

/// ostream operator for InterfaceID. Needed by PluginSvc
inline std::ostream& operator << ( std::ostream& s, const InterfaceID& id) {
    s << "IID_" << id.m_id;
    return s;
}

/// Small function to be used instead of the construct (void**)&pointer, which
/// produces, on gcc 4.1 optimized, the warning
/// <quote>warning: dereferencing type-punned pointer will break strict-aliasing rules</quote>
/// The assempler code produced is equivalent to the one with the direct cast.
/// The function can be used als to replace the construct (IInterface *&)m_MySvc.
/// Replacement rules:
/// <table>
/// <tr><td><tt>(void**)&pointer</tt></td><td><tt>pp_cast<void>(&pointer)</tt></td></tr>
/// <tr><td><tt>(IInterface *&)m_mySvc</tt></td><td><tt>*pp_cast<IInterface>(&m_mySvc)</tt></td></tr>
/// </table>
template <class DEST,class SRC>
inline DEST** pp_cast(SRC** ptr){
  return reinterpret_cast<DEST**>(ptr);
}
#endif  // GAUDIKERNEL_IINTERFACE_H
