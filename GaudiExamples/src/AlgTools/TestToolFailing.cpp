/*
 * TestToolFailing.cpp
 *
 *  @author: Marco Clemencic
 *  @date: 2008-10-22
 */

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "ITestTool.h"


class TestToolFailing: public GaudiTool,
                       virtual public ITestTool {

public:
  TestToolFailing(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  /// Initialize method
  StatusCode initialize();

  virtual ~TestToolFailing( ) {} ///< Destructor

};

#include "GaudiKernel/ToolFactory.h"
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(TestToolFailing)

TestToolFailing::TestToolFailing(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent):
  GaudiTool(type, name, parent) {
  declareInterface<ITestTool>(this);
}

StatusCode TestToolFailing::initialize(){
  debug() << "Initialize" << endreq;
  if ( !GaudiTool::initialize() ) return StatusCode::FAILURE;

  return Error("Initialization failure");
}
