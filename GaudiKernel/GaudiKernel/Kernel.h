// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/Kernel.h,v 1.15 2006/05/02 13:03:03 hmd Exp $
#ifndef GAUDIKERNEL_KERNEL_H
#define GAUDIKERNEL_KERNEL_H

// Some pragmas to avoid warnings in VisualC
#ifdef _WIN32
  // Disable warning C4786: identifier was truncated to '255' characters in the debug information
  #pragma warning ( disable : 4786 )
  // Disable warning C4291: no matching operator delete found; memory will not be freed if initialization throws an exception
  #pragma warning ( disable : 4291 )
  // Disable warning C4250: inheritance via dominance
  #pragma warning ( disable : 4250 )
#endif


// Large integer definition depends of the platform
#ifdef _WIN32
  typedef __int64 longlong;
  #ifndef LONGLONG_MAX
    #define LONGLONG_MAX 0x7FFFFFFFFFFFFFFFLL
  #endif
  #ifndef LONGLONG_MIN
    #define LONGLONG_MIN 0x8000000000000000LL
  #endif

  typedef unsigned __int64 ulonglong;
  #ifndef ULONGLONG_MAX
    #define ULONGLONG_MAX 0xFFFFFFFFFFFFFFFFLL
  #endif
  #ifndef ULONGLONG_MIN
    #define ULONGLONG_MIN 0x0000000000000000LL
  #endif
#elif defined(__linux) || defined(__APPLE__)
  typedef long long int longlong;
  #ifndef LONGLONG_MAX
    #define LONGLONG_MAX 0x7FFFFFFFFFFFFFFFLL
  #endif
  #ifndef LONGLONG_MIN
    #define LONGLONG_MIN 0x8000000000000000LL
  #endif
  typedef unsigned long long int ulonglong;
  #ifndef ULONGLONG_MAX
    #define ULONGLONG_MAX 0xfFFFFFFFFFFFFFFFLL
  #endif
  #ifndef ULONGLONG_MIN
    #define ULONGLONG_MIN 0x0000000000000000LL
  #endif
#else
  // This will not really work !!
  struct __longlong { public: long __data[2]; };
  typedef __longlong longlong;
  typedef __longlong ulonglong;
  static const __longlong LONGLONG_MAX = {0x7FFFFFFF, 0xFFFFFFFF};
  static const __longlong LONGLONG_MIN = {0x80000000, 0x00000000};
  static const __ulonglong ULONGLONG_MAX = {0xFFFFFFFF, 0xFFFFFFFF};
  static const __ulonglong ULONGLONG_MIN = {0x00000000, 0x00000000};
#endif    // linux

#ifdef _WIN32
  #define TEMPLATE_SPECIALIZATION template <>
#elif defined(__linux) || defined(__APPLE__)
  #define TEMPLATE_SPECIALIZATION
#endif
  

#endif  // GAUDIKERNEL_KERNEL_H
