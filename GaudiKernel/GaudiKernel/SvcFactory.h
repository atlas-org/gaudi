// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/SvcFactory.h,v 1.10 2007/01/23 09:13:07 mato Exp $
#ifndef KERNEL_SVCFACTORY_H
#define KERNEL_SVCFACTORY_H

#include "Reflex/PluginService.h"

// The following is needed to be backward compatible with the old factories of Gaudi. Otherwise the components
// having the constructor/destructor protected will not be working

class IService;
class ISvcLocator;

template <typename T> class SvcFactory {
	public: static IService* create(const std::string& name, ISvcLocator *svcloc) { 
    return new T(name, svcloc ); 
  }
};

namespace {
  template < typename P > class Factory<P, IService*(std::string, ISvcLocator*)> {
    public: static void* Func( void*, const std::vector<void*>& arg, void*) {
      return SvcFactory<P>::create(*(std::string*)(arg[0]), (ISvcLocator*)(arg[1]));
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

#define DECLARE_SERVICE_FACTORY(x)              PLUGINSVC_FACTORY(x,IService*(std::string, ISvcLocator*))
#define DECLARE_NAMESPACE_SERVICE_FACTORY(n,x)  using n::x; PLUGINSVC_FACTORY(x,IService*(std::string, ISvcLocator*))
#define DECLARE_SERVICE(x)  /*dummy*/
#define DECLARE_NAMESPACE_SERVICE(n,x) /*dummy*/


#endif // GAUDIKERNEL_SVCFACTORY_H
