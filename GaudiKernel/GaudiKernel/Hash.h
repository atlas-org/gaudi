// $Id: Hash.h,v 1.6 2007/05/23 18:05:15 marcocle Exp $
#ifndef GAUDIKERNEL_HASH_H
#define GAUDIKERNEL_HASH_H 1

// Include files
#include <string>
#include <functional>

namespace GaudiUtils {
  
  /** @class Hash Hash.h GaudiKernel/Hash.h
  *
  * Simple hash function based on Stroustrup example.
  * It provides correct handling of pointers, std::string and char*.
  *
  * @author Marco Clemencic
  * @date 2005-10-07
  */
  
  template <class T>
  struct Hash: public std::unary_function<T,size_t> {
    inline size_t operator() ( const T& key ) const;
  
    // Needed to behave like VC++'s hash_compare
    enum { // parameters for hash table
      bucket_size = 4,    // 0 < bucket_size
      min_buckets = 8};   // min_buckets = 2 ^^ N, 0 < N
  
    inline bool operator() ( const T& key1, const T& key2 ) const { 
      // test if key1 ordered before key2
      return comp(key1,key2);
    }
  
  protected:
    std::less<T> comp;  ///< comparison operator needed for VC++
  };
  
  //--- Partial specializations 
  template <class T>
  struct Hash<T*>: public std::unary_function<T*,size_t>, private Hash<T> {
    inline size_t operator() ( const T* key ) const {
      return ( NULL == key ) ? (size_t)0 : Hash<T>::operator()(*key); 
    }
    inline bool operator() ( const T* key1, const T* key2 ) const {
      return Hash<T>::operator() (*key1,*key2);
    }
  };
  
  template<class T>
  struct Hash<const T*> : public Hash<T*>{};
  template<class T>
  struct Hash<T&>       : public Hash<T>{};
  template<class T>
  struct Hash<const T&> : public Hash<T>{};
  
  
  template <class T>
  size_t Hash<T>::operator() ( const T& key ) const {
    size_t res = 0 ;
    size_t len = sizeof(T) ;
    const char* p = reinterpret_cast<const char*>( &key );
    while( len-- ) { res = ( res << 1 ) ^ *p; ++p; }
    return res;
  }
  
  template <>
  inline size_t Hash<int>::operator() ( const int& key ) const { return (size_t)key; }

  template <>
  inline size_t Hash<unsigned int>::operator() ( const unsigned int& key ) const { return (size_t)key; }

  template <>
  inline size_t Hash<long>::operator() ( const long& key ) const { return (size_t)key; }

  template <>
  inline size_t Hash<unsigned long>::operator() ( const unsigned long& key ) const { return (size_t)key; }

  template <>
  inline size_t Hash<long long>::operator() ( const long long& key ) const { return (size_t)key; }

  template <>
  inline size_t Hash<unsigned long long>::operator() ( const unsigned long long& key ) const { return (size_t)key; }   

	// void* is a special case and needs a full specialization
  template <>
  struct Hash<void *>: public std::unary_function<void*,size_t> {
    inline size_t operator() ( const void* key ) const {
    	return reinterpret_cast<size_t>(key);
    }
    inline bool operator() ( const void* key1, const void* key2 ) const { 
      return reinterpret_cast<size_t>(key1) < reinterpret_cast<size_t>(key2);
    }
  };

  template <>
  inline size_t Hash<std::string>::operator() ( const std::string& key ) const {
    size_t res = 0 ;
    std::string::const_iterator e = key.end();
    std::string::const_iterator p = key.begin();
    while( p != e ) { res = ( res << 1 ) ^ *p; ++p; }
    return res;
  }
  
  template <>
  inline size_t Hash<char *>::operator() ( const char *key ) const {
    size_t res = 0 ;
    if ( 0 == key ) { return 0; }
    while ( *key ) { res = ( res << 1 ) ^ *key; ++key; }
    return res;
  }
} // namespace GaudiUtils

#endif // GAUDIKERNEL_HASH_H
