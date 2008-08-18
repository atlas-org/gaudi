// $Header: /tmp/svngaudi/tmp.jEpFh25751/Gaudi/GaudiKernel/GaudiKernel/AudFactory.h,v 1.8 2007/05/25 16:39:16 marcocle Exp $
#ifndef GAUDIKERNEL_AUDFACTORY_H
#define GAUDIKERNEL_AUDFACTORY_H

#include "Reflex/PluginService.h"

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

#define DECLARE_AUDITOR(x)  /*dummy*/
#define DECLARE_NAMESPACE_AUDITOR(n,x)  /*dummy*/
#define DECLARE_AUDITOR_FACTORY(x) PLUGINSVC_FACTORY(x,IAuditor*(std::string, ISvcLocator*))
#define DECLARE_NAMESPACE_AUDITOR_FACTORY(n,x) using n::x; PLUGINSVC_FACTORY(x,IAuditor*(std::string, ISvcLocator*))

#endif // GAUDIKERNEL_AUDFACTORY_H
