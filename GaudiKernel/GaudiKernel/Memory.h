// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/Memory.h,v 1.2 2006/12/10 20:29:17 leggett Exp $
//====================================================================
//	Memory.h
//--------------------------------------------------------------------
//
//	Package    : System (The LHCb System service)
//
//  Description: Information of memory usage from a given process
//
//	Author     : M.Frank
//  Created    : 13/11/00
//	Changes    : 
//
//====================================================================
#ifndef GAUDIKERNEL_MEMORY_H
#define GAUDIKERNEL_MEMORY_H

// Framework include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SystemBase.h"


/** Note: OS specific details for memory usage

    Entrypoints:
    - basePriority            Base priority
    - procID                  Process ID
    - parentID                Parent's process ID
    - affinityMask            Affinity mask
    - exitStatus              Exit status
    - priorityBoost           Priority boost
    - numPageFault            Number of page faults
    - pagefileUsage           Current page file usage
    - pagefileUsagePeak       Peak usage of page file
    - nonPagedMemory          Current usage of non paged memory
    - nonPagedMemoryPeak      Peak usage of non paged memory
    - nonPagedMemoryLimit     Maximum amount of non-paged memory this process is allowed to use
    - pagedMemory             Amount of paged memory currently occupied 
    - pagedMemoryPeak         Maximum of paged memory occupied by the process
    - pagedMemoryLimit        paged memory that can be occupied by the process
    - memorySize              Size of currently occupied memory
    - memorySizePeak          Maximum memory occupied by the process
    - memoryLimit             Amount of memory that can be occupied by the process
    - virtualMemorySize       Virtual memory size currenly occupied
    - peakVirtualMemorySize   Maximum amount of virtual memory occupied
    - virtualMemoryLimit      Amount of virtual memory that can be occupied by the process
    - minMemoryLimit          Minimum amount of virtual memory this process may use
    - maxMemoryLimit          Maximum amount of virtual memory this process is allowed to use
    - mappedMemorySize        Currenly mapped virtual memory size
    - mappedMemorySizePeak    Peak mapped virtual memory size

    @author:  M.Frank
    @version: 1.0
*/
namespace System    {
  /// Unit of memory
  enum MemoryUnit { Byte, kByte, MByte, GByte, TByte, PByte, EByte };
  /// Convert time from kByte to requested representation (Experts only)
  long adjustMemory(MemoryUnit typ, long value);
  /// Basic Process Information: Process ID
  long procID();
  /// Basic Process Information: Parent's process ID
  long parentID(InfoType fetch = ProcessBasics, long pid = -1);
  /// Basic Process Information: Affinity mask
  long affinityMask(InfoType fetch = ProcessBasics, long pid = -1);
  /// Basic Process Information: Exit status (does not really make sense for the running process, but for others!)
  long exitStatus(InfoType fetch = ProcessBasics, long pid = -1);
  /// Basic Process Information: Base priority
  long basePriority(InfoType fetch = ProcessBasics, long pid = -1);
  /// Basic Process Information: priority boost
  long priorityBoost(InfoType fetch = PriorityBoost, long pid = -1);

  /// Basic Process Information: Number of page faults
  long numPageFault(InfoType fetch = Memory, long pid = -1);
  /// Basic Process Information: Current page file usage
  long pagefileUsage(MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1);
  /// Basic Process Information: Peak usage of page file
  long pagefileUsagePeak(MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1);
  /// Basic Process Information: Peak usage of page file
  long pagefileUsageLimit(MemoryUnit unit = kByte, InfoType fetch = Quota, long pid = -1);

  /// Basic Process Information: Current usage of non paged memory
  long nonPagedMemory( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// Basic Process Information: Peak usage of non paged memory
  long nonPagedMemoryPeak( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// System Process Limits: Maximum amount of non-paged memory this process is allowed to use
  long nonPagedMemoryLimit( MemoryUnit unit = kByte, InfoType fetch = Quota, long pid = -1 );

  /// Basic Process Information: Amount of paged memory currently occupied by the process 'pid'
  long pagedMemory( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// Basic Process Information: Maximum of paged memory occupied by the process 'pid'
  long pagedMemoryPeak( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// Basic Process Information: Amount of paged memory that can be occupied by the process 'pid'
  long pagedMemoryLimit( MemoryUnit unit = kByte, InfoType fetch = Quota, long pid = -1 );

  /// System Process Limits: Minimum amount of virtual memory this process may use
  long minMemoryLimit( MemoryUnit unit = kByte, InfoType fetch = Quota, long pid = -1 );
  /// System Process Limits: Maximum amount of virtual memory this process is allowed to use
  long maxMemoryLimit( MemoryUnit unit = kByte, InfoType fetch = Quota, long pid = -1 );
  /// Basic Process Information: priority boost
  long mappedMemory( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// Basic Process Information: priority boost
  long mappedMemoryPeak( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// Basic Process Information: priority boost
  long virtualMemory( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// Basic Process Information: priority boost
  long virtualMemoryPeak( MemoryUnit unit = kByte, InfoType fetch = Memory, long pid = -1 );
  /// System Process Limits: Maximum amount of the page file this process is allowed to use
  long virtualMemoryLimit( MemoryUnit unit = kByte, InfoType fetch = Quota, long pid = -1 );
}
#endif //GAUDIKERNEL_MEMORY_H
