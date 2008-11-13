// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/IDataSourceMgr.h,v 1.4 2004/07/06 10:17:39 mato Exp $
#ifndef GAUDIKERNEL_IDATASOURCEMGR_H
#define GAUDIKERNEL_IDATASOURCEMGR_H

// Framework include files
#include "GaudiKernel/IInterface.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IDataSourceMgr(121, 1 , 0); 

/** @class IDataSourceMgr IDataSourceMgr.h GaudiKernel/IDataSourceMgr.h

    IDataSourceMgr interface definition.
 
    @author Markus Frank
    @version 1.0
*/
class IDataSourceMgr : virtual public IInterface	    {
  public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDataSourceMgr; }
  /** Check if a datasource is connected
      @param    identifier  Connection string
      @return               Status code indicating success or failure.
  */
  virtual bool isConnected(const std::string& identifier)   const  = 0;

  /** Connect data source
      @param    mode        Connection mode (READ,CREATE,UPDATE)
      @param    logon       Connection string
      @param    identifier  Logical name (OUTPUT)
      @return               Status code indicating success or failure.
  */
  virtual StatusCode connect(const std::string& logon, std::string& identifier) = 0;

  /** Connect data source
      @param    mode        Connection mode (READ,CREATE,UPDATE)
      @param    logon       Connection string
      @return               Status code indicating success or failure.
  */
  virtual StatusCode connect(const std::string& logon) = 0;

  /** Disconnect data source
      @param    identifier  Connection string
      @return               Status code indicating success or failure.
  */
  virtual StatusCode disconnect(const std::string& identifier) = 0;

  /// Close all open connections
  virtual StatusCode disconnectAll() = 0;
};

#endif // GAUDIKERNEL_IDATASOURCEMGR_H
