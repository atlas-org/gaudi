// $Id: DataOnDemandSvc.cpp,v 1.15 2008/01/14 19:42:15 marcocle Exp $
// ============================================================================
// CVS tag $Name:  $ 
// ============================================================================
// Incldue files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <set>
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ListItem.h"
#include "DataOnDemandSvc.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/format.hpp"
// ============================================================================

// Instantiation of a static factory class used by clients to create
//  instances of this service
DECLARE_SERVICE_FACTORY(DataOnDemandSvc)

//============================================================================================
// Constructors and Desctructors
//============================================================================================
DataOnDemandSvc::DataOnDemandSvc 
( const std::string& name, ISvcLocator* svc )
  : Service(name, svc)
  , m_incSvc   ( 0 )
  , m_algMgr   ( 0 )
  , m_dataSvc  ( 0 )
  //
  , m_algMap   (   ) 
  , m_nodeMap  (   )  
  , m_algMap_  ( 0 ) 
  , m_nodeMap_ ( 0 )  
  //
  , m_updateRequired ( true )
  , m_prefix         ( "/Event/" )
  , m_log      ( 0 )
  , m_total    (   ) 
  , m_statAlg  ( 0 ) 
  , m_statNode ( 0 ) 
  , m_stat     ( 0 )
{
  declareProperty ( "Algorithms"         , m_algMapping                   ) ;
  declareProperty ( "Nodes"              , m_nodeMapping                  ) ;
  declareProperty ( "UsePreceedingPath"  , m_partialPath = true           ) ;
  declareProperty ( "IncidentName"       , m_trapType    = "DataFault"    ) ;
  declareProperty ( "DataSvc"            , m_dataSvcName = "EventDataSvc" ) ;
  declareProperty ( "Prefix"             , m_prefix                       ) ;
  // 
  m_algMap_  = new MapRef ( "AlgMap"  , m_algMap  ) ;
  m_nodeMap_ = new MapRef ( "NodeMap" , m_nodeMap ) ;
  declareProperty ( m_algMap_  -> name () , *m_algMap_  ) ;
  declareProperty ( m_nodeMap_ -> name () , *m_nodeMap_ ) ;
  m_algMap_  ->  declareUpdateHandler ( &DataOnDemandSvc::update_1 , this ) ;
  m_nodeMap_ ->  declareUpdateHandler ( &DataOnDemandSvc::update_1 , this ) ;
  //
  m_algMapping  .declareUpdateHandler ( &DataOnDemandSvc::update_2 , this ) ;
  m_nodeMapping .declareUpdateHandler ( &DataOnDemandSvc::update_3 , this ) ;
} 
// ============================================================================
/// Update handler 
// ============================================================================
void DataOnDemandSvc::update_1 ( Property& p ) 
{
  stream() 
    << MSG::VERBOSE << " I am update handler for property " << p << endreq ;
  // force update 
  m_updateRequired = true ;
} 
// ============================================================================
/// Update handler 
// ============================================================================
void DataOnDemandSvc::update_3 ( Property& p ) 
{
  stream() 
    << MSG::VERBOSE << " I am update handler for property " << p << endreq ;
  if ( setupNodeHandlers().isFailure() ) 
  {
    stream() 
      << MSG::ERROR << " Error from setupNodeHandlers " << endreq ;
  }
  // force update 
  m_updateRequired = true ;
} 
// ============================================================================
/// Update handler 
// ============================================================================
void DataOnDemandSvc::update_2 ( Property& p ) 
{
  stream() 
    << MSG::VERBOSE << " I am update handler for property " << p << endreq ;
  if ( setupAlgHandlers().isFailure() ) 
  {
    stream() 
      << MSG::ERROR << " Error from setupAlgHandlers " << endreq ;
  }
  // force update 
  m_updateRequired = true ;
}
// ============================================================================
/// anonymous namespace to hide few local functions 
// ============================================================================
namespace 
{
  /** remove the prefix from the string
   *  @param value input tring
   *  @param prefox prefix to be removed 
   *  @return input string without prefix 
   */
  inline std::string no_prefix 
  ( const std::string& value  , 
    const std::string& prefix )
  {
    return 
      !prefix.empty() && 0 == value.find(prefix) ? 
      std::string( value , prefix.size() ) : value ;
  }
  /** add a prefix (if needed) to all keys of the map
   *  the previus key is removed
   *  @param _map map to be modified 
   *  @param prefix prefix to be added 
   *  @return number of modified keys 
   */
  template <class MAP>
  inline size_t add_prefix ( MAP& _map , const std::string& prefix ) 
  {
    // empty  prefix 
    if ( prefix.empty() ) { return 0 ; }                    // RETURN 
    /// loop over all entries to find the  proper keys 
    for ( typename MAP::iterator it = _map.begin() ; _map.end() != it ; ++it ) 
    {
      if ( 0 != it->first.find(prefix) )  // valid prefix? 
      { 
        std::string key   = prefix + it->first ;
        std::string value = it->second ;
        _map.erase ( it ) ;
        _map[ key ] = value  ;
        return 1 + add_prefix ( _map , prefix ) ;    // RETURN, recursion
      }
    }
    //
    return 0 ;
  }
  /** get the list of directories for a certain TES location
   *  @param object to be inspected
   *  @param _set (output) the set with directoiry list
   *  @return incrment of the output set size 
   */
  template <class SET>
  inline size_t get_dir ( const std::string& object , SET& _set ) 
  {
    std::string::size_type ifind = object.rfind('/') ;
    // stop recursion 
    if ( std::string::npos == ifind ) { return 0 ; } // RETURN 
    if ( 0 == ifind                 ) { return 0 ; }
    //
    const std::string top = std::string( object , 0 , ifind) ;
    _set.insert( top ) ;
    return 1 + get_dir ( top , _set ) ;   // RETURN, recursion 
  } 
  /** get the list of directories for a all keys of the input map
   *  @param _map (input) map to be inspected 
   *  @param _set (output) the set with directoiry list
   *  @return incrment of the output set size 
   */
  template <class MAP, class SET>
  inline size_t get_dirs ( const MAP& _map, SET& _set ) 
  {
    size_t size = _set.size() ;
    for ( typename MAP::const_iterator item = _map.begin() ; 
          _map.end() != item ; ++item ) {  get_dir ( item->first , _set ) ; }
    return _set.size() - size ;
  } 
} // end of anonymous namespace
// ============================================================================
// update the content of Data-On-Demand actions 
// ============================================================================
StatusCode DataOnDemandSvc::update () 
{
  /// add the default prefix 
  add_prefix ( m_algMap  , m_prefix ) ;
  /// add the default prefix 
  add_prefix ( m_nodeMap , m_prefix ) ;
  /// get all directories 
  typedef std::set<std::string> Set ; 
  Set dirs ;
  if ( m_partialPath ){ get_dirs ( m_algMap  , dirs ) ; }
  if ( m_partialPath ){ get_dirs ( m_nodeMap , dirs ) ; }
  //
  Set::iterator _e = dirs.find("/Event") ;
  if ( dirs.end() != _e ) { dirs.erase( _e ) ; }
  // add all directories as nodes 
  for ( Set::const_iterator dir = dirs.begin() ; dirs.end() != dir ; ++dir ) 
  {
    if ( m_algMap  .end () != m_algMap  .find ( *dir ) ) { continue ; }
    if ( m_nodeMap .end () != m_nodeMap .find ( *dir ) ) { continue ; }    
    m_nodeMap [*dir] = "DataObject" ;  
  }
  //
  m_algs  .clear  () ;
  m_nodes .clear () ;
  //
  /// setup algorithms
  for ( Map::const_iterator ialg = m_algMap.begin() ; 
        m_algMap.end() != ialg ; ++ialg )
  {
    ListItem alg ( ialg->second ) ;
    m_algs[ialg->first] =  Leaf( alg.type() , alg.name() ) ;
  }
  /// setup nodes 
  for ( Map::const_iterator inode = m_nodeMap.begin() ; 
        m_nodeMap.end() != inode ; ++inode ) 
  {
    ClassH cl = ROOT::Reflex::Type::ByName( inode->second ) ;
    if ( !cl ) 
    {
      stream() << MSG::WARNING 
               << "Failed to access dictionary class for "
               << inode->first << " of type:" << inode->second << endmsg;
    }
    m_nodes[inode->first] = Node ( cl , false , inode->second ) ;
  }
  ///
  m_updateRequired = false ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// destructor 
// ============================================================================
DataOnDemandSvc::~DataOnDemandSvc() 
{ 
  if ( 0 != m_algMap_  ) { delete m_algMap_  ; m_algMap_  = 0 ; }
  if ( 0 != m_nodeMap_ ) { delete m_nodeMap_ ; m_nodeMap_ = 0 ; }
  if ( 0 != m_log      ) { delete m_log      ; m_log      = 0 ; }
}
//=============================================================================
// Inherited Service overrides:
//=============================================================================
StatusCode DataOnDemandSvc::initialize() 
{
  // initialize the Service Base class
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() )  { return sc; }
  sc = setup();
  if ( sc.isFailure() )  { return sc; }
  return dump( ) ;
}
// ============================================================================
/// dump the configuration 
// ============================================================================
StatusCode DataOnDemandSvc::dump ( MSG::Level level )  const 
{
  if ( !m_algs.empty()  || m_nodes.empty() ) 
  {
    stream() << level << "DataOnDemand Actions enabled for:" << endmsg;
    Map _m ;
    for ( AlgMap::const_iterator alg = m_algs.begin() ; 
          m_algs.end() != alg ; ++alg )
    {
      Map::const_iterator check = _m.find(alg->first) ;
      if ( _m.end() != check ) 
      {
        stream() << MSG::WARNING 
                 << " The data item is activated for '"
                 << check->first << "' as '" << check->second << "'" << endreq ;
      }
      boost::format fmt ("%1% %|40t|%2%") ;
      const Leaf& l = alg->second ;
      fmt 
        % ( l.name == l.type ? l.type  : (l.type+"/"+l.name) ) 
        % ( 0 == l.algorithm ? "F"     : "T"                 ) ;
      _m[alg->first] = boost::str( fmt ) ;
    }
    for ( NodeMap::const_iterator node = m_nodes.begin() ; 
          m_nodes.end() != node ; ++node )
    {
      Map::const_iterator check = _m.find(node->first) ;
      if ( _m.end() != check ) 
      {
        stream() << MSG::WARNING 
                 << " The data item is already activated for '"
                 << check->first << "' as '" << check->second << "'" << endreq ;
      }
      boost::format fmt ("%1% %|40t|%2%") ;
      const Node& n = node->second ;
      fmt % ( "'" + n.name + "'" ) % ( n.clazz ? "T" : "F" ) ;
      _m[node->first] = boost::str( fmt ) ;
    }
    for ( Map::const_iterator item = _m.begin() ; _m.end() != item ; ++item ) 
    {
      boost::format fmt( "  %1% %|45t|%2% " ) ;
      fmt % no_prefix( item->first , m_prefix )  % item->second ;
      stream() << level  << boost::str( fmt ) << endreq ;
    }
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// finalization of the service 
// ============================================================================
StatusCode DataOnDemandSvc::finalize() 
{
  stream() << MSG::INFO 
           <<  " #Incidents: Alg/Node/All:"
           << " " << m_statAlg << "/" << m_statNode << "/" << m_stat 
           << " Algorithms: " << m_total.outputUserTime () << endreq ;
  
  // dump it! 
  dump ( MSG::DEBUG ).ignore() ;
  //
  if ( m_incSvc )  
  {
    m_incSvc->removeListener(this, m_trapType);
    m_incSvc->release();
    m_incSvc = 0;
  }
  if ( 0 != m_algMgr  ) { m_algMgr   -> release () ; m_algMgr  = 0 ; }
  if ( 0 != m_dataSvc ) { m_dataSvc  -> release () ; m_dataSvc = 0 ; }
  if ( 0 != m_log     ) { delete m_log ; m_log = 0 ; }
  //
  return Service::finalize();
}
// ============================================================================
/// re-initialization of the service 
// ============================================================================
StatusCode DataOnDemandSvc::reinitialize() 
{
  // reinitialize the Service Base class
  if ( 0 != m_incSvc )  
  {
    m_incSvc->removeListener(this, m_trapType);
    m_incSvc->release();
    m_incSvc = 0;
  }
  if ( 0 != m_algMgr  ) { m_algMgr   -> release() ; m_algMgr  = 0 ; }
  if ( 0 != m_dataSvc ) { m_dataSvc  -> release() ; m_dataSvc = 0 ; }
  //
  StatusCode sc = Service::reinitialize();
  if ( sc.isFailure() )  { return sc; }
  //
  sc = setup() ;
  if ( sc.isFailure() )  { return sc; }
  //
  return dump() ;
}
// ============================================================================
/// query interface 
// ============================================================================
StatusCode DataOnDemandSvc::queryInterface
( const InterfaceID& riid, 
  void** ppvInterface ) 
{
  if ( IID_IIncidentListener == riid )    
  {
    *ppvInterface = static_cast<IIncidentListener*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly available: try out a base class
  return Service::queryInterface(riid, ppvInterface);
}
// ============================================================================
/// setup service 
// ============================================================================
StatusCode DataOnDemandSvc::setup() 
{
  m_algMgr = 0;
  StatusCode sc = 
    serviceLocator()->queryInterface(IID_IAlgManager, pp_cast<void>(&m_algMgr));
  if ( sc.isFailure () )  
  {
    stream() 
      << MSG::ERROR
      << "Failed to retrieve the IAlgManager interface." << endmsg;
    return sc;
  }
  sc = service("IncidentSvc", m_incSvc, true);
  if ( sc.isFailure () )  
  {
    stream() 
      << MSG::ERROR << "Failed to retrieve Incident service." << endmsg;
    return sc;
  }
  m_incSvc->addListener(this, m_trapType);
  sc = service(m_dataSvcName, m_dataSvc, true);
  if ( sc.isFailure () )  
  {
    stream() 
      << MSG::ERROR
      << "Failed to retrieve the data provider interface of "
      << m_dataSvcName << endmsg;
    return sc;
  }
  sc = setupNodeHandlers() ;
  if ( sc.isFailure() ) 
  {
    stream() << MSG::ERROR << "Failed to setup nodes " << endreq ;
    return sc ;
  }
  sc = setupAlgHandlers();
  if ( sc.isFailure() ) 
  {
    stream() << MSG::ERROR << "Failed to setup algorithms " << endreq ;
    return sc ;
  }
  //
  return update() ;
}
// ============================================================================
/// setup node handlers 
// ============================================================================
StatusCode DataOnDemandSvc::setupNodeHandlers()  
{
  Setup::const_iterator j;
  std::string nam, typ, tag;
  StatusCode sc = StatusCode::SUCCESS;
  // Setup for node leafs, where simply a constructor is called...
  for ( j=m_nodeMapping.value().begin(); j != m_nodeMapping.value().end(); ++j) 
  {
    Tokenizer tok(true);
    tok.analyse(*j, " ", "", "", "=", "'", "'");
    for ( Tokenizer::Items::iterator i = tok.items().begin(); 
          i != tok.items().end(); i++ )   {
      const std::string& t = (*i).tag();
      const std::string& v = (*i).value();
      switch( ::toupper(t[0]) )    {
      case 'D':
        tag = v;
        break;
      case 'T':
        nam = v;
        break;
      }
    } 
    m_nodeMap[tag] = nam ; 
  }
  //
  m_updateRequired = true ;
  //
  return sc;
}
// ============================================================================
/// setup algorithm  handlers 
// ============================================================================
StatusCode DataOnDemandSvc::setupAlgHandlers()  
{
  Setup::const_iterator j;
  std::string typ, tag;
  m_algs.clear();
  for(j=m_algMapping.value().begin(); j != m_algMapping.value().end(); ++j)  
  {
    Tokenizer tok(true);
    tok.analyse(*j, " ", "", "", "=", "'", "'");
    for(Tokenizer::Items::iterator i = tok.items().begin(); i != tok.items().end(); i++ )   {
      const std::string& t = (*i).tag();
      const std::string& v = (*i).value();
      switch( ::toupper(t[0]) )    {
      case 'D':
        tag = v;
        break;
      case 'T':
        typ = v;
        break;
      }
    }
    ListItem item(typ);
    m_algMap[tag] = item.type() + "/" + item.name() ;
  }
  m_updateRequired = true ;
  return StatusCode::SUCCESS;
}
// ============================================================================
/// Configure handler for leaf
// ============================================================================
StatusCode DataOnDemandSvc::configureHandler(Leaf& l)  
{
  if ( 0 == m_algMgr  ) { return StatusCode::FAILURE ; }
  StatusCode sc = m_algMgr->getAlgorithm(l.name, l.algorithm);
  if ( sc.isFailure() ) 
  {
    sc = m_algMgr->createAlgorithm(l.type, l.name, l.algorithm, true );
    if ( sc.isFailure() ) 
    {
      stream()  
        << MSG::ERROR 
        << "Failed to create algorithm of type:"
        << l.name << "[" << l.type << "] " << endmsg; 
    }
  }
  return sc;
}
// ===========================================================================
/// IIncidentListener interfaces overrides: incident handling
// ===========================================================================
void DataOnDemandSvc::handle ( const Incident& incident )
{ 
  ++m_stat ;
  // proper incident type? 
  if ( incident.type() != m_trapType ) { return ; }             // RETURN
  const DataIncident* inc = dynamic_cast<const DataIncident*>(&incident);
  if ( 0 == inc                      ) { return ; }             // RETURN
  // update if needed! 
  if ( m_updateRequired ) { update() ; }
  const std::string& tag = inc->tag();
  stream()
    << MSG::DEBUG 
    << "Incident: [" << incident.type   () << "] " 
    << " = "         << incident.source ()
    << " Location:"  << tag  << endmsg;
  NodeMap::iterator icl = m_nodes.find ( tag ) ;
  if ( icl != m_nodes.end() )  
  {
    StatusCode sc = execHandler ( tag , icl->second ) ;
    if ( sc.isSuccess() ) { ++m_statNode ; } 
    return ;                                                        // RETURN 
  }
  AlgMap::iterator ialg = m_algs.find ( tag ) ;
  if ( ialg != m_algs.end() )  
  {
    StatusCode sc = execHandler ( tag , ialg->second ) ;
    if ( sc.isSuccess() ) { ++m_statAlg ; }
    return ;                                                        // RETURN 
  }
}
// ===========================================================================
/// ecxecute the handler 
// ===========================================================================
StatusCode 
DataOnDemandSvc::execHandler(const std::string& tag, Node& n)  
{
  if ( n.executing ) { return StatusCode::FAILURE ; }            // RETURN 
 
  // try to recover the handler 
  if ( !n.clazz  ) { n.clazz = ROOT::Reflex::Type::ByName(n.name) ; }
  if ( !n.clazz  ) 
  {
    stream() 
      << MSG::ERROR 
      << "Failed to get dictionary for class '"
      << n.name  
      << "' for location:" << tag << endmsg;
    return StatusCode::FAILURE ;                               // RETURN 
  }
  
  ROOT::Reflex::Object obj = n.clazz.Construct();
  DataObject* pO = (DataObject*)obj.Address();
  if ( !pO )
  {
    stream() 
      << MSG::ERROR 
      << "Failed to create an object of type:"
      << n.clazz.Name(ROOT::Reflex::SCOPED) << " for location:" << tag 
      << endmsg;  
    return StatusCode::FAILURE  ;                               // RETURN 
  }
  //
  Protection p(n.executing);
  StatusCode sc = m_dataSvc->registerObject(tag, pO);
  if ( sc.isFailure() ) 
  {
    stream() 
      << MSG::ERROR << "Failed to register an object of type:"
      << n.clazz.Name(ROOT::Reflex::SCOPED) << " at location:" << tag 
      << endmsg;
    return sc ;                                                  // RETURN 
  }
  //
  return StatusCode::SUCCESS ;
}
// ===========================================================================
/// ecxecute the handler 
// ===========================================================================
StatusCode 
DataOnDemandSvc::execHandler(const std::string& tag, Leaf& l)  
{
  //
  if ( l.executing ) { return StatusCode::FAILURE ; }             // RETURN  
  //
  if ( 0 == l.algorithm ) 
  {
    StatusCode sc = configureHandler ( l ) ;
    if ( sc.isFailure() ) 
    {
      stream()  
        << MSG::ERROR 
        << "Failed to configure handler for: "
        << l.name << "[" << l.type << "] " << tag << endmsg; 
      return sc ;                                                 // RETURN 
    }
  }
  //
  Timer timer ( m_total ) ;
  //
  Protection p(l.executing);
  StatusCode sc = l.algorithm->sysExecute();
  if ( sc.isFailure() )  
  {
    stream() << MSG::ERROR 
             << "Failed to execute the algorithm:"
             << l.algorithm->name() << " for location:" << tag << endmsg;
    return sc ;                                                       // RETURN 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================
