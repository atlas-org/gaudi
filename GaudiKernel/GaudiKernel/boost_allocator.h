// $Id: boost_allocator.h,v 1.1 2006/05/02 16:49:02 hmd Exp $
#ifndef GAUDIKERNEL_BOOST_ALLOCATOR_H 
#define GAUDIKERNEL_BOOST_ALLOCATOR_H 1

/// Include file needed to use Boost singleton pool.

#ifdef WIN32
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#endif
#include <boost/pool/singleton_pool.hpp>

#endif // GAUDIKERNEL_BOOST_ALLOCATOR_H
