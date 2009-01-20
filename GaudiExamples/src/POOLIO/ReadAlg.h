//$Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiExamples/src/POOLIO/ReadAlg.h,v 1.2 2008/11/04 22:49:24 marcocle Exp $       //
#ifndef GAUDIEXAMPLES_READALG_H
#define GAUDIEXAMPLES_READALG_H

// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance

/** @class ReadAlg ReadAlg.h

    ReadAlg class for the RootIOExample

    @author Markus Frank
*/

class ReadAlg : public Algorithm {
  /// Reference to run records data service
  IDataProviderSvc* m_runRecordSvc;

public:
  /// Constructor: A constructor of this form must be provided.
  ReadAlg(const std::string& nam, ISvcLocator* pSvc)
    : Algorithm(nam, pSvc), m_runRecordSvc(0) { }
  /// Standard Destructor
  virtual ~ReadAlg() { }
  /// Initialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
};

#endif // GAUDIEXAMPLES_READALG_H
