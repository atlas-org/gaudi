// $Id: OutputFSRStream.h,v 1.1 2008/11/04 22:49:25 marcocle Exp $
#ifndef GAUDISVC_PERSISTENCYSVC_OUTPUTFSRSTREAM_H
#define GAUDISVC_PERSISTENCYSVC_OUTPUTFSRSTREAM_H

// Required for inheritance
#include "OutputStream.h"

// Needed for data members
#include <string>
#include <vector>

/** @class OutputFSRStream OutputFSRStream.h
  * Extension of OutputStream to write File Summary Records (FSR) after last event
  *
  * @author  Marco Cattaneo
  */
class OutputFSRStream : public OutputStream     {
  friend class AlgFactory<OutputFSRStream>;
  friend class Factory<OutputFSRStream,IAlgorithm* (std::string,ISvcLocator *)>;

public:
	/// Standard algorithm Constructor
	OutputFSRStream(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~OutputFSRStream();
  /// Over-ride stop() of base class
  virtual StatusCode stop();
  /// Over-ride finalize() of base class
  virtual StatusCode finalize();

private:
  /// Mandatory items to be written as File Summary Records
  std::vector<std::string> m_FSRItemNames;
  /// Optional items to be written as File Summary Records
  std::vector<std::string> m_FSROptItemNames;
};

#endif // GAUDISVC_PERSISTENCYSVC_OUPUTFSRSTREAM_H
