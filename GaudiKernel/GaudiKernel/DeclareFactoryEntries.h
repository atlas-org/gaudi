#ifndef GAUDIKERNEL_DECLAREFACTORYENTRIES_H
#define GAUDIKERNEL_DECLAREFACTORYENTRIES_H 1

#ifdef GAUDI_NEW_PLUGIN_SERVICE
#warning "obsolete empty header, please remove it"
#else

#define DECLARE_ALGORITHM(x)  /*dummy*/
#define DECLARE_NAMESPACE_ALGORITHM(n,x) /*dummy*/

#define DECLARE_AUDITOR(x)  /*dummy*/
#define DECLARE_NAMESPACE_AUDITOR(n,x)  /*dummy*/

#define DECLARE_CONVERTER(x) /*dummy*/
#define DECLARE_NAMESPACE_CONVERTER(n,x)   /*dummy */

#define DECLARE_OBJECT(x)       /* dummy */
#define DECLARE_DATAOBJECT(x)   /* dummy */
#define DECLARE_NAMESPACE_OBJECT(n,x)      /* dummy */
#define DECLARE_NAMESPACE_DATAOBJECT(n,x)  /* dummy */

#define DECLARE_SERVICE(x)  /*dummy*/
#define DECLARE_NAMESPACE_SERVICE(n,x) /*dummy*/

#define DECLARE_ALGTOOL(x)             /*dummy*/
#define DECLARE_NAMESPACE_ALGTOOL(n,x) /*dummy*/
#define DECLARE_TOOL(x)                /*dummy*/
#define DECLARE_NAMESPACE_TOOL(n,x)    /*dummy*/


#define DECLARE_FACTORY_ENTRIES(x) \
void x##_load(); \
\
extern "C" void x##_loadRef() { \
  x##_load();  \
} \
\
void x##_load()

#endif

#endif // GAUDIKERNEL_DECLAREFACTORYENTRIES_H
