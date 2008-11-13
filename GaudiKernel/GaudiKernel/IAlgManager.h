// $Id: IAlgManager.h,v 1.7 2008/06/02 14:20:38 marcocle Exp $
#ifndef GAUDIKERNEL_IALGMANAGER_H
#define GAUDIKERNEL_IALGMANAGER_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <string>
#include <list>

// Forward class declaration
class IService;
class IAlgorithm;
class ISvcLocator;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IAlgManager(14, 4 , 1);

/** @class IAlgManager IAlgManager.h GaudiKernel/IAlgManager.h

    The IAlgManager is the interface implemented by the Algorithm Factory in the
    Application Manager to support management functions. Clients use this
    interface to declare abstract algorithm factories, and to create concrete
    instances of algorithms. There are currently 3 methods for the declaration:
    static creator method (creator). an abstract factory (factory) or a shareable
    library (module).               

    @author Pere Mato
*/
class IAlgManager : virtual public  IInterface  {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAlgManager; }

  /// Add an algorithm to the list of known algorithms
  virtual StatusCode addAlgorithm( IAlgorithm* alg     // Pointer to the Algorithm
                                 ) = 0;
  /// Remove an algorithm from the list of known algorithms
  virtual StatusCode removeAlgorithm( IAlgorithm* alg  // Pointer to the Algorithm
                                 ) = 0;
  /// Create an instance of a algorithm type that has been declared beforehand and assigns to it a name. 
  /// It returns a pointer to an IAlgorithm. 
  virtual StatusCode createAlgorithm( const std::string& algtype,  // Algorithm type name
                                      const std::string& algname,  // Algorithm name to be assigned
                                      IAlgorithm*& alg,            // Returned algorithm
                                      bool managed = false         // Flag to indicate if the algorithm is managed
                                    ) = 0;
  /// Find an algorithm with given name in the list of known algorithms
  virtual StatusCode getAlgorithm( const std::string& name,        // Algorithm name to be searched
                                   IAlgorithm*& alg                // Returned algorithm
                                 ) const = 0;
  /// Check the existance of an algorithm with a given name in the list of known algorithms
  virtual bool existsAlgorithm( const std::string& name            // Algorithm name to be searched
                              ) const = 0;
  /// Return the list of Algorithms
  virtual std::list<IAlgorithm*>& getAlgorithms( ) const = 0;
  
  /// Initializes the list of "managed" algorithms
  virtual StatusCode initializeAlgorithms() = 0;

  /// Starts the list of "managed" algorithms
  virtual StatusCode startAlgorithms() = 0;

  /// Stops the list of "managed" algorithms
  virtual StatusCode stopAlgorithms() = 0;

  /// Finalizes the list of "managed" algorithms
  virtual StatusCode finalizeAlgorithms() = 0;
  
  /// Initializes the list of "managed" algorithms
  virtual StatusCode reinitializeAlgorithms() = 0;

  /// Starts the list of "managed" algorithms
  virtual StatusCode restartAlgorithms() = 0;

};


#endif  // GAUDI_IALGMANAGER_H
