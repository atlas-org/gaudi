// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/src/Lib/Converter.cpp,v 1.17 2007/12/12 16:02:32 marcocle Exp $

// Include Files
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/ServiceLocatorHelper.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/ConversionSvc.h"

//--- IInterface::addRef
unsigned long Converter::addRef()   {
  m_refCount++;
  return m_refCount;
}

//--- IInterface::release
unsigned long Converter::release()   {
  long count = --m_refCount;
  if ( count <= 0 ) {
    delete this;
  }
  return count;
}

//--- IInterface::queryInterface
StatusCode Converter::queryInterface(const InterfaceID& riid, void** ppvInterface)  {
  if ( IID_IInterface.versionMatch(riid) )   {
    *ppvInterface = (IInterface*)this;
  }
  else if ( IID_IConverter.versionMatch(riid) )  {
    *ppvInterface = (IConverter*)this;
  }
  else   {
     return NO_INTERFACE;
  }
  addRef();
  return StatusCode::SUCCESS;
}

/// Retrieve the class type of objects the converter produces. 
const CLID& Converter::objType() const    {
  return m_classType;
}

/// Retrieve the class type of the data store the converter uses.
long Converter::i_repSvcType() const   {
  return m_storageType;
}

/// Create the transient representation of an object.
StatusCode Converter::createObj(IOpaqueAddress*, DataObject*&)   {
  return StatusCode::SUCCESS;
}


/// Resolve the references of the created transient object.
StatusCode Converter::fillObjRefs(IOpaqueAddress*, DataObject*)    {
  return StatusCode::SUCCESS;
}

/// Update the transient object from the other representation.
StatusCode Converter::updateObj(IOpaqueAddress*, DataObject*)   {
  return StatusCode::SUCCESS;
}

/// Update the references of an updated transient object.
StatusCode Converter::updateObjRefs(IOpaqueAddress*, DataObject*)  {
  return StatusCode::SUCCESS;
}

/// Convert the transient object to the requested representation.
StatusCode Converter::createRep(DataObject*, IOpaqueAddress*&)  {
  return StatusCode::SUCCESS;
}

/// Resolve the references of the converted object. 
StatusCode Converter::fillRepRefs(IOpaqueAddress*, DataObject*)  {
  return StatusCode::SUCCESS;
}

/// Update the converted representation of a transient object.
StatusCode Converter::updateRep(IOpaqueAddress*, DataObject*)  {
  return StatusCode::SUCCESS;
}

/// Update the references of an already converted object.
StatusCode Converter::updateRepRefs(IOpaqueAddress*, DataObject*)    {
  return StatusCode::SUCCESS;
}

/// Initialize the converter
StatusCode Converter::initialize()    {
  StatusCode status = StatusCode::SUCCESS;
  // Get a reference to the Message Service
  status = serviceLocator()->service("MessageSvc",m_messageSvc, true );
  if ( status.isFailure() )   {
    return status;
  }
  return status;
}

/// Finalize the converter
StatusCode Converter::finalize()    {
  if ( 0 != m_messageSvc )    {
    m_messageSvc->release();
    m_messageSvc = 0;
  }
  if ( 0 != m_dataManager )  {
    m_dataManager->release();
    m_dataManager = 0;
  }
  if ( 0 != m_dataProvider )  {
    m_dataProvider->release();
    m_dataProvider = 0;
  }
  return StatusCode::SUCCESS;
}

/// Set data provider service
StatusCode Converter::setDataProvider(IDataProviderSvc* svc)  {
  IDataManagerSvc* oldDataManager = m_dataManager;
  IDataProviderSvc* oldDataProvider = m_dataProvider;
  m_dataProvider = svc;
  m_dataManager  = 0;
  if ( 0 != m_dataProvider )  {
    m_dataProvider->addRef();
    svc->queryInterface(IDataManagerSvc::interfaceID(), pp_cast<void>(&m_dataManager)).ignore();
  }
  if ( oldDataManager )  {
    oldDataManager->release();
  }
  if ( oldDataProvider )  {
    oldDataProvider->release();
  }
  return StatusCode::SUCCESS;
}

/// Get data provider service
IDataProviderSvc* Converter::dataProvider()  const    {
  return m_dataProvider;
}

/// Get data manager service
IDataManagerSvc* Converter::dataManager()  const    {
  return m_dataManager;
}

/// Set conversion service the converter is connected to
StatusCode Converter::setConversionSvc(IConversionSvc* svc)   {
  m_conversionSvc = svc;
  return StatusCode::SUCCESS;
}

/// Get data conversion service the converter is connected to
IConversionSvc* Converter::conversionSvc()  const    {
  return m_conversionSvc;
}

/// Set address creator facility
StatusCode Converter::setAddressCreator(IAddressCreator* creator)   {
  m_addressCreator = creator;
  return StatusCode::SUCCESS;
}

/// Access the transient store
IAddressCreator* Converter::addressCreator()  const   {
  return m_addressCreator;
}

///--- Retrieve pointer to service locator
ISvcLocator* Converter::serviceLocator()  const     {
  return m_svcLocator;
}

///--- Retrieve pointer to message service
IMessageSvc* Converter::msgSvc()  const   {
  if ( 0 == m_messageSvc ) {
    StatusCode sc = serviceLocator()->service( "MessageSvc", m_messageSvc, true );
    if( sc.isFailure() ) {
      throw GaudiException("Service [MessageSvc] not found", "Converter", sc);
    }
  }
  return m_messageSvc;
}

// Obsoleted name, kept due to the backwards compatibility
IMessageSvc* Converter::messageService()  const   {
  return m_messageSvc;
}

/// Standard Constructor
Converter::Converter(long storage_type, const CLID& class_type, ISvcLocator* svc=0) :
  m_refCount(0),   
  m_storageType(storage_type), 
  m_classType(class_type), 
  m_dataProvider(0),
  m_dataManager(0),
  m_conversionSvc(0),
  m_svcLocator(svc),
  m_messageSvc(0)
{
  if ( 0 != m_svcLocator ) m_svcLocator->addRef();
}

/// Standard Destructor
Converter::~Converter()   {
  if ( 0 != m_svcLocator ) m_svcLocator->release();
}

StatusCode 
Converter::service_i(const std::string& svcName, bool createIf, 
		     const InterfaceID& iid, void** ppSvc) const {
  StatusCode sc(StatusCode::FAILURE);
  // Check for name of conversion service
  INamedInterface* cnvsvc =
    dynamic_cast<INamedInterface*> (Converter::conversionSvc());
  if( cnvsvc != 0 ) {
    MsgStream log(msgSvc(),"Converter");
    ServiceLocatorHelper helper(*serviceLocator(), log, cnvsvc->name());
    sc = helper.getService(svcName, createIf, iid, ppSvc);
  }
  return sc;
}

StatusCode 
Converter::service_i(const std::string& svcType, const std::string& svcName,
		     const InterfaceID& iid, void** ppSvc) const {
  StatusCode sc(StatusCode::FAILURE);
  // Check for name of conversion service
  ConversionSvc* cnvsvc =
    dynamic_cast<ConversionSvc*> (Converter::conversionSvc());
  if( cnvsvc != 0 ) {
    MsgStream log(msgSvc(),"Converter");
    ServiceLocatorHelper helper(*serviceLocator(), log, cnvsvc->name());
    sc = helper.createService(svcType, svcName, iid, ppSvc);
  }
  return sc;
}
