// $Id: SmartIF.h,v 1.9 2007/12/12 16:02:31 marcocle Exp $
#ifndef GAUDI_SMARTIF_H
#define GAUDI_SMARTIF_H 1

// Framework include files
#include "GaudiKernel/IInterface.h"

// Forward declarations
template <class TYPE> class SmartIF;
template <> class SmartIF<IInterface>;
template <class TYPE> class SmartIFBase;

/**
 * @class SmartIF SmartIF.h GaudiKernel/SmartIF.h
 *
 * Smart pointer base class to handle easily interfaces
 *
 * Description:
 * A small class to easy the use of Gaudi interfaces. This class acts as
 * a base class for the specific instances SmartIF<TYPE>.
 * The base class only defines operations on the valid smart pointer.
 *
 * Dependencies:
 * <UL>
 * <LI> Gaudi Interface definition:        Gaudi/Interfaces/IInterface.h
 * </UL>
 *
 * Author:  M.Frank
 * @author Sebastien Ponce
 */
template <class TYPE> class SmartIFBase   {
protected:
  const InterfaceID  m_iid;
  TYPE*              m_interface;
  /// Release interface
  void releaseInterface()   {
    if ( m_interface ) m_interface->release();
    m_interface = 0;
  }
  /// Standard constructor
  SmartIFBase(const InterfaceID& iid)  :  m_iid(iid), m_interface(0)      {
  }
  /// Standard Destructor
  virtual ~SmartIFBase()    {
    releaseInterface();
  }
public:
  /// Allow for check if smart pointer is valid
  bool isValid()   const   {
    return ( 0 != m_interface );
  }
  /// Automatic conversion to constined interface type
  operator TYPE* ()   {
    return m_interface;
  }
  /// Automatic conversion to constined interface type (CONST)
  operator const TYPE* ()  const  {
    return m_interface;
  }
  /// Check for non-equality
  bool operator !=(const TYPE* test)   const   {
    return test != m_interface;
  }
  /// Check for equality
  bool operator ==(const TYPE* test)   const   {
    return test == m_interface;
  }
  /// Dereference operator
  TYPE* operator->()    {
    return m_interface;
  }
  /// Dereference operator (CONST)
  const TYPE* operator->()  const  {
    return m_interface;
  }
  /// Allow for use as void pointer
  void** operator & ()   {
    return (void**) &m_interface;
  }
  /// Get reference to interface pointer
  const TYPE* const get() {
    return m_interface;
  }
  /// Get reference to interface pointer
  TYPE*& pRef() {
    return this->m_interface;
  }
  /// Get reference to interface pointer (CONST)
  const TYPE* const & pRef() const  {
    return this->m_interface;
  }
  /// check the validity of the interface 
  bool operator!() const { return !(this->isValid()) ; }
private:
  // disable copy 
  SmartIFBase( const SmartIFBase& copy ); ///< disable copy 
  // disable assignement
  SmartIFBase& operator=( const SmartIFBase& right ) ; ///< disable assignement
};

/** Smart pointer to handle easily interfaces

    Description:
    A small class to easy the use of Gaudi interfaces.

    Base Class:
    SmartIFBase<TYPE>

    <P> History    :
    <PRE>
    +---------+----------------------------------------------+--------+
    |    Date |                 Comment                      | Who    |
    +---------+----------------------------------------------+--------+
    | 30/10/99| Initial version.                             | MF     |
    +---------+----------------------------------------------+--------+
    </PRE>
    Author:  M.Frank
    Version: 1.0
*/
template <class TYPE> class SmartIF : public SmartIFBase<TYPE>  {
public:
  /// Standard constructor with initialisation
  SmartIF(const InterfaceID& iid, IInterface* iface) : SmartIFBase<TYPE>(iid)   {
    SmartIF<TYPE>::operator=(iface);
  }
  /// Standard constructor with initialisation
  SmartIF(const InterfaceID& iid, TYPE* iface) : SmartIFBase<TYPE>(iid)    {
    SmartIF<TYPE>::operator=(iface);
  }
  /// Standard constructor with initialisation
  SmartIF(const InterfaceID& iid) : SmartIFBase<TYPE>(iid)    {
  }
  /// Standard constructor with initialisation
  SmartIF ( TYPE* iface ) : SmartIFBase<TYPE>(TYPE::interfaceID())    {
    SmartIF<TYPE>::operator=(iface);
  }
  /// Standard constructor with initialisation
  SmartIF(IInterface* iface = 0) : SmartIFBase<TYPE>(TYPE::interfaceID())    {
    SmartIF<TYPE>::operator=(iface);
  }
  /// Copy constructor
  SmartIF( const SmartIF<TYPE>& copy) : SmartIFBase<TYPE>(copy.m_iid)  {
    SmartIF<TYPE>::operator=(copy.m_interface);
  }
  /// Standard destructor
  virtual ~SmartIF()    {
  }
  /// Simple assignment operator
  SmartIF<TYPE>& operator = (int /* iface = 0*/)   {
    this->releaseInterface();
    this->m_interface = 0;
    return *this;
  }
  /// Copy assignment operator with conversion to requested interface type
  template <class T> 
  SmartIF<TYPE>& operator = ( const SmartIF<T>& iface )   
  {
    T* ptr = iface.m_interface ;
    if ( (void*)ptr != (void*)this->m_interface )   
    {
      TYPE* newIF = 0;
      if ( ptr != 0 )   {
        ptr->queryInterface(this->m_iid, pp_cast<void>(&newIF)).ignore();
      }
      this->releaseInterface();
      this->m_interface = newIF;
    }
    return *this;
  }
  /// Copy assignment operator 
  SmartIF<TYPE>& operator = ( const SmartIF<TYPE>& iface)   
  {
    TYPE* newIF = iface.m_interface ;
    return (*this)=newIF ;
  }
  /// Assignment operator with conversion to requested interface type
  SmartIF& operator = (IInterface* iface)   
  {
    if ( iface != this->m_interface )   
    {
      TYPE* newIF = 0;
      if ( iface != 0 )   {
        iface->queryInterface(this->m_iid, pp_cast<void>(&newIF)).ignore();
      }
      this->releaseInterface();
      this->m_interface = newIF;
    }
    return *this;
  }
  /// Simple assignment operator
  SmartIF<TYPE>& operator = (TYPE* iface)   
  {
    if ( iface != this->m_interface )   {
      if ( iface != 0 )   {
        iface->addRef();
      }
      this->releaseInterface();
      this->m_interface = iface;
    }
    return *this;
  }
  /// check the validity of the interface 
  bool operator!() const { return !(this->isValid()) ; }
};

/** Smart pointer to handle IInterface interfaces

    Description:
    The smart pointer to IInterface is a little bit special, 
    mainly because the IInterface is contained in any other interface.
    Hence, the exported methodes are only a subset of the 
    generic template.

    Base Class:
    SmartIFBase<TYPE>

    <P> History    :
    <PRE>
    +---------+----------------------------------------------+--------+
    |    Date |                 Comment                      | Who    |
    +---------+----------------------------------------------+--------+
    | 30/10/99| Initial version.                             | MF     |
    +---------+----------------------------------------------+--------+
    </PRE>
    Author:  M.Frank
    Version: 1.0
*/
template <> class SmartIF<IInterface> : public SmartIFBase<IInterface>  {
public:
  typedef IInterface TYPE;
  /// Copy constructor
  SmartIF( const SmartIF<IInterface>& copy) 
    : SmartIFBase<IInterface>(copy.m_iid)
  {
    SmartIF<TYPE>::operator=(copy.m_interface);
  }
  /// Standard constructor with initialisation
  SmartIF(IInterface* iface = 0) 
  : SmartIFBase<IInterface>(IID_IInterface)    
  {
    SmartIF<TYPE>::operator=(iface);
  }
  /// Constructor from other SmartIF
  template <class T>
  SmartIF( const SmartIF<T>& right ) 
    : SmartIFBase<IInterface>(IID_IInterface)
  {
    SmartIF<TYPE>::operator=(right.m_interface);
  }
  /// Standard Destructor
  virtual ~SmartIF()    {
  }
  /// Copy assignment operator with conversion to requested interface type
  template <class T> 
  SmartIF<TYPE>& operator = ( const SmartIF<T>& iface)  
  {
    T* ptr = iface.m_interface ;
    if ( (void*)ptr != (void*)this->m_interface )   {
      TYPE* newIF = 0;
      if ( ptr != 0 )   {
        ptr->queryInterface(this->m_iid, (void**)&newIF).ignore();
      }
      this->releaseInterface();
      this->m_interface = newIF;
    }
    return *this;
  }
  /// Copy assignment operator
  SmartIF<TYPE>& operator = ( const SmartIF<TYPE>& iface)   {
    TYPE* newIF = iface.m_interface ;
    return (*this)=newIF ;
  }
  /// Assignment operator 
  SmartIF<TYPE>& operator = (TYPE* iface)   {
    if ( iface != m_interface )   {
      if ( iface != 0 )   {
        iface->addRef();
      }
      releaseInterface();
      m_interface = iface;
    }
    return *this;
  }
  /// check the validity of the interface 
  bool operator!() const { return !(this->isValid()) ; }
};

#endif // GAUDI_SMARTIF_H
