// $Id: ISequencerTimerTool.h,v 1.4 2005/07/29 16:49:43 hmd Exp $
#ifndef ISEQUENCERTIMERTOOL_H 
#define ISEQUENCERTIMERTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ISequencerTimerTool( "ISequencerTimerTool", 1, 0 );

/** @class ISequencerTimerTool ISequencerTimerTool.h
 *  Implements the time measurement inside a sequencer
 *
 *  @author Olivier Callot
 *  @date   2004-05-19
 */
 
class ISequencerTimerTool : public virtual IAlgTool {
public: 
   // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ISequencerTimerTool; }

  /** add a timer entry with the specified name **/
  virtual int addTimer( std::string name ) = 0 ;

  /** Increase the indentation of the name **/
  virtual void increaseIndent() = 0 ;

  /** Decrease the indentation fo the name **/
  virtual void decreaseIndent() = 0;

  /** start the counter, i.e. register the current time **/
  virtual void start( int index ) = 0;

  /** stop the counter, return the elapsed time **/
  virtual double stop( int index ) = 0;

  /** returns the name of the counter **/
  virtual std::string name( int index ) = 0;

  /** returns the last measured time time **/
  virtual double lastTime( int index ) = 0;

  /** returns the index of the counter with that name, or -1 **/
  virtual int indexByName( std::string name ) = 0;

  /** returns the flag telling that global timing is wanted **/
  virtual bool globalTiming() = 0;
};
#endif // ISEQUENCERTIMERTOOL_H
