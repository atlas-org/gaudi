// $Id: AlgorithmManager.h,v 1.3 2006/11/30 20:51:35 mato Exp $	//
#ifndef GAUDISVC_ALGORITHMMANAGER_H
#define GAUDISVC_ALGORITHMMANAGER_H

// Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IAlgManager.h"
#include <string>
#include <list>
#include <map>

// Forward declarations
class IAlgorithm;
class ISvcLocator;
class IMessageSvc;

/** @class AlgorithmManager AlgorithmManager.h

    The AlgorithmManager class is in charge of the creation of concrete
    instances of Algorithms. 
    The ApplicationMgr delegates the creation and bookkeeping of 
    algorithms to the algorithm factory. In order to be able to create 
    algorithms from which it does not know the concrete type it requires
    that the algorithm has been declared in one of 3 possible ways: an abstract
    static creator function, a dynamic link library or an abstract factory reference.
    
    @author  Pere Mato
*/
class AlgorithmManager : virtual public  IAlgManager  {
  public:
  /// typedefs and classes
  typedef std::list<IAlgorithm*> ListAlg;
  
  /// default creator
  AlgorithmManager( IInterface* iface );
  /// virtual destructor
  virtual ~AlgorithmManager();

  /// implmentation of IInterface::addRef
  virtual unsigned long addRef();
  /// implmentation of IInterface::release
  virtual unsigned long release();
  /// implementation of IInterface::queryInterface
  virtual StatusCode queryInterface(const InterfaceID& iid, void** pinterface);

  /// implementation of IAlgManager::addAlgorithm
  virtual StatusCode addAlgorithm( IAlgorithm* alg);
  /// implementation of IAlgManager::removeAlgorithm
  virtual StatusCode removeAlgorithm( IAlgorithm* alg);
  /// implementation of IAlgManager::createAlgorithm
  virtual StatusCode createAlgorithm( const std::string& algtype, const std::string& algname, 
                                      IAlgorithm*& algorithm, bool managed = false );
  /// implementation of IAlgManager::getAlgorithm
  virtual StatusCode getAlgorithm( const std::string& name, IAlgorithm*& alg) const;
  /// implementation of IAlgManager::existsAlgorithm
  virtual bool existsAlgorithm( const std::string& name) const;
  /// implementation of IAlgManager::getAlgorithms
  virtual std::list<IAlgorithm*>& getAlgorithms( ) const;
  /// implementation of IAlgManager::initializeAlgorithms
  virtual StatusCode initializeAlgorithms();
  /// implementation of IAlgManager::finalizeAlgorithms
  virtual StatusCode finalizeAlgorithms();

private:
  IInterface*   m_pOuter;      ///< Interface hub reference (ApplicationMgr)
  unsigned long m_refcount;    ///< Reference counter
  ListAlg*      m_listalg;     ///< List of algorithms maintained by AlgorithmManager
  ListAlg*      m_listmgralg;  ///< List of managed algorithms maintained by AlgorithmManager
  ISvcLocator*  m_svclocator;  ///< Service locator reference
  IMessageSvc*  m_msgsvc;      ///< Pointer to the message service if it exists
};
#endif  // GAUDISVC_ALGORITHMFACTORY_H

