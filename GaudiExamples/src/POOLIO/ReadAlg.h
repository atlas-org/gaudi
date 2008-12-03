//$Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiExamples/src/POOLIO/ReadAlg.h,v 1.2 2008/11/04 22:49:24 marcocle Exp $	//
#ifndef GAUDIEXAMPLES_READALG_H
#define GAUDIEXAMPLES_READALG_H

// Framework include files
#include "GaudiAlg/GaudiAlgorithm.h"  // Required for inheritance

/** @class ReadAlg ReadAlg.h

    ReadAlg class for the RootIOExample

    @author Markus Frank
*/

class ReadAlg : public GaudiAlgorithm {
public:
	/// Constructor: A constructor of this form must be provided.
	ReadAlg(const std::string& name, ISvcLocator* pSvcLocator)  
    : GaudiAlgorithm(name, pSvcLocator) { }
  /// Standard Destructor
  virtual ~ReadAlg() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize() {
    return StatusCode::SUCCESS;
  }
  /// Event callback
  virtual StatusCode execute();
};

#endif // GAUDIEXAMPLES_READALG_H
