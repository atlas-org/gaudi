// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/Converter.h,v 1.10 2004/09/11 00:22:41 leggett Exp $
#ifndef GAUDIKERNEL_CONVERTER_H
#define GAUDIKERNEL_CONVERTER_H

// generic experiment headers
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ConversionSvc.h"

// Forward declarations
class IMessageSvc;
class IRegistry;
class IDataProviderSvc;
class IDataManagerSvc;

/** @class Converter Converter.h GaudiKernel/Converter.h

    Converter base class. See interface for detailed description, 
    arguments and return values

    @author Markus Frank
    @version 1.0
*/
class Converter : virtual public IConverter	    {

public:

  /// Query interfaces of Interface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// Reference Interface instance
  virtual unsigned long addRef();

  /// Release Interface instance 
  virtual unsigned long release();

  /// Initialize the converter
  virtual StatusCode initialize();

  /// Initialize the converter
  virtual StatusCode finalize();

  /// Set Data provider service
  virtual StatusCode setDataProvider(IDataProviderSvc* svc);

  /// Get Data provider service
  virtual IDataProviderSvc* dataProvider()    const;

  /// Set conversion service the converter is connected to
  virtual StatusCode setConversionSvc(IConversionSvc* svc);

  /// Get conversion service the converter is connected to
  virtual IConversionSvc* conversionSvc()    const;

  /// Set address creator facility
  virtual StatusCode setAddressCreator(IAddressCreator* creator);

  /// Retrieve address creator facility
  virtual IAddressCreator* addressCreator()   const;

  /// Retrieve the class type of objects the converter produces. 
  virtual const CLID& objType() const;

  /// Retrieve the class type of the data store the converter uses.
  // MSF: Masked to generate compiler error due to interface change
  virtual long i_repSvcType() const;

  /// Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress,DataObject*& refpObject);

  /// Resolve the references of the created transient object.
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);

  /// Update the references of an updated transient object.
  virtual StatusCode updateObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);

  /// Resolve the references of the converted object. 
  virtual StatusCode fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject);

  /// Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Update the references of an already converted object.
  virtual StatusCode updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Standard Constructor
  Converter(long storage_type, const CLID& class_type, ISvcLocator* svc);

  /// Access a service by name, creating it if it doesn't already exist.
  template <class T>
  StatusCode service( const std::string& name, T*& psvc, bool createIf = false ) const {
    return service_i(name, createIf, T::interfaceID(), (void**)&psvc);
  }

  /// Access a service by name, type creating it if it doesn't already exist.
  template <class T>
  StatusCode service( const std::string& type, const std::string& name, T*& psvc) const {
    return service_i(type, name, T::interfaceID(), (void**)&psvc);
  }

protected:

  /// Standard Destructor
  virtual ~Converter();

  /// Retrieve pointer to service locator
  ISvcLocator* serviceLocator()   const;
  /// Retrieve pointer to message service
  IMessageSvc* msgSvc()   const;
  // Obsoleted name, kept due to the backwards compatibility
  IMessageSvc* messageService()   const;
  /// Get Data Manager service
  IDataManagerSvc* dataManager() const;

private:

  /// Reference counter                          
  unsigned long     m_refCount;
  /// Storage type
  long              m_storageType;
  /// Class type the converter can handle
  const CLID        m_classType;
  /// Pointer to the address creation service interface
  IAddressCreator*    m_addressCreator;
  /// Pointer to data provider service
  IDataProviderSvc* m_dataProvider;
  /// Pointer to data manager service
  IDataManagerSvc*  m_dataManager;
  /// Pointer to the connected conversion service
  IConversionSvc*   m_conversionSvc;
  /// Service Locator reference
  ISvcLocator*      m_svcLocator;
  /// MessageSvc reference 
  mutable IMessageSvc* m_messageSvc;

  /** implementation of service method */
  StatusCode service_i(const std::string& svcName,
		       bool createIf,
		       const InterfaceID& iid,
		       void** ppSvc) const;
  StatusCode service_i(const std::string& svcType,
		       const std::string& svcName,
		       const InterfaceID& iid,
		       void** ppSvc) const;
};


#endif // GAUDIKERNEL_CONVERTER_H 
