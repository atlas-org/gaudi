#ifndef GAUDIKERNEL_SLEEP_H_
#define GAUDIKERNEL_SLEEP_H_

namespace Gaudi {

/// Simple sleep function.
/// @author Marco Clemencic
void Sleep(int sec);

/// Small variation on the sleep function for nanoseconds sleep.
/// @author Marco Clemencic
void NanoSleep(long long nsec);

}

#endif /*GAUDIKERNEL_SLEEP_H_*/
