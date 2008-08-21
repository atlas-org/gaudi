//	============================================================
//
//	HistogramPersistencySvc.h
//	------------------------------------------------------------
//
//	Package   : PersistencySvc
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef PERSISTENCYSVC_HISTOGRAMPERSISTENCYSVC_H
#define PERSISTENCYSVC_HISTOGRAMPERSISTENCYSVC_H 1

#include "PersistencySvc.h"

/** HistogramPersistencySvc class implementation definition.

    <P> System:  The LHCb Offline System
    <P> Package: HistogramPersistencySvc

     Dependencies:
    <UL>
    <LI> PersistencySvc definition:  "Kernel/Interfaces/PersistencySvc.h"
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
class HistogramPersistencySvc  : virtual public PersistencySvc	{
public:
  friend class SvcFactory<HistogramPersistencySvc>;
  friend class Factory<HistogramPersistencySvc,IService* (std::string,ISvcLocator *)>;

  /**@name PersistencySvc overrides    */
  //@{
  /// Initialize the service.
  virtual StatusCode initialize();
  /// Reinitialize the service.
  virtual StatusCode reinitialize();
  /// Finalize the service.
  virtual StatusCode finalize();
  /// Implementation of IConverter: Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  //@}

  /**@name: Object implementation  */
  //@{
  /// Standard Constructor
  HistogramPersistencySvc(const std::string& name, ISvcLocator* svc);

  /// Standard Destructor
  virtual ~HistogramPersistencySvc();
  //@}
protected:
  /// Name of the Hist Pers type
  std::string       m_histPersName;
  /// Name of the outputFile
  std::string       m_outputFile;
};
#endif // PERSISTENCYSVC_HISTOGRAMPERSISTENCYSVC_H
