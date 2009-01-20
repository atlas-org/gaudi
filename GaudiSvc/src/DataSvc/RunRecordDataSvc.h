// $Id: RunRecordDataSvc.h,v 1.4 2006/11/30 14:57:03 mato Exp $
#ifndef DATASVC_EVTDATASVC_H
#define DATASVC_EVTDATASVC_H

#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IIncidentListener.h"
class IConversionSvc;


/** @class RunRecordDataSvc RunRecordDataSvc.h
 * 
 *   A RunRecordDataSvc is the base class for event services
 *
 *   When a new datafile is opened the data service retrieves an incident
 *   from the persistency area (currently only from POOL) together with an opaque
 *   address describing the run record. These history records are put onto the
 *   run-records datastore under the name of the logical file (FID):
 *   /RunRecords/<FID>/.....
 *
 *   The data service by default has two top level leafs:
 *   /RunRecords/SOR   (start-of-run-record)  and
 *   /RunRecords/EOR   (end-of-run-record)
 *
 *   Hence, each history records can be addresses the same way:
 *   /RunRecord/<FID>/SOR  (/EOR)
 *
 *   @author  M.Frank
 *   @version 1.0
 */
class RunRecordDataSvc  : public DataSvc, virtual public IIncidentListener   {
  friend class SvcFactory<RunRecordDataSvc>;
 protected:
  typedef std::vector<std::string> StringV;
 public:
  virtual StatusCode initialize();
  virtual StatusCode reinitialize();
  virtual StatusCode finalize();

  /** IInterface override: Query interfaces of Interface
      @param riid         ID of Interface to be retrieved
      @param ppvInterface Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvInterface);

  /// IIncidentListener override: Inform that a new incident has occured
  virtual void handle(const Incident& incident);

  /// Standard Constructor
  RunRecordDataSvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~RunRecordDataSvc();
  
 protected:
  /// Load new run record into the data store if necessary
  void registerRunRecord(const std::string& data, IOpaqueAddress* pAddr);

 private:
  /// Reference to the main data conversion service
  IConversionSvc* m_cnvSvc;
};
#endif // DATASVC_EVTDATASVC_H
