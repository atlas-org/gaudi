// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/AlgFactory.h,v 1.10 2008/10/09 21:00:37 marcocle Exp $
#ifndef GAUDIKERNEL_ALGFACTORY_H
#define GAUDIKERNEL_ALGFACTORY_H

#include "Reflex/PluginService.h"

// The following is needed to be backward compatible with the old factories of Gaudi. Otherwise the components
// having the constructor/destructor protected will not be working

class IAlgorithm;
class ISvcLocator;

template <typename T> class AlgFactory {
	public: static IAlgorithm* create(const std::string& name, ISvcLocator *svcloc) { 
    return new T(name, svcloc ); 
  }
};

namespace {
  template < typename P > class Factory<P, IAlgorithm*(std::string, ISvcLocator*)> {
    public: static void* Func( void*, const std::vector<void*>& arg, void*) {
      return AlgFactory<P>::create(*(std::string*)(arg[0]), (ISvcLocator*)(arg[1]));
    }
  };
}
//---Specialization needed to overcome a problem with Reflex.....
#ifndef TYPEDISTILLER_STRING_SPECIALIZATION
#define TYPEDISTILLER_STRING_SPECIALIZATION
namespace ROOT {
   namespace Reflex {
      template<> class TypeDistiller<std::string> {
      public:
         static Type Get() {
            return TypeBuilder("std::basic_string<char>");
         }
      };
   }
}
#endif

// Macros to declare component factories
#define DECLARE_ALGORITHM_FACTORY(x)          PLUGINSVC_FACTORY(x,IAlgorithm*(std::string, ISvcLocator*))
#define DECLARE_NAMED_ALGORITHM_FACTORY(x,n)  PLUGINSVC_FACTORY_WITH_ID(x,std::string(#n), IAlgorithm*(std::string, ISvcLocator*))
  
#define DECLARE_NAMESPACE_ALGORITHM_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY(x,IAlgorithm*(std::string, ISvcLocator*))
#define DECLARE_ALGORITHM(x)  /*dummy*/
#define DECLARE_NAMESPACE_ALGORITHM(n,x) /*dummy*/

#endif // GAUDIKERNEL_ALGFACTORY_H
