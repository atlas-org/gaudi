// $Id: PartitionSwitchTool.cpp,v 1.2 2006/01/10 20:09:25 hmd Exp $

// Framework include files
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Tokenizer.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartitionControl.h"

/** @class PartitionSwitchTool PartitionSwitchTool.h
 *
 *  @author Markus Frank
 *  @date   2004-06-24
 */
class PartitionSwitchTool 
  : public AlgTool, virtual public IPartitionControl 
{

  typedef const std::string& CSTR;
  typedef StatusCode         STATUS;

private:

  /// Job option to set the multi-service name
  std::string                m_actorName;
  /// reference to Partition Controller
  SmartIF<IPartitionControl> m_actor;

public:

  /// Standard constructor
  PartitionSwitchTool(CSTR typ, CSTR nam, const IInterface* parent)
  : AlgTool(typ, nam , parent)
  {
    // Declaring implemented interfaces
    declareInterface<IPartitionControl>(this);
    declareProperty("Actor", m_actorName = "EventDataService");
  }
  /// Standard destructor
  virtual ~PartitionSwitchTool()   {
    m_actor = 0;
  }

  /// IInterface overload: Query implemented interface(s)
  virtual STATUS queryInterface(const InterfaceID& riid, void** ppvUnknown) {
    if ( IPartitionControl::interfaceID().versionMatch(riid) )
      *ppvUnknown = (IPartitionControl*)this;
    else 
      return AlgTool::queryInterface(riid, ppvUnknown);
    addRef();
    return SUCCESS;
  }

  /// Initialize
  virtual STATUS initialize()  {
    /// Access partitioned multi-service
    STATUS sc = AlgTool::initialize();
    MsgStream log(msgSvc(), name());
    if ( !sc.isSuccess() )  {
      log << MSG::ERROR << "Cannot initialize base class!" << endmsg;
      return sc;
    }
    m_actor = 0;
    sc = service(m_actorName, m_actor.pRef());
    if ( !sc.isSuccess() )  {
      log << MSG::ERROR << "Cannot retrieve partition controller \""
          << m_actorName << "\"!" << endmsg;
      return sc;
    }
    return sc;
  }
  /// Finalize
  virtual STATUS finalize()   {
    m_actor = 0;
    return AlgTool::finalize();
  }

  void _check(STATUS sc, CSTR msg)  const {
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << msg << " Status=" << sc.getCode() << endmsg;
  }
#define CHECK(x,y) if ( !x.isSuccess() ) _check(x, y); return x;
    
  /// Create a partition object. The name identifies the partition uniquely
  virtual STATUS create(CSTR nam, CSTR typ)  {
    STATUS sc = m_actor ? m_actor->create(nam,typ) : NO_INTERFACE;
    CHECK(sc, "Cannot create partition: "+nam+" of type "+typ);
  }
  /// Create a partition object. The name identifies the partition uniquely
  virtual STATUS create(CSTR nam, CSTR typ, IInterface*& pPartition)  {
    STATUS sc = m_actor ? m_actor->create(nam,typ,pPartition) : NO_INTERFACE;
    CHECK(sc, "Cannot create partition: "+nam+" of type "+typ);
  }
  /// Drop a partition object. The name identifies the partition uniquely
  virtual STATUS drop(CSTR nam)  {
    STATUS sc = m_actor ? m_actor->drop(nam) : NO_INTERFACE;
    CHECK(sc, "Cannot drop partition: "+nam);
  }
  /// Drop a partition object. The name identifies the partition uniquely
  virtual STATUS drop(IInterface* pPartition)  {
    STATUS sc = m_actor ? m_actor->drop(pPartition) : NO_INTERFACE;
    CHECK(sc, "Cannot drop partition by Interface.");
  }
  /// Activate a partition object. The name identifies the partition uniquely.
  virtual STATUS activate(CSTR nam)  {
    STATUS sc = m_actor ? m_actor->activate(nam) : NO_INTERFACE;
    CHECK(sc, "Cannot activate partition: "+nam);
  }
  /// Activate a partition object.
  virtual STATUS activate(IInterface* pPartition)  {
    STATUS sc = m_actor ? m_actor->activate(pPartition) : NO_INTERFACE;
    CHECK(sc, "Cannot activate partition by Interface.");
  }
  /// Access a partition object. The name identifies the partition uniquely.
  virtual STATUS get(CSTR nam, IInterface*& pPartition) const  {
    STATUS sc = m_actor ? m_actor->get(nam, pPartition) : NO_INTERFACE;
    CHECK(sc, "Cannot get partition "+nam);
  }
  /// Access the active partition object.
  virtual STATUS activePartition(std::string& nam, IInterface*& pPartition) const  {
    STATUS sc = m_actor ? m_actor->activePartition(nam, pPartition) : NO_INTERFACE;
    CHECK(sc, "Cannot determine active partition.");
  }
};

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(PartitionSwitchTool)
