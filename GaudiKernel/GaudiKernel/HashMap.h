// $Id: HashMap.h,v 1.5 2007/05/23 17:13:30 marcocle Exp $
#ifndef GAUDIKERNEL_HASHMAP_H
#define GAUDIKERNEL_HASHMAP_H 1

// Include files
#include "Map.h"
#include "Hash.h"

#ifdef __GNUC__
#include <ext/hash_map>
#elif defined(_WIN32) || defined(__ICC) || defined(__ECC)
#include <hash_map>
#endif

namespace GaudiUtils {
  
  /** @class HashMap HashMap.h GaudiKernel/HashMap.h
   *
   * Common class providing an architecture-independent hash map.
   *
   * @author Marco Clemencic
   * @date 2005-10-06
   */
  
  template <typename K, typename T,
            typename H = Hash<K>,
  
  #ifdef __GNUC__
            typename M = __gnu_cxx::hash_map<K,T,H>
  #elif defined(_WIN32)
// Marco Cl.: (work-around) it seems that Windows implementation of hash map is extremely slow
// we use a normal map instead.
//            typename M = stdext::hash_map<K,T,H>
            typename M = std::map<K,T>
  #elif defined(__ICC) || defined(__ECC)
            typename M = std::hash_map<K,T,H>
  #endif
            >
  class HashMap: public Map<K,T,M> {
  
  public:
    typedef H hasher;

    inline hasher hash_funct() const {
// Marco Cl.: since on windows we are using a std::map, we have to provide a different implementation
// for GaudiUtils::HashMap::hash_funct(). (std::map::hash_funct does not exist)
#ifdef _WIN32
      return hasher();
#else
      return this->m_map.hash_funct();
#endif
    }
  };
} // namespace GaudiUtils

#endif // GAUDIKERNEL_GAUDIHASHMAP_H
