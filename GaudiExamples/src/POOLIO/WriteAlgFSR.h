//$Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiExamples/src/POOLIO/WriteAlg.h,v 1.1 2004/05/24 14:05:04 mato Exp $	//
#ifndef GAUDIEXAMPLE_WRITEALGFSR_H
#define GAUDIEXAMPLE_WRITEALGFSR_H

// Framework include files
#include "GaudiKernel/Algorithm.h"

/** @class WriteAlg WriteAlg.h

    WriteAlg class for the RootIOExample

    @author Markus Frank
*/

class WriteAlgFSR : public Algorithm {
public:
  /// Constructor: A constructor of this form must be provided.
  WriteAlgFSR(const std::string& name, ISvcLocator* pSvcLocator)
  :	Algorithm(name, pSvcLocator) { }
  /// Standard Destructor
  virtual ~WriteAlgFSR() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
};

#endif // GAUDIEXAMPLE_WRITEALGFSR_H
