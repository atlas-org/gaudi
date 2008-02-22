// $Id: PoolDbIOHandler.h,v 1.3 2006/01/27 08:27:02 hmd Exp $
//====================================================================
//	PoolDbIOHandler.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiPoolDb/GaudiPoolDb/PoolDbIOHandler.h,v 1.3 2006/01/27 08:27:02 hmd Exp $
#ifndef POOLDB_POOLDBIOHANDLER_H
#define POOLDB_POOLDBIOHANDLER_H
#include "StorageSvc/IIOHandler.h"
#include "Reflex/Reflex.h"

template <class T>
class PoolDbIOHandler : public pool::IIOHandler {
protected:
  // Refeence to type structure,
  ROOT::Reflex::Type m_class;
public:
  PoolDbIOHandler(const ROOT::Reflex::Type& cl) : m_class(cl) {
    //std::cout << "Created IOHandler of type:" << cl->fullName() << std::endl;
  }

  virtual ~PoolDbIOHandler() {
  }

  /// Release call: destroy all internally allocated resources.
  virtual void release() {
    delete this;
  }

  /// Callback for reading the object
  /** @param obj  [IN]   Pointer to user object.
    */
  virtual void onReadUpdate(void* obj);

  /// Callback for writing the object
  /** @param obj  [IN]   Pointer to user object.
    */
  virtual void onWriteUpdate(void* obj);
};

#endif // POOLDB_PoolDbAddress_H
