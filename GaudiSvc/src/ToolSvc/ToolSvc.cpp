// $Id: ToolSvc.cpp,v 1.35.2.1 2008/06/19 21:28:02 leggett Exp $

// Include Files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IHistorySvc.h"
#include "GaudiKernel/ToolFactory.h"
#include "ToolSvc.h"
#include <algorithm>
#include <map>
#include <string>
#include "boost/lambda/bind.hpp"



// Instantiation of a static factory class used by clients to create
//  instances of this service
DECLARE_SERVICE_FACTORY(ToolSvc)

using ROOT::Reflex::PluginService;
namespace bl = boost::lambda;

//------------------------------------------------------------------------------
ToolSvc::ToolSvc( const std::string& name, ISvcLocator* svc )
  //------------------------------------------------------------------------------
  : Service(name, svc),
    m_pHistorySvc(0),
    m_isInFinalize( false )
 { }

//------------------------------------------------------------------------------
ToolSvc::~ToolSvc()
  //------------------------------------------------------------------------------
{ 

}

//------------------------------------------------------------------------------
StatusCode ToolSvc::initialize()
  //------------------------------------------------------------------------------
{

  // initialize the Service Base class
  StatusCode status = Service::initialize();
  if ( status.isFailure() )
  {
    MsgStream log( msgSvc(), name() );
    log << MSG::ERROR << "Unable to initialize the Service" << endreq;
    return status;
  }

  // set my own (ToolSvc) properties via the jobOptionService
  if (setProperties().isFailure()) {
    MsgStream log( msgSvc(), name() );
    log << MSG::ERROR << "Unable to set base properties" << endreq;
    return StatusCode::FAILURE;
  }

  return status;
}

//------------------------------------------------------------------------------
StatusCode ToolSvc::finalize()
  //------------------------------------------------------------------------------
{
  // Finalize and delete all left-over tools. Normally all tools created with 
  // ToolSvc are left over, since ToolSvc holds a refCount (via AlgTool ctor).
  // Several cases need to be covered:
  // 1) Simple dependencies: no circular dependencies between tools,
  //    and tools not using other tools
  // 2) Tools-using-tools (but no circular dependencies)
  //   a) release() is called in the tool::finalize() (e.g. via GaudiAlgorithm)
  //   b) release() is called in the tool destructor  (e.g. via ToolHandle)
  // 3) Circular dependencies between tools
  //   a) release() is called in the tool::finalize() (e.g. via GaudiAlgorithm)
  //   b) release() is called in the tool destructor  (e.g. via ToolHandle)
  // 4) In addition to each of the above cases, refCounting of a particular tool 
  //    might not have been done correctly in the code. Typically release() 
  //    is not called, so we are left with a too high refCount.
  //    What to do with those, and how to avoid a crash while handling them...
  
  /** Algorithm: 2 passes.
      First pass:
        - do not explicitly release any tools
        - finalize tools, in the order of increasing number of refCounts
      Second pass:
        - explicitly release all tools, one release() on all tools per loop.
        -> tools are deleted in the order of increasing number of refCounts.
  */
  MsgStream log( msgSvc(), name());
  // to ensure that any tool that gets deleted by another tool in the process,
  // also calls sysFinalize() instead of finalize() (tested for in releaseTool())
  m_isInFinalize = true;
  ListTools finalizedTools; // list of tools that have been finalized
  log << MSG::INFO  << "finalize: Removing all tools created by ToolSvc" << endreq;

  // Print out list of tools
  log << MSG::DEBUG << "finalize:  Tool List : ";
  for ( ListTools::const_iterator iTool = m_instancesTools.begin();
        iTool != m_instancesTools.end(); ++iTool ) {
    log << (*iTool)->name() << ":" << refCountTool( *iTool ) << " ";
  }
  log << endreq;

  //
  // first pass: Finalize all tools (but don't delete them)
  //
  /** Inner loop: full loop over all left-over tools
        + finalize tools with the minimum number of refCounts in the list.
        + Remove finalized tools from list of tools, and add them to the
          list of finalized tools, to be deleted at the end. This way,
          any non-finalized tools who still reference already finalized 
          tools in their finalize() will still find a live tool.
      Outer loop: keep on going until nothing changes in the list of tools.
        Checking for: 
        + number of left-over tools
        + total number of refcounts
        + minimum number of refcounts
  */
  bool fail(false);
  size_t toolCount = m_instancesTools.size();
  unsigned long startRefCount = 0;
  unsigned long endRefCount = totalToolRefCount();
  unsigned long startMinRefCount = 0;
  unsigned long endMinRefCount = minimumToolRefCount();
  while ( toolCount > 0 && 
	  endRefCount > 0 && 
	  (endRefCount != startRefCount || endMinRefCount != startMinRefCount) ) {
    if ( endMinRefCount != startMinRefCount ) {
      log << MSG::DEBUG << toolCount << " tools left to finalize. Summed refCounts: " 
	  << endRefCount << endreq;
      log << MSG::DEBUG << "Will finalize tools with refCount <= " 
	  << endMinRefCount << endreq;
    }
    startMinRefCount = endMinRefCount;
    startRefCount = endRefCount;
    unsigned long maxLoop = toolCount + 1;
    while ( --maxLoop > 0 &&  m_instancesTools.size() > 0 ) {
      IAlgTool* pTool = m_instancesTools.back();
      // removing tool from list makes ToolSvc::releaseTool( IAlgTool* ) a noop
      m_instancesTools.pop_back();
      unsigned long count = refCountTool( pTool );
      // cache tool name
      std::string toolName = pTool->name();
      if ( count <= startMinRefCount ) {
	log << MSG::DEBUG << "  Performing finalization of " << toolName 
	    << " (refCount " << count << ")" << endreq;
	// finalize of one tool may trigger a release of another tool
	//	pTool->sysFinalize().ignore();
	if (!finalizeTool(pTool).isSuccess()) fail = true;
	// postpone deletion
	finalizedTools.push_back(pTool);
      } else {
	// Place back in list to try again later
	// ToolSvc::releaseTool( IAlgTool* ) remains active for this tool
	log << MSG::DEBUG << "  Delaying   finalization of " << toolName
	    << " (refCount " << count << ")" << endreq;
	m_instancesTools.push_front(pTool);
      }
    } // end of inner loop
    toolCount = m_instancesTools.size();
    endRefCount = totalToolRefCount();
    endMinRefCount = minimumToolRefCount();
  }; // end of outer loop

  //
  // Second pass: Delete all finalized tools
  //
  // Delete in the order of increasing number of refCounts. 
  // Loop over tools in the same order as the order in which they were finalized.
  // All tools in the list of finalized tools are no longer in the instancesTools list.
  // If a tool destructor calls releaseTool() on another tool, this will have no
  // effect on the 'other tool' if this 'other tool' is in the list of finalized tools.
  // If this 'other tool' is still in the instancesTools list, it may trigger finalization
  // (in releaseTool()), deletion and removal from the instancesTools list.
  // Therefore, the check on non-finalised tools should happen *after* the deletion
  // of the finalized tools.
  log << MSG::DEBUG << "Deleting " << finalizedTools.size() << " finalized tools" << endreq;
  unsigned long maxLoop = totalToolRefCount( finalizedTools ) + 1;
  while ( --maxLoop > 0 && finalizedTools.size() > 0 ) {
    IAlgTool* pTool = finalizedTools.front();
    finalizedTools.pop_front();
    unsigned long count = refCountTool( pTool );
    if ( count == 1 ) {
      log << MSG::DEBUG << "  Performing deletion of " << pTool->name() << endreq;
    } else {
      log << MSG::VERBOSE << "  Delaying   deletion of " << pTool->name()
	  << " (refCount " << count << ")" << endreq;
      // Put it back at the end of the list if refCount still not zero
      finalizedTools.push_back(pTool);
    }
    // do a forced release
    pTool->release();
  }

  // Error if by now not all tools are properly finalised
  if ( !m_instancesTools.empty() ) {
    log << MSG::ERROR << "Unable to finalize and delete the following tools : ";
    for ( ListTools::const_iterator iTool = m_instancesTools.begin();
          iTool != m_instancesTools.end(); ++iTool ) { 
      log << (*iTool)->name() << ": " << refCountTool( *iTool ) << " ";
    }
    log << endreq;
  }
  
  // by now, all tools should be deleted and removed.
  if ( finalizedTools.size() > 0 ) {
    log << MSG::ERROR << "Failed to delete the following " <<  finalizedTools.size() 
	<< " finalized tools. Bug in ToolSvc::finalize()?: ";
    for ( ListTools::const_iterator iTool = finalizedTools.begin();
          iTool != finalizedTools.end(); ++iTool ) { 
      log << (*iTool)->name() << ": " << refCountTool( *iTool ) << " ";
    }
    log << endreq;
  }
  
  if ( 0 != m_pHistorySvc ) {
    m_pHistorySvc->release();
  }
  
  m_isInFinalize = false;
  // Finalize this specific service
  if (! Service::finalize().isSuccess() || fail) {
    return StatusCode::FAILURE;
  } else {
    return StatusCode::SUCCESS;
  }


}

//------------------------------------------------------------------------------
StatusCode ToolSvc::queryInterface( const InterfaceID& riid, void** ppvInterface )
  //------------------------------------------------------------------------------
{
  if ( IID_IToolSvc == riid )    {
    *ppvInterface = (IToolSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// ===================================================================================
/** the indicator for the tool to be "PUBLIC"
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ===================================================================================
namespace
{
  const std::string s_PUBLIC = ":PUBLIC" ;
}

//------------------------------------------------------------------------------
StatusCode ToolSvc::retrieve ( const std::string& tooltype ,
                               const InterfaceID& iid      ,
                               IAlgTool*&         tool     ,
                               const IInterface*  parent   ,
                               bool               createIf )  
//------------------------------------------------------------------------------
{

  // protect against empty type
  if ( tooltype.empty() ) {
    MsgStream log( msgSvc(), name() );
    log << MSG::ERROR << "retrieve(): No Tool Type/Name given" << endreq;
    return StatusCode::FAILURE;
  }
  
  {
    // check for tools, which by name is required to be public:
    const std::string::size_type pos = tooltype.find ( s_PUBLIC ) ;
    if ( std::string::npos != pos )
    {
      // set parent for PUBLIC tool
      parent = this ;
      return retrieve ( std::string( tooltype , 0 , pos ) ,
                        iid , tool , parent , createIf ) ;
    }
  }

  const std::string::size_type pos = tooltype.find('/');
  if( std::string::npos == pos )
  { return retrieve ( tooltype , tooltype , iid , tool , parent , createIf );}
  const std::string newtype ( tooltype ,       0 , pos               ) ;
  const std::string newname ( tooltype , pos + 1 , std::string::npos ) ;
  return retrieve ( newtype , newname , iid , tool , parent , createIf ) ;
}

// ===================================================================================

//------------------------------------------------------------------------------
StatusCode ToolSvc::retrieve ( const std::string& tooltype ,
                               const std::string& toolname ,
                               const InterfaceID& iid      ,
                               IAlgTool*&         tool     ,
                               const IInterface*  parent   ,
                               bool               createIf )
  //------------------------------------------------------------------------------
{
  MsgStream log( msgSvc(), name() );
  
  // check the applicability of another method:
  // ignore the provided name if it is empty or the type contains a name
  if( toolname.empty() || (std::string::npos != tooltype.find('/')) )
  { return retrieve ( tooltype , iid , tool , parent , createIf ) ; }

  {
    // check for tools, which by name is required to be public:
    const std::string::size_type pos = toolname.find ( s_PUBLIC ) ;
    if ( std::string::npos != pos )
    {
      // set parent for PUBLIC tool
      parent = this ;
      return retrieve ( tooltype , std::string( toolname , 0 , pos ) ,
                        iid , tool , parent , createIf ) ;
    }
  }

  IAlgTool* itool = 0;
  StatusCode sc(StatusCode::FAILURE);

  tool = 0;

  // If parent is not specified it means it is the ToolSvc itself
  if( 0 == parent ) {
    parent = this;
  }
  const std::string fullname = nameTool( toolname, parent );

  // Find tool in list of those already existing, and tell its
  // interface that it has been used one more time
  ListTools::const_iterator it;
  for( it = m_instancesTools.begin(); it != m_instancesTools.end(); ++it ) {
    if( (*it)->name() == fullname ) {
      log << MSG::DEBUG << "Retrieved tool " << toolname << endreq;
      itool = *it;
      break;
    }
  }

  if ( 0 == itool ) {
    // Instances of this tool do not exist, create an instance if desired
    // otherwise return failure
    if( !createIf ) {
      log << MSG::WARNING << "Tool " << toolname
          << " not found and creation not requested" << endreq;
      return sc;
    }
    else {
      sc = create( tooltype, toolname, parent, itool );
      if ( sc.isFailure() ) { return sc; }
    }
  }

  // Get the right interface of it
  sc = itool->queryInterface( iid, (void**)&tool);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Tool " << toolname
        << " either does not implement the correct interface, or its version is incompatible"
        << endreq;
    return sc;
  }
  ///////////////
  /// invoke retrieve callbacks...
  ///////////////
  if (!m_observers.empty()) {
     std::for_each( m_observers.begin(), 
                    m_observers.end(), 
                    bl::bind(&IToolSvc::Observer::onRetrieve,
                             bl::_1,
                             itool));
  }

  return sc;
}
//------------------------------------------------------------------------------
std::vector<std::string> ToolSvc::getInstances( const std::string& toolType ) 
//------------------------------------------------------------------------------
{

  std::vector<std::string> tools;

  ListTools::const_iterator it;
  for (it = m_instancesTools.begin(); it != m_instancesTools.end(); ++it) {
    if ((*it)->type() == toolType) {
      tools.push_back( (*it)->name() );
    }
  }

  return tools;

}
//------------------------------------------------------------------------------
StatusCode ToolSvc::releaseTool( IAlgTool* tool )
  //------------------------------------------------------------------------------
{
  StatusCode sc(StatusCode::SUCCESS);
  // test if tool is in known list (protect trying to access a previously deleted tool)
  if ( m_instancesTools.rend() != std::find( m_instancesTools.rbegin(),
                                              m_instancesTools.rend(),
                                              tool ) ) {
    unsigned long count = refCountTool(tool);
    if ( count == 1 ) {
      MsgStream log( msgSvc(), name() + ".releaseTool()" );
      // finalize the tool
      if ( m_isInFinalize ) {
	// We are being called during ToolSvc::finalize()
	// message format matches the one in ToolSvc::finalize()
	log << MSG::DEBUG << "  Performing finalization of " << tool->name()
	    << " (refCount " << count << ")" << endreq;
	// message format matches the one in ToolSvc::finalize()
	log << MSG::DEBUG << "  Performing     deletion of " << tool->name() << endreq;
      } else {
	log << MSG::DEBUG << "Performing finalization and deletion of " << tool->name() << endreq;
      }
      sc = finalizeTool(tool);
      // remove from known tools...
      m_instancesTools.remove(tool);
    }
    tool->release();
  }

  return sc;
}

//------------------------------------------------------------------------------
StatusCode ToolSvc::create(const std::string& tooltype,
                           const IInterface* parent,
                           IAlgTool*& tool)
  //------------------------------------------------------------------------------
{
  const std::string & toolname = tooltype;
  return create( tooltype, toolname, parent, tool);
}

//------------------------------------------------------------------------------
StatusCode ToolSvc::create(const std::string& tooltype,
                           const std::string& toolname,
                           const IInterface* parent,
                           IAlgTool*& tool)
  //------------------------------------------------------------------------------
{
  MsgStream log( msgSvc(), name() );
  // protect against empty type
  if ( tooltype.empty() ) {
    log << MSG::ERROR << "create(): No Tool Type given" << endreq;
    return StatusCode::FAILURE;
  }

  // If parent has not been specified, assume it is the ToolSvc
  if ( 0 == parent ) parent = this;

  tool = 0;

  // Check if the tool already exist : this should never happen
  const std::string fullname = nameTool(toolname, parent);
  if( existsTool( fullname ) ) {
    log << MSG::ERROR << "Tool " << fullname << " already exists" << endreq;
    return StatusCode::FAILURE;
  }
  // instanciate the tool using the factory
  try {
    tool = PluginService::Create<IAlgTool*>(tooltype, tooltype, fullname, parent);
    if (!tool ){
       log << MSG::ERROR
           << "Cannot create tool " << tooltype << " (No factory found)" << endreq;
       return StatusCode::FAILURE;
    }
  }
  catch ( const GaudiException& Exception )  {
    // (1) perform the printout of message
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "Exception with tag=" << Exception.tag()
	<< " is caught whilst instantiating tool '" << tooltype << "'" << endreq;
    // (2) print  the exception itself
    // (NB!  - GaudiException is a linked list of all "previous exceptions")
    log << MSG::FATAL << Exception  << endreq;
    tool = 0;
    return StatusCode::FAILURE;
  }
  catch( const std::exception& Exception ) {
    // (1) perform the printout of message
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL
	<< "Standard std::exception is caught whilst instantiating tool '"
          << tooltype << "'" << endreq;
    // (2) print  the exception itself
    // (NB!  - GaudiException is a linked list of all "previous exceptions")
    log << MSG::FATAL << Exception.what()  << endreq;
    tool = 0;
    return StatusCode::FAILURE;
  }
  catch(...) {
    // (1) perform the printout
    MsgStream log ( msgSvc() , name() );
    log << MSG::FATAL << "UNKNOWN Exception is caught whilst instantiating tool '"
	<< tooltype << "'" << endreq;
    tool = 0;
    return StatusCode::FAILURE;
  }
  log << MSG::VERBOSE << "Created tool " << tooltype << "/" << fullname << endreq;
  
  m_instancesTools.push_back( tool );
  
  // Since only AlgTool has the setProperties() method it is necessary to cast
  // to downcast IAlgTool to AlgTool in order to set the properties via the JobOptions
  // service
  AlgTool* mytool = dynamic_cast<AlgTool*> (tool);
  if ( mytool != 0 ) {
    StatusCode sc = mytool->setProperties();
    if ( sc.isFailure() ) {
      MsgStream log ( msgSvc() , name() );
      log << MSG::ERROR << "Error setting properties for tool '"
	  << fullname << "'" << endreq;
      return sc;
    }
  }
  
  // Initialize the Tool
  StatusCode sc (StatusCode::FAILURE,true);
  try { sc = tool->sysInitialize(); }
  
  // Catch any exceptions
  catch ( const GaudiException & Exception )
    {
      MsgStream msg ( msgSvc(), name() );
      msg << MSG::ERROR
          << "GaudiException with tag=" << Exception.tag()
          << " caught whilst initializing tool '" << fullname << "'" << endreq
          << Exception << endreq;
      return StatusCode::FAILURE;
    }
  catch( const std::exception & Exception )
    {
      MsgStream msg ( msgSvc(), name() );
      msg << MSG::ERROR
          << "Standard std::exception caught whilst initializing tool '" 
          << fullname << "'" << endreq << Exception.what() << endreq;
      return StatusCode::FAILURE;
    }
  catch (...)
    {
      MsgStream msg ( msgSvc(), name() );
      msg << MSG::ERROR
          << "UNKNOWN Exception caught whilst initializing tool '" 
          << fullname << "'" << endreq;
      return StatusCode::FAILURE;
    }
  
  // Status of tool intialization
  if ( sc.isFailure() ) {
    MsgStream log( msgSvc(), name() );
    log << MSG::ERROR << "Error initializing tool '" << fullname << "'" << endreq;
    return sc;
  }
  
  ///////////////
  /// invoke create callbacks...
  ///////////////
  if (!m_observers.empty()) {
      std::for_each( m_observers.begin(), 
                     m_observers.end(), 
                     bl::bind(&IToolSvc::Observer::onCreate,
                              bl::_1,
                              tool));
  }
  // TODO: replace by generic callback
  // Register the tool with the HistorySvc
  if (m_pHistorySvc != 0 || 
      service("HistorySvc",m_pHistorySvc,false).isSuccess() ) {
    m_pHistorySvc->registerAlgTool(*tool).ignore();
  }
  
  return StatusCode::SUCCESS;
 
}

//------------------------------------------------------------------------------
std::string ToolSvc::nameTool( const std::string& toolname,
                               const IInterface* parent )
  //------------------------------------------------------------------------------
{
  
  std::string fullname = "";
  if ( parent == 0 ) { return this->name() + "." + toolname; }    // RETURN 
  
  
  IInterface* cparent = const_cast<IInterface*>( parent ) ;
  // check that parent has a name!
  INamedInterface* _p = 0 ;
  StatusCode sc = cparent->queryInterface( INamedInterface::interfaceID() , pp_cast<void>(&_p) ) ;
  if ( sc.isSuccess() )
  { 
    fullname = _p->name() + "." + toolname ; 
    _p->release() ;
    return fullname ;                                          // RETURN 
  }

  MsgStream log ( msgSvc(), name() );
  log << MSG::ERROR
      << "Private Tools only allowed for INamedInterfaces"
      << endreq;
  //
  return "." + toolname ;
}

//------------------------------------------------------------------------------
bool ToolSvc::existsTool( const std::string& fullname) const
  //------------------------------------------------------------------------------
{
  for ( ListTools::const_iterator it = m_instancesTools.begin(); 
        it != m_instancesTools.end(); ++it ) {
    if ( (*it)->name() == fullname ) { return true; }
  }
  return false;
}

//------------------------------------------------------------------------------
StatusCode ToolSvc::finalizeTool( IAlgTool* itool ) const
  //------------------------------------------------------------------------------
{

  // Cache tool name in case of errors
  const std::string toolName = itool->name();
  StatusCode sc;

  // Finalise the tool inside a try block
  try { sc = itool->sysFinalize(); }

  // Catch any exceptions
  catch ( const GaudiException & Exception )
  {
    MsgStream msg ( msgSvc(), name() );
    msg << MSG::ERROR
        << "GaudiException with tag=" << Exception.tag()
        << " caught whilst finalizing tool '" << toolName << "'" << endreq
        << Exception << endreq;
    sc = StatusCode::FAILURE;
  }
  catch( const std::exception & Exception )
  {
    MsgStream msg ( msgSvc(), name() );
    msg << MSG::ERROR
        << "Standard std::exception caught whilst finalizing tool '" 
        << toolName << "'" << endreq << Exception.what() << endreq;
    sc = StatusCode::FAILURE;
  }
  catch (...)
  {
    MsgStream msg ( msgSvc(), name() );
    msg << MSG::ERROR
        << "UNKNOWN Exception caught whilst finalizing tool '" 
        << toolName << "'" << endreq;
    sc = StatusCode::FAILURE;
  }

  return sc;

}

//------------------------------------------------------------------------------
unsigned long ToolSvc::totalToolRefCount( const ToolSvc::ListTools& toolList ) const
//------------------------------------------------------------------------------
{
  unsigned long count = 0;
  for ( ListTools::const_iterator iTool = toolList.begin();
	iTool != toolList.end(); ++iTool ) {
    count += refCountTool( *iTool );
  }
  return count;
}

//------------------------------------------------------------------------------
unsigned long ToolSvc::totalToolRefCount() const
//------------------------------------------------------------------------------
{
  return totalToolRefCount( m_instancesTools );
}
//------------------------------------------------------------------------------
unsigned long ToolSvc::minimumToolRefCount() const
//------------------------------------------------------------------------------
{
  unsigned long count = 0;
  if ( m_instancesTools.size() > 0 ) {
    ListTools::const_iterator iTool = m_instancesTools.begin();
    // start with first
    count = refCountTool( *iTool );
    // then compare the others
    for( ++iTool; iTool != m_instancesTools.end(); ++iTool ) {
      count = std::min( count, refCountTool( *iTool ) );
    }
  }
  return count;
}

void ToolSvc::registerObserver(IToolSvc::Observer* obs) {
  if ( 0 == obs )
    throw GaudiException( "Received NULL pointer", this->name() + "::registerObserver", StatusCode::FAILURE );
  m_observers.push_back(obs); 
}

void ToolSvc::unRegisterObserver(IToolSvc::Observer* obs) {
  std::vector<IToolSvc::Observer*>::iterator i = 
    find(m_observers.begin(),m_observers.end(),obs); 
  if (i!=m_observers.end()) m_observers.erase(i);
}
