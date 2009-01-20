//  ====================================================================
//  ReadAlg.cpp
//  --------------------------------------------------------------------
//
//  Package   : GaudiExamples/Example3
//
//  Author    : Markus Frank
//
//  ====================================================================

// Framework include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataStoreItem.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"

#include "GaudiKernel/IDataProviderSvc.h"

// Example related include files
#include "ReadAlgFSR.h"

// Event Model related classes
#include "Event.h"
#include "MyTrack.h"

#include "GaudiKernel/System.h"


using namespace Gaudi::Examples ;

DECLARE_ALGORITHM_FACTORY(ReadAlgFSR);

StatusCode ReadAlgFSR::initialize() {
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------
// Execute
//--------------------------------------------------------------------
StatusCode ReadAlgFSR::execute() {
  // This just makes the code below a bit easier to read (and type)
  MsgStream log(msgSvc(), name());
  SmartDataPtr<Event> evt(eventSvc(),"/Event/Header");
  SmartDataPtr<MyTrackVector> myTracks(eventSvc(), "/Event/MyTracks");
  if ( evt != 0 )    {
    int evt_num = evt->event();
    if ( evt_num <= 10 || evt_num%100==0 )  {
      log << MSG::INFO << "========= EVENT:" << evt->event() << " RUN:" << evt->run()
          << " TIME:" << evt->time();
      for (size_t nc=0; nc<evt->collisions().size(); ++nc) {
        if ( evt->collisions()[nc] )  {
          log << " " << evt->collisions()[nc]->collision();
          log << " (" << evt->collisions()[nc].hintID() << ")";
        }
        else  {
          log << " ===";
        }
      }
      log << endmsg;
    }
    SmartDataPtr<MyTrackVector> myTracks(eventSvc(), "/Event/MyTracks");
    SmartDataPtr<MyVertexVector> myVtx(eventSvc(), "/Event/Collision_0/MyVertices");
    if ( myTracks != 0 )    {
      int count = 0;
      for ( MyTrackVector::iterator i = myTracks->begin(); i != myTracks->end() && count++ < 5; i++ )   {
        try   {
          if ( evt->event() < 10 || evt->event()%500==0 )  {
            log << MSG::DEBUG << "Evt:";
            if ( (*i)->event() )  {
              log << (*i)->event()->event();
            }
            else  {
              log << "Unknown";
            }
            log << " Track:";
            log.width(12);          log << (*i)->px();
            log.width(12);          log << (*i)->py();
            log.width(12);          log << (*i)->pz();
            log << " Org:";
            if ( (*i)->originVertex() )  {
              log.width(4);           log << (*i)->originVertex()->index();
              log.width(10);          log << (*i)->originVertex()->x();
              log.width(10);          log << (*i)->originVertex()->y();
              log.width(10);          log << (*i)->originVertex()->z();
            }
            else {
              log << "No Origin Vertex!";
            }
            log << endmsg << "   Decays:";
            log.width(4);           log << (*i)->decayVertices().size();
            log << endreq;
            for (size_t id = 0; id < (*i)->decayVertices().size(); ++id) {
              const MyVertex* v = (*i)->decayVertices()[id];
              if ( v )  {
                log << "    ";
                log.width(10);        log << v->x();
                log.width(10);        log << v->y();
                log.width(10);        log << v->z() << " Collisions:";
                for (size_t ic = 0; ic < v->collisions().size(); ++ic) {
                  log.width(3);
                  if ( v->collisions()[ic] )  {
                    log << v->collisions()[ic]->collision();
                    log << " (" << v->collisions()[ic].hintID() << ")";
                  }
                  else {
                    log << "-";
                    log << " (" << v->collisions()[ic].hintID() << ")";
                  }
                }
              }
              else  {
                log << "No decay vertex!";
              }
              log << endmsg;
            }
          }
        }
        catch(...)    {
          log << MSG::ERROR << "Exception occurred!" << endreq;
        }
      }
    }
    else
      log << MSG::WARNING << "No tracks found, Event " << evt->event() << endmsg;
    return StatusCode::SUCCESS;
  }
  log << MSG::ERROR << "Unable to retrieve Event Header object" << endreq;
  return StatusCode::FAILURE;
}
