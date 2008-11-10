// $Id: GaudiSequencer.cpp,v 1.18 2008/10/10 13:50:35 marcocle Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiAlg/GaudiSequencer.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/IJobOptionsSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GaudiSequencer
//
// 2004-05-13 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GaudiSequencer::GaudiSequencer( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_timerTool( 0 )
{
  declareProperty( "Members"             , m_names                  );
  declareProperty( "ModeOR"              , m_modeOR         = false );
  declareProperty( "IgnoreFilterPassed"  , m_ignoreFilter   = false );
  declareProperty( "MeasureTime"         , m_measureTime    = false );
  declareProperty( "ReturnOK"            , m_returnOK       = false );
  declareProperty( "ShortCircuit"        , m_shortCircuit   = true  );

  m_names.declareUpdateHandler (& GaudiSequencer::membershipHandler, this );
}
//=============================================================================
// Destructor
//=============================================================================
GaudiSequencer::~GaudiSequencer() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GaudiSequencer::initialize() {
  GaudiAlgorithm::initialize();

  debug() << "==> Initialise" << endreq;

  StatusCode status = decodeNames();
  if ( !status.isSuccess() ) return status;

  m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
  if ( m_timerTool->globalTiming() ) m_measureTime = true;

  if ( m_measureTime ) {
    m_timer = m_timerTool->addTimer( name() );
    m_timerTool->increaseIndent();
  } else {
    release( m_timerTool );
    m_timerTool = 0;
  }

  //== Initialize the algorithms
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    if ( m_measureTime ) {
      itE->setTimer( m_timerTool->addTimer( itE->algorithm()->name() ) );
    }

    status = itE->algorithm()->sysInitialize();
    if ( !status.isSuccess() ) {
      return Error( "Can not initialize " + itE->algorithm()->name(),
                    status );
    }
  }
  if ( m_measureTime ) m_timerTool->decreaseIndent();

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GaudiSequencer::execute() {

  if ( m_measureTime ) m_timerTool->start( m_timer );

  debug() << "==> Execute" << endreq;

  StatusCode result = StatusCode::SUCCESS;

  bool seqPass = !m_modeOR; //  for OR, result will be false, unless (at least) one is true
                            //  for AND, result will be true, unless (at least) one is false
                            //    also see comment below ....

  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; ++itE ) {
    Algorithm* myAlg = itE->algorithm();
    if ( ! myAlg->isEnabled() ) continue;
      if ( ! myAlg->isExecuted() ) {
      if ( m_measureTime ) m_timerTool->start( itE->timer() );
        result = myAlg->sysExecute();
      if ( m_measureTime ) m_timerTool->stop( itE->timer() );
        myAlg->setExecuted( true );
        if ( ! result.isSuccess() ) break;  //== Abort and return bad status

      }
      //== Check the returned status
      if ( !m_ignoreFilter ) {
        bool passed = myAlg->filterPassed();
      if ( itE->reverse() ) passed = !passed;

        //== indicate our own result. For OR, exit as soon as true.
        //        If no more, will exit with false.
        //== for AND, exit as soon as false. Else, will be true (default)

      // if not short-circuiting, make sure we latch iPass to 'true' in 
      // OR mode (i.e. it is sufficient for one item to be true in order
      // to be true at the end, and thus we start out at 'false'), and latch 
      // to 'false' in AND mode (i.e. it is sufficient for one item to 
      // be false to the false in the end, and thus we start out at 'true')
      // -- i.e. we should not just blindly return the 'last' passed status!

      // or to put it another way: in OR mode, we don't care about things
      // which are false, as they leave our current state alone (provided
      // we stared as 'false'!), and in AND mode, we keep our current 
      // state until someone returns 'false' (provided we started as 'true')
      if ( m_modeOR ? passed : !passed ) {
        seqPass = passed;
        if (m_shortCircuit) break;
      }
    }
    
  }
  if ( !m_ignoreFilter && !m_entries.empty() ) setFilterPassed( seqPass );
  setExecuted( true );

  if ( m_measureTime ) m_timerTool->stop( m_timer );

  return m_returnOK ? StatusCode::SUCCESS : result;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GaudiSequencer::finalize() {

  debug() << "==> Finalize" << endreq;
  return  GaudiAlgorithm::finalize();
}

//=========================================================================
//  Execute the beginRun of every algorithm
//=========================================================================
StatusCode GaudiSequencer::beginRun ( ) {

  if ( !isEnabled() ) return StatusCode::SUCCESS;

  debug() << "==> beginRun" << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Execute the endRun() of every algorithm
//=========================================================================
StatusCode GaudiSequencer::endRun ( ) {

  if ( !isEnabled() ) return StatusCode::SUCCESS;

  debug() << "==> endRun" << endreq;
  return StatusCode::SUCCESS;
}

//=========================================================================
// reset the executed status of all members
//=========================================================================
void GaudiSequencer::resetExecuted ( ) {
  Algorithm::resetExecuted();
  // algorithm doesn't call resetExecuted of subalgos! should it???
  std::vector<AlgorithmEntry>::const_iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; ++itE ) {
    itE->algorithm()->resetExecuted();
  }
}
//=========================================================================
//  Decode the input names and fills the m_algs vector.
//=========================================================================
StatusCode GaudiSequencer::decodeNames( )  {

  StatusCode final = StatusCode::SUCCESS;
  m_entries.clear();

  //== Get the "Context" option if in the file...
  IJobOptionsSvc* jos = svc<IJobOptionsSvc>( "JobOptionsSvc" );
  bool addedContext = false;  //= Have we added the context ?
  bool addedRootInTES = false;  //= Have we added the rootInTES ?
  bool addedGlobalTimeOffset = false;  //= Have we added the globalTimeOffset ?


  //= Get the Application manager, to see if algorithm exist
  IAlgManager* appMgr = svc<IAlgManager>("ApplicationMgr");
  const std::vector<std::string>& nameVector = m_names.value();
  std::vector<std::string>::const_iterator it;
  for ( it = nameVector.begin(); nameVector.end() != it; it++ ) {
    std::string theName( *it );
    std::string theType( *it );
    std::string::size_type slash = it->find_first_of( '/' );
    if ( slash != std::string::npos ) {
      theType = it->substr( 0, slash );
      theName = it->substr( slash+1 );
    }
    //== handling of extensions to the name ???

    //== Check wether the specified algorithm already exists. If not, create it

    IAlgorithm* myIAlg;
    StatusCode result = appMgr->getAlgorithm( theName, myIAlg );
    if ( !result.isSuccess() ) {
      //== Set the Context if not in the jobOptions list
      if ( ""  != context() ||
           ""  != rootInTES() ||
           0.0 != globalTimeOffset() ) {
        bool foundContext = false;
        bool foundRootInTES = false;
        bool foundGlobalTimeOffset = false;
        const std::vector<const Property*>* properties = jos->getProperties( theName );
        if ( 0 != properties ) {
          // Iterate over the list to set the options
          for ( std::vector<const Property*>::const_iterator itProp = properties->begin();
               itProp != properties->end();
               itProp++ )   {
            const StringProperty* sp = dynamic_cast<const StringProperty*>(*itProp);
            if ( 0 != sp )    {
              if ( "Context" == (*itProp)->name() ) {
                foundContext = true;
              }
              if ( "RootInTES" == (*itProp)->name() ) {
                foundRootInTES = true;
              }
              if ( "GlobalTimeOffset" == (*itProp)->name() ) {
                foundGlobalTimeOffset = true;
              }
            }
          }
        }
        if ( !foundContext && "" != context() ) {
          StringProperty contextProperty( "Context", context() );
          jos->addPropertyToCatalogue( theName, contextProperty ).ignore();
          addedContext = true;
        }
        if ( !foundRootInTES && "" != rootInTES() ) {
          StringProperty rootInTESProperty( "RootInTES", rootInTES() );
          jos->addPropertyToCatalogue( theName, rootInTESProperty ).ignore();
          addedRootInTES = true;
        }
        if ( !foundGlobalTimeOffset && 0.0 != globalTimeOffset() ) {
          DoubleProperty globalTimeOffsetProperty( "GlobalTimeOffset", globalTimeOffset() );
          jos->addPropertyToCatalogue( theName, globalTimeOffsetProperty ).ignore();
          addedGlobalTimeOffset = true;
        }
      }

      Algorithm *myAlg = 0;
      result = createSubAlgorithm( theType, theName, myAlg );
      // (MCl) this should prevent bug #35199... even if I didn't manage to
      // reproduce it with a simple test.
      if (result.isSuccess())
        myIAlg = myAlg;
    }

    //== Remove the property, in case this is not a GaudiAlgorithm...
    if ( addedContext ) {
      jos->removePropertyFromCatalogue( theName, "Context" ).ignore();
      addedContext = false;
    }
    if ( addedRootInTES ) {
      jos->removePropertyFromCatalogue( theName, "RootInTES" ).ignore();
      addedRootInTES = false;
    }
    if ( addedGlobalTimeOffset ) {
      jos->removePropertyFromCatalogue( theName, "GlobalTimeOffset" ).ignore();
      addedGlobalTimeOffset = false;
    }

    //== Is it an Algorithm ?  Strange test...
    if ( result.isSuccess() ) {
      // TODO: (MCl) it is possible to avoid the dynamic_cast in most of the
      //             cases by keeping the result of createSubAlgorithm. 
      Algorithm*  myAlg = dynamic_cast<Algorithm*>( myIAlg );
      if (myAlg!=0) {
        m_entries.push_back( AlgorithmEntry( myAlg ) );
        myAlg->addRef();                  //== Indicate it is used.
        debug () << "Added algorithm " << theName << endreq;
      } else {
        warning() << theName << " is not an Algorithm - failed dynamic_cast"
                  << endreq;
        final = StatusCode::FAILURE;
      }
    } else {
      warning() << "Unable to find or create " << theName << endreq;
      final = result;
    }

  }
  release(appMgr).ignore();
  release(jos).ignore();
  
  //== Print the list of algorithms
  MsgStream& msg = info();
  if ( m_modeOR ) msg << "OR ";
  msg << "Member list: ";
  std::vector<AlgorithmEntry>::iterator itE;
  for ( itE = m_entries.begin(); m_entries.end() != itE; itE++ ) {
    Algorithm* myAlg = (*itE).algorithm();
    std::string myAlgType = System::typeinfoName( typeid( *myAlg) ) ;
    if ( myAlg->name() == myAlgType ) {
      msg << myAlg->name();
    } else {
      msg << myAlgType << "/" << myAlg->name();
    }
    if ( itE+1 != m_entries.end() ) msg << ", ";
  }
  if ( "" != context() ) msg << ", with context '" << context() << "'";
  if ( "" != rootInTES() ) msg << ", with rootInTES '" << rootInTES() << "'";
  if ( 0.0 != globalTimeOffset() ) msg << ", with globalTimeOffset " << globalTimeOffset();
  msg << endreq;

  return final;

}

//=========================================================================
//  Interface for the Property manager
//=========================================================================
void GaudiSequencer::membershipHandler ( Property& ) {
  if ( isInitialized() ) decodeNames().ignore();
}
//=============================================================================
