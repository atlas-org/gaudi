#ifndef GAUDIKERNEL_IMONITORSVC_H
#define GAUDIKERNEL_IMONITORSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <set>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IMonitorSvc("IMonitorSvc", 3, 0);

// forward declaration
namespace AIDA { class IBaseHistogram; }

/** @class IMonitorSvc IMonitorSvc.h GaudiKernel/IMonitorSvc.h

    Definition of the IMonitorSvc interface, which publishes Gaudi variables
    to outside monitoring processes.

    @author Philippe Vannerem
    @author Pere Mato
*/

class IMonitorSvc : virtual public IInterface {
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMonitorSvc; }

  /** Declare monitoring information
      @param name Monitoring information name knwon to the external system
      @param var Monitoring Listener address
      @param desc Textual description
      @param owner Owner identifier of the monitoring information (needed to peform 
                   clean up 
  */
  virtual void declareInfo(const std::string& name, const bool&  var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const int&  var, const  std::string& desc,  const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const long&  var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const double& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::string& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::pair<double,double>& var, const std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const AIDA::IBaseHistogram* var, const  std::string& desc, const IInterface* owner) = 0;
  virtual void declareInfo(const std::string& name, const std::string& format, const void * var, int size, const std::string& desc, const IInterface* owner) = 0;
 
  /** Undeclare monitoring information
      @param name Monitoring information name knwon to the external system
      @param owner Owner identifier of the monitoring information
  */
  virtual void undeclareInfo( const std::string& name, const IInterface* owner ) = 0;
  /** Undeclare monitoring information
      @param owner Owner identifier of the monitoring information
  */
  virtual void undeclareAll( const IInterface* owner ) = 0;

  /** Get the names for all declared monitoring informations for a given
      owener. If the owner is NULL, then it returns for all owners
      informationUndeclare monitoring information
  */
  virtual std::set<std::string>* getInfos(const IInterface* owner = 0) = 0;

};

#endif // GAUDIKERNEL_IMONITORSVC_H
