// $Id: GaudiCommonImp.h,v 1.11 2008/10/10 08:06:33 marcocle Exp $
// ============================================================================
#ifndef GAUDIALG_GAUDICOMMONIMP_H
#define GAUDIALG_GAUDICOMMONIMP_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GetData.h"
#include "GaudiAlg/GaudiCommon.h"
// ============================================================================
/** @file
 *  The implementation of inline/templated methods for class GaudiCommon
 *  @see    GaudiCommon
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-01-19
 */
// ============================================================================
// Returns the full correct event location given the rootInTes settings
// ============================================================================
template < class PBASE >
inline const std::string
GaudiCommon<PBASE>::fullTESLocation( const std::string & location,
                                     const bool useRootInTES ) const
{
  // The logic is:
  // if no R.I.T., give back location
  // if R.I.T., this is the mapping:
  // (note that R.I.T. contains a trailing '/')
  //  location       -> result
  //  -------------------------------------------------
  //  ""             -> R.I.T.[:-1]      ("rit")
  //  "/Event"       -> R.I.T.[:-1]      ("rit")
  //  "/Event/MyObj" -> R.I.T. + "MyObj" ("rit/MyObj")
  //  "MyObj"        -> R.I.T. + "MyObj" ("rit/MyObj")
  return ( !useRootInTES || rootInTES().empty() ?
           location
         :
           location.empty() || ( location == "/Event" ) ?
             rootInTES().substr(0,rootInTES().size()-1)
           :
             0 == location.find("/Event/") ?
               rootInTES() + location.substr(7)
             : 
               rootInTES() + location );
}
// ============================================================================
// Templated access to the data in Gaudi Transient Store
// ============================================================================
template < class PBASE >
template < class TYPE  >
inline typename Gaudi::Utils::GetData<TYPE>::return_type
GaudiCommon<PBASE>::get( IDataProviderSvc*  service ,
                         const std::string& location,
                         const bool useRootInTES ) const
{
  // check the environment
  Assert( 0 != service ,    "get():: IDataProvider* points to NULL!"      ) ;
  // get the helper object:
  Gaudi::Utils::GetData<TYPE> helper ;
  return helper ( *this , service , 
                  fullTESLocation ( location , useRootInTES ) ) ;
}
// ============================================================================
// check the existence of objects in Gaudi Transient Store
// ============================================================================
template < class PBASE >
template < class TYPE  >
inline bool GaudiCommon<PBASE>::exist( IDataProviderSvc*  service  ,
                                       const std::string& location ,
                                       const bool useRootInTES ) const
{
  // check the environment
  Assert( 0 != service , "exist():: IDataProvider* points to NULL!"      ) ;
  // check the data object
  const std::string & fullLoc = fullTESLocation( location, useRootInTES );
  SmartDataPtr<TYPE> obj( service , fullLoc ) ;
  return obj ? true : false ;
}
// ============================================================================

// ============================================================================
// get the existing object from Gaudi Event Transient store
// or create new object register in in TES and return if object
// does not exist
// ============================================================================
template <class PBASE>
template <class TYPE,class TYPE2>
inline TYPE* GaudiCommon<PBASE>::getOrCreate( IDataProviderSvc* svc ,
                                              const std::string& location ,
                                              const bool useRootInTES  ) const
{
  // check the environment
  Assert( 0 != svc , "getOrCreate():: svc points to NULL!" ) ;
  const std::string & fullLoc = fullTESLocation( location, useRootInTES );
  SmartDataPtr<TYPE> obj ( svc , fullLoc ) ;
  if ( !obj )
  {
    TYPE2* o = new TYPE2() ;
    put ( svc , o , location ) ; // do not use fullLoc here as put recreates it
    return o ;
  }
  TYPE* aux = obj ;
  if( !aux )
  { Exception ( "getOrCreate():: No valid data at '" + fullLoc + "'" ) ; }
  // return located *VALID* data
  return aux ;
}
// ============================================================================

// ============================================================================
// the useful method for location of tools.
// ============================================================================
template < class PBASE >
template < class TOOL  >
inline TOOL* GaudiCommon<PBASE>::tool( const std::string& type           ,
                                       const std::string& name           ,
                                       const IInterface*  parent         ,
                                       bool               create         ) const
{
  TOOL* Tool = 0 ;
  // for empty names delegate to another method
  if ( name.empty() )
  {
    Tool = tool<TOOL>( type , parent , create ) ;
  }
  else
  {
    Assert( 0 != this->toolSvc() , "tool():: IToolSvc* points to NULL!" ) ;
    // get the tool from Tool Service
    const StatusCode sc =
      this->toolSvc()->retrieveTool ( type , name , Tool , parent , create ) ;
    if ( sc.isFailure() )
    { Exception("tool():: Could not retrieve Tool '" + type + "'/'" + name + "'", sc ) ; }
    if ( 0 == Tool )
    { Exception("tool():: Could not retrieve Tool '" + type + "'/'" + name + "'"     ) ; }
    // add the tool into list of known tools to be properly released
    addToToolList( Tool );
  }
  // return *VALID* located tool
  return Tool ;
}
// ============================================================================

// ============================================================================
// the useful method for location of tools.
// ============================================================================
template < class PBASE >
template < class TOOL  >
inline TOOL* GaudiCommon<PBASE>::tool( const std::string& type   ,
                                       const IInterface*  parent ,
                                       bool               create ) const
{
  // check the environment
  Assert ( 0 != PBASE::toolSvc() , "IToolSvc* points to NULL!" );
  // retrieve the tool from Tool Service
  TOOL* Tool = 0 ;
  const StatusCode sc =
    this->toolSvc() -> retrieveTool ( type, Tool, parent , create   );
  if ( sc.isFailure() )
  { Exception("tool():: Could not retrieve Tool '" + type + "'", sc ) ; }
  if ( 0 == Tool )
  { Exception("tool():: Could not retrieve Tool '" + type + "'"     ) ; }
  // add the tool into the list of known tools to be properly released
  addToToolList( Tool );
  // return *VALID* located tool
  return Tool ;
}
// ============================================================================

// ============================================================================
// the useful method for location of services
// ============================================================================
template < class PBASE   >
template < class SERVICE >
inline SERVICE* GaudiCommon<PBASE>::svc( const std::string& name   ,
                                         const bool         create ) const
{
  SERVICE* s = 0 ;
  Assert ( 0 != this->svcLoc() , "ISvcLocator* points to NULL!" );
  // retrieve the service using templated method Algorithm::service
  StatusCode sc = this->svcLoc() -> service( name , s , create ) ;
  // check the results
  if ( sc.isFailure() )
  { Exception ( "svc():: Could not retrieve Svc '" + name + "'", sc ) ; }
  if ( 0 == s         )
  { Exception ( "svc():: Could not retrieve Svc '" + name + "'"     ) ; }
  // add the tool into list of known tools, to be properly released
  addToServiceList( s, name  ) ;
  // return *VALID* located service
  return s ;
}
// ============================================================================

// ============================================================================
// Short-cut  to get a pointer to the UpdateManagerSvc
// ============================================================================
template <class PBASE>
inline IUpdateManagerSvc *
GaudiCommon<PBASE>::updMgrSvc() const
{
  if ( !m_updMgrSvc )
  { m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc",true); }
  return m_updMgrSvc ;
}
// ============================================================================

// ============================================================================
// Short-cut  to get a pointer to the
// ============================================================================
template <class PBASE>
inline IDataProviderSvc *
GaudiCommon<PBASE>::fastContainersSvc() const
{
  if ( !m_fastContainersSvc )
  { m_fastContainersSvc = svc<IDataProviderSvc>("FastContainersSvc",true); }
  return m_fastContainersSvc ;
}

// ============================================================================
// Get a fast container
// ============================================================================
template < class PBASE   >
template < class T >
inline TransientFastContainer<T> *
GaudiCommon<PBASE>::getFastContainer( const std::string &location,
                                      typename TransientFastContainer<T>::size_type initial )
{
  typedef TransientFastContainer<T> container_type;

  IDataProviderSvc* svc = fastContainersSvc();
  Assert( 0 != svc , "getFastContainer(): cannot get FastContainersSvc" );

  container_type *ptr = NULL;
  SmartDataPtr<container_type> obj( svc, location );
  if (!obj){
    ptr = new container_type(initial);
    StatusCode status = svc->registerObject(location,ptr);
    if ( !status.isSuccess() ){
      Exception("getFastContainer(): cannot register '" +
                System::typeinfoName( typeid( *ptr ) ) +
                "' at address '" + location + "'"  , status );
    }
  } else {
    ptr = obj;
    if ( !ptr ){
      Exception("getFastContainer(): No valid container at '" + location + "'");
    }
  }

  return ptr;
}
// ============================================================================

// ============================================================================
// predefined configurable message stream for the effective printouts
// ============================================================================
template <class PBASE>
inline MsgStream&
GaudiCommon<PBASE>::msgStream ( const MSG::Level level ) const
{
  if ( !m_msgStream )
  { m_msgStream = new MsgStream ( PBASE::msgSvc() , this->name() ) ; }
  return *m_msgStream << level ;
}
// ============================================================================

// ============================================================================
// Assertion - throw exception, if condition is not fulfilled
// ============================================================================
template <class PBASE>
inline StatusCode GaudiCommon<PBASE>::Assert( const bool         OK  ,
                                              const std::string& msg ,
                                              const StatusCode   sc  ) const
{
  return ( OK ? StatusCode(StatusCode::SUCCESS, true) : Exception( msg , sc ) ) ;
}
// ============================================================================

// ============================================================================
// Delete the current message stream object
// ============================================================================
template <class PBASE>
inline void GaudiCommon<PBASE>::resetMsgStream() const
{
  if ( 0 != m_msgStream ) { delete m_msgStream; m_msgStream = 0; }
}
// ============================================================================

// ============================================================================
// Assertion - throw exception, if condition is not fulfilled
// ============================================================================
template <class PBASE>
inline StatusCode
GaudiCommon<PBASE>::Assert( const bool        OK  ,
                            const char*       msg ,
                            const StatusCode  sc  ) const
{
  return OK ? StatusCode(StatusCode::SUCCESS, true) : Exception( msg , sc ) ;
}
// ============================================================================

// ============================================================================
/** @def ALG_ERROR
 *  Small and simple macro to add into error message the file name
 *  and the line number for easy location of the problematic lines.
 *
 *  @code
 *
 *  if ( a < 0 ) { return ALG_ERROR( "'a' is negative" , 301 ) ; }
 *
 *  @endcode
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2002-10-29
 */
// ============================================================================
#define ALG_ERROR( message , code )                                     \
  ( Error( message                                   +                  \
           std::string             ( " [ at line " ) +                  \
           GaudiAlg::fileLine      (   __LINE__    ) +                  \
           std::string             ( " in file '"  ) +                  \
           std::string             (   __FILE__    ) + "']" , code ) )

// ============================================================================

#endif // GAUDIALG_GAUDICOMMONIMP_H
