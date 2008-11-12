// $Id: MyGaudiTool.cpp,v 1.4 2008/10/10 15:18:56 marcocle Exp $
// Framework include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// Accessing data:
#include "GaudiKernel/PhysicalConstants.h"

// Tool example
#include "MyGaudiTool.h"

// Declaration of the AlgTool Factory
DECLARE_TOOL_FACTORY(MyGaudiTool);

//------------------------------------------------------------------------------
MyGaudiTool::MyGaudiTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  //------------------------------------------------------------------------------
  : GaudiTool( type, name, parent ) {

  // declare my interface
  declareInterface<IMyTool>(this);
  // declare my second interface
  declareInterface<IMyOtherTool>(this);

  // declare properties
  declareProperty( "Int",    m_int    = 100);
  declareProperty( "Double", m_double = 100.);
  declareProperty( "String", m_string = "hundred");
  declareProperty( "Bool",   m_bool   = true);
}

//------------------------------------------------------------------------------
const std::string&  MyGaudiTool::message() const
  //------------------------------------------------------------------------------
{
  static std::string msg("It works!!!");
  return msg;
}

//------------------------------------------------------------------------------
void  MyGaudiTool::doIt()
  //------------------------------------------------------------------------------
{
  info()  << "doIt() has been called" << endreq;
  debug() << "doIt() [DEBUG] has been called" << endreq;
}

//------------------------------------------------------------------------------
void MyGaudiTool::doItAgain()
//------------------------------------------------------------------------------
{
  info()  << "doItAgain() has been called" << endreq; 
}

//------------------------------------------------------------------------------
StatusCode  MyGaudiTool::initialize()
  //------------------------------------------------------------------------------
{
  info() << "intialize() has been called" << endreq;

  // Make use of tool<>

  info() << "Int    = " << m_int    << endreq;
  info() << "Double = " << m_double << endreq;
  info() << "String = " << m_string << endreq;
  info() << "Bool   = " << m_bool   << endreq;

  return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------
StatusCode  MyGaudiTool::finalize()
  //------------------------------------------------------------------------------
{
  info() << "finalize() has been called" << endreq;
  return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
MyGaudiTool::~MyGaudiTool( )
  //------------------------------------------------------------------------------
{
  info() << "destructor has been called" << endreq;
}


