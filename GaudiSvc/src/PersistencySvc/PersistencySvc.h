//	============================================================
//
//	PersistencySvc.h
//	------------------------------------------------------------
//
//	Package   : PersistencySvc
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef PERSISTENCYSVC_PERSISTENCYSVC_H
#define PERSISTENCYSVC_PERSISTENCYSVC_H 1

// Framework include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IPersistencySvc.h"
#include "GaudiKernel/IAddressCreator.h"

// STL include files
#include <map>
#include <algorithm>
#include <functional>

// Forward declarations
template <class TYPE> class SvcFactory;
namespace { template <class P, class S> class Factory; }

/** PersistencySvc class implementation definition.

    <P> System:  The LHCb Offline System
    <P> Package: PersistencySvc

     Dependencies:
    <UL>
    <LI> STL:                         <vector>, <algorithm>, <functional>
    <LI> Generic service definiton:  "Kernel/Service.h"
    <LI> IConversionSvc definition:  "Kernel/IConversionSvc.h"
    <LI> PersistencySvc definition:  "Kernel/Interfaces/IPersistencySvc.h"
    </UL>

    History:
    <PRE>
    +---------+----------------------------------------------+---------+
    |    Date |                 Comment                      | Who     |
    +---------+----------------------------------------------+---------+
    | 3/11/98 | Initial version                              | M.Frank |
    +---------+----------------------------------------------+---------+
    </PRE>
   @author Markus Frank
   @version 1.0
*/
class PersistencySvc  : public Service, 
                        virtual public IConversionSvc,
                        virtual public IPersistencySvc,
                        virtual public IAddressCreator  {
protected:
  friend class SvcFactory<PersistencySvc>;
  friend class Factory<PersistencySvc,IService* (std::string,ISvcLocator *)>;

  class ServiceEntry {
    long             m_serviceType;
    IService*        m_service;
    IConversionSvc*  m_cnvService;
    IAddressCreator* m_addrCreator;
  public:
    ServiceEntry( long type, 
                  IService* svc, 
                  IConversionSvc* cnv, 
                  IAddressCreator* cr)  {
      m_serviceType  = type;
      m_addrCreator  = cr;
      m_cnvService   = cnv;
      m_service      = svc;
    }
    ServiceEntry(const ServiceEntry& copy)  {
      m_serviceType  = copy.m_serviceType;
      m_addrCreator  = copy.m_addrCreator;
      m_cnvService   = copy.m_cnvService;
      m_service      = copy.m_service;
    }  
    virtual ~ServiceEntry()  {
    }
    IService*           service()   const   {
      return m_service;
    }
    IConversionSvc*     conversionSvc()  const    {
      return m_cnvService;
    }
    IAddressCreator*    addrCreator()  const    {
      return m_addrCreator;
    }
    long svcType()  const {
      return m_serviceType;
    }
  };
  typedef std::map<long,ServiceEntry> Services;
/*
  class SvcTest : public std::unary_function<ServiceEntry, bool>   {
  public:
    SvcTest(long test) : m_test(test)    {
    }
    virtual ~SvcTest()    {
    }
    bool operator()( const ServiceEntry& testee )  {
        return m_test == testee.svcType() ? true : false;
    }
  protected:
       long m_test;
  };
*/
public:

  /**@name IPersistencySvc Interface implementation */
  //@{
  /// Return default service type
  long repSvcType() const;

  /// Retrieve the class type of objects the converter produces. (DUMMY)
  const CLID& objType() const;

  /// Implementation of IConverter: Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress,DataObject*& refpObject);

  /// Implementation of IConverter: Resolve the references of the created transient object.
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Implementation of IConverter: Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);

  /// Implementation of IConverter: Update the references of an updated transient object.
  virtual StatusCode updateObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Implementation of IConverter: Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);

  /// Implementation of IConverter: Resolve the references of the converted object. 
  virtual StatusCode fillRepRefs(IOpaqueAddress* pAddress,DataObject* pObject);

  /// Implementation of IConverter: Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Implementation of IConverter: Update the references of an already converted object.
  virtual StatusCode updateRepRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /// Define transient datastore.
  virtual StatusCode setDataProvider(IDataProviderSvc* pStore);

  /// Access reference to transient datastore
  virtual IDataProviderSvc* dataProvider()    const;

  /// Set conversion service the converter is connected to
  virtual StatusCode setConversionSvc(IConversionSvc* svc);

  /// Get conversion service the converter is connected to
  virtual IConversionSvc* conversionSvc()    const;

  /// Set address creator facility
  virtual StatusCode setAddressCreator(IAddressCreator* creator);

  /// Retrieve address creator facility
  virtual IAddressCreator* addressCreator()   const;

  /// Add converter object to conversion service.
  virtual StatusCode addConverter(IConverter* pConverter);

  /// Add converter object to conversion service.
  virtual StatusCode addConverter(const CLID& clid);

  /// Retrieve converter from list
  virtual IConverter* converter(const CLID& clid);

  /// Remove converter object from conversion service (if present).
  virtual StatusCode removeConverter(const CLID& clid);

  /// Connect the output file to the service with open mode.
  virtual StatusCode connectOutput(const std::string& outputFile,
                                   const std::string& openMode);
  /// Connect the output file to the service.
  virtual StatusCode connectOutput(const std::string& outputFile);

  /// Commit pending output.
  virtual StatusCode commitOutput(const std::string& output, bool do_commit);

  /// Create a Generic address using explicit arguments to identify a single object.
  virtual StatusCode createAddress(long svc_type,
                                   const CLID& clid,
                                   const std::string* pars, 
                                   const unsigned long* ipars,
                                   IOpaqueAddress*& refpAddress);

  /// Convert an address to string form
  virtual StatusCode convertAddress( const IOpaqueAddress* pAddress,
                                     std::string& refAddress);

  /// Convert an address in string form to object form
  virtual StatusCode createAddress( long svc_type, 
                                    const CLID& clid, 
                                    const std::string& refAddress,
                                    IOpaqueAddress*& refpAddress);
  //@}

  /**@name: IPersistencySvc implementation  */
  //@{
  /// Add a new Service
  virtual StatusCode addCnvService(IConversionSvc* service);

  /// Remove a Service
  virtual StatusCode removeCnvService(long type);

  /// Set default service type
  virtual StatusCode setDefaultCnvService(long type);

  /// Retrieve conversion service identified by technology
  virtual StatusCode getService(long service_type, IConversionSvc*& refpSvc);

  /// Retrieve conversion service identified by technology
  virtual StatusCode getService(const std::string& service_type, IConversionSvc*& refpSvc);
  //@}

  /**@name: IService implementation */
  //@{
  /// Initialize the service.
  virtual StatusCode initialize();

  /// stop the service.
  virtual StatusCode finalize();
  //@}

  /**@name: IInterface implementation  */
  //@{
  /// Query interface
  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  //@}
  /**@name: Object implementation  */
  //@{
  /// Standard Constructor
  PersistencySvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~PersistencySvc();
protected:

  /// Retrieve conversion service by name
  IConversionSvc* service(const std::string& nam);

  /// Retrieve conversion service from list
  IConversionSvc* service(long service_type);

  /// Retrieve address creator from list
  IAddressCreator* addressCreator(long service_type);

  /// Retrieve string from storage type and clid
  void encodeAddrHdr(long service_type, 
		     const CLID& clid, 	
		     std::string& address) const;

  /// Retrieve storage type and clid from address header of string
  void decodeAddrHdr(const std::string& address, 
		     long& service_type, 
		     CLID& clid, 
		     std::string& address_trailer) const;
    

  /// Implementation helper
  StatusCode makeCall(int typ, IOpaqueAddress*& pAddress, DataObject*& pObject);

  /// Set enabled flag
  bool enable(bool value);
  //@}

  /// Handlers for Service Names Property
  void svcNamesHandler( Property& theProp );

  /// Default service type
  long                m_cnvDefType;
  /// Pointer to datma provider service
  IDataProviderSvc*   m_dataSvc;
  /// List of convermsion workers
  Services            m_cnvServices;
  /// Default output service
  IConversionSvc*     m_cnvDefault;
  /** Names of services to be requested from 
      the service locator and added by default.
  */
  StringArrayProperty m_svcNames;
  /// Flag to indicate that the service is enabled
  bool                m_enable;
};
#endif // PERSISTENCYSVC_PERSISTENCYSVC_H
