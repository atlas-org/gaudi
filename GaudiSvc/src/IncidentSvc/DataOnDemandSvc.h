// $Id: DataOnDemandSvc.h,v 1.8 2007/05/24 14:41:23 hmd Exp $ 
// ============================================================================
// CVS tag $Name:  $ 
// ============================================================================
#ifndef GAUDISVC_DATAONDEMANDSVC_H
#define GAUDISVC_DATAONDEMANDSVC_H
// ============================================================================
// Include Files
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ChronoEntity.h"
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// Reflex
// ============================================================================
#include "Reflex/Reflex.h"
// ============================================================================
// Forward declarations
// ============================================================================
class IAlgTool;
class IAlgorithm;
class IAlgManager;
class IIncidentSvc;
class IDataProviderSvc;
// ============================================================================
/** @class DataOnDemandSvc DataOnDemandSvc.h IncidentSvc/DataOnDemandSvc.h
 *
 * The DataOnDemandSvc listens to incidents typically
 * triggered by the data service of the configurable name
 * "IncidentName".
 * In the job options handlers can be declared, which allow
 * to configure this service. Such handlers are either:
 *
 * - Node handlers, if objects other than the default object
 *   type have to be instantiated.
 *   DataOnDemandSvc.Nodes = { 
 *     "DATA='/Event/Rec'      TYPE='DataObject'",
 *     "DATA='/Event/Rec/Muon' TYPE='DataObject'"
 *   };
 *
 * - Leaf handlers (Algorithms), which get instantiated
 *   and executed on demand.
 *   DataOnDemandSvc.Algorithms = { 
 *     "DATA='/Event/Rec/Muon/Digits' TYPE='MuonDigitAlg/MyMuonDigits'"
 *   };
 *   If the algorithm name is omitted the class name will be the
 *   instance name.
 *   

 * The handlers only get called if the exact path mathes.
 * In the event already the partial path to any handler is
 * missing a leaf handler may be triggered, which includes 
 * the partal pathes ( DataOnDemandSvc.UsePreceedingPath = true )
 *
 *
 *  2006-10-15: New options  (using map-like semantics:) 
 *
 *  @code 
 *   
 *    DataOnDemandSvc.AlgMap  += 
 *   { "Phys/StdLoosePions/Particles" : "PreLoadParticles/StdLoosePions" , 
 *     "Phys/StdLoosePions/Vertioces" : "PreLoadParticles/StdLoosePions" } ;
 *
 *    DataOnDemandSvc.NodeMap +=
 *   { "Phys" : "DataObject" , 
 *     "MC"   : "DataObject" } ;
 * 
 *  @endcode 
 *
 *    New treatment of preceeding paths. for each registered leaf or node
 *    the all parent nodes are added into the node-map with default 
 *    directory type 'DataObject'
 * 
 *    The major properties are equipped with handlers 
 *    (more or less mandatory for interactive work in python)
 * 
 *    From now the default prefix ( "/Event/" ) coudl be omitted from 
 *    any data-item. It will be added automatically.  
 *
 * @author  M.Frank
 * @version 1.0
 */
class DataOnDemandSvc 
  :         public Service
  , virtual public IIncidentListener 
{
public: 
  // Typedefs
  typedef std::vector<std::string> Setup;
  typedef ROOT::Reflex::Type       ClassH;
  /** @struct Protection
   *  Helper class of the DataOnDemandSvc 
   *  @author  M.Frank
   */
  struct Protection 
  {
    bool& m_bool;
    Protection(bool& b) : m_bool(b) { m_bool = true;  }
    ~Protection()                   { m_bool = false; }
  };
  /** @struct Node
   *  Helper class of the DataOnDemandSvc 
   *  @author  M.Frank
   *  @version 1.0
   */
  struct Node  
  {
    ClassH      clazz;
    bool        executing;
    std::string name;
    Node() : clazz() , executing(false) , name () {}
    Node(ClassH c, bool e, const std::string& n)
      : clazz(c), executing(e), name(n) {}
    Node(const Node& c)
      : clazz(c.clazz), executing(c.executing), name(c.name){}
  };
  /// @struct Leaf
  struct Leaf  
  {
    IAlgorithm* algorithm;
    bool        executing;
    std::string name;
    std::string type;
    Leaf() : algorithm ( 0 ) , executing (false ) , name() , type() {}
    Leaf(const Leaf& l) 
      : algorithm(l.algorithm), 
        executing(l.executing), name(l.name), type(l.type) {}
    Leaf(const std::string& t, const std::string& n)
      : algorithm(0), executing(false), name(n), type(t) {}
  };
  /// @struct Timer 
  struct Timer 
  {
  public:
    Timer  ( ChronoEntity& c ) 
      : m_timer ( c ) { m_timer.start () ; }
    ~Timer ( )        { m_timer.stop  () ; }
  private:
    Timer() ;
  private:
    ChronoEntity& m_timer ;
  } ;
  ///
  typedef std::map<std::string, Node>  NodeMap;
  typedef std::map<std::string, Leaf>  AlgMap;
  /// Inherited Service overrides: Service initialization
  virtual StatusCode initialize();
  /// Inherited Service overrides: Service finalization
  virtual StatusCode finalize();
  /// Inherited Service overrides: Service reinitialization
  virtual StatusCode reinitialize();
  /// Inherited Service overrides: query existing interfaces
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  /// IIncidentListener interfaces overrides: incident handling
  virtual void handle(const Incident& incident);
  /** Standard initializing service constructor.
   *  @param   name   [IN]    Service name
   *  @param   svc    [IN]    Pointer to service locator
   *  @return Reference to DataOnDemandSvc object.
   */
  DataOnDemandSvc( const std::string& name, ISvcLocator* svc );
  /// Standard destructor.
  virtual ~DataOnDemandSvc();
protected:
  /** Configure handler for leaf
   *  @param   leaf   [IN]    Reference to leaf handler
   *  @return StatusCode indicating success or failure
   */
   StatusCode configureHandler(Leaf& leaf);
  /** Execute leaf handler (algorithm)
   *  @param   tag    [IN]    Path to requested leaf
   *  @param   leaf   [IN]    Reference to leaf handler
   *  @return StatusCode indicating success or failure
   */
  StatusCode execHandler(const std::string& tag, Leaf& leaf);
  /** Execute node handler (simple object creation using seal reflection)
   *  @param   tag    [IN]    Path to requested leaf
   *  @param   node   [IN]    Reference to node handler
   *  @return StatusCode indicating success or failure
   */
  StatusCode execHandler(const std::string& tag, Node& node);
  /// Initialize node handlers
  StatusCode setupNodeHandlers();
  /// Initialize leaf handlers
  StatusCode setupAlgHandlers();
  /// Setup routine (called by (re-) initialize
  StatusCode setup();
public:
  void update_1 ( Property& p ) ;
  void update_2 ( Property& p ) ;
  void update_3 ( Property& p ) ;
protected:
  /// update the handlers 
  StatusCode update() ;
  /// get the message stream 
  inline MsgStream& stream () const 
  {
    if ( 0 == m_log ) { m_log = new MsgStream( msgSvc() , name() ) ; }
    return *m_log;
  }
  /// dump the content of DataOnDemand service 
  StatusCode dump( MSG::Level level = MSG::INFO ) const ;
private:
  /// Incident service
  IIncidentSvc*     m_incSvc;
  /// Algorithm manager
  IAlgManager*      m_algMgr;
  /// Data provider reference
  IDataProviderSvc* m_dataSvc;
  /// Trap name
  std::string       m_trapType;
  /// Data service name 
  std::string       m_dataSvcName;
  /// Flag to allow for the creation of partial leaves
  bool              m_partialPath;
  /// Mapping to algorithms
  SimpleProperty<Setup> m_algMapping;
  /// Mapping to nodes
  SimpleProperty<Setup> m_nodeMapping;
  /// List of algorithms to handle incidents
  AlgMap            m_algs;
  /// Map of "empty" objects to be placed as intermediate nodes
  NodeMap           m_nodes;
  //
  typedef std::map<std::string,std::string> Map ;
  typedef SimplePropertyRef<Map>  MapRef   ;
  Map                m_algMap          ;
  Map                m_nodeMap         ;
  MapRef*            m_algMap_         ;
  MapRef*            m_nodeMap_        ;
  bool               m_updateRequired  ;
  std::string        m_prefix          ;
  mutable MsgStream* m_log             ;
  ChronoEntity       m_total           ;
  ulonglong          m_statAlg         ;  
  ulonglong          m_statNode        ;  
  ulonglong          m_stat            ;  
} ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GAUDISVC_DATAONDEMANDSVC_H
// ============================================================================

