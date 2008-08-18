#ifndef      __GAUDI_INTERFACES_IVALIDITY_H__
#define      __GAUDI_INTERFACES_IVALIDITY_H__

//
//   class IValidity : interface for asking the validity of a given object
//                      
//

namespace Gaudi { class Time; }

class StatusCode;

class IValidity
{
  
 public:
  
  // is the Object valid? (it can be always invalid)
  virtual       bool           isValid    ()                const = 0 ; 
  
  // is the Object valid for a given time?
  virtual       bool           isValid    ( const Gaudi::Time& )  const = 0 ; 
  
  // since what time the Objest is valid?
  virtual const Gaudi::Time&   validSince ()                const = 0 ; 
  
  // till  what time the Object is Valid?
  virtual const Gaudi::Time&   validTill  ()                const = 0 ;
  
  // set the validity range of the Object
  virtual       void           setValidity       ( const Gaudi::Time& , const Gaudi::Time& ) = 0 ; 
  // set the validity time of the Object 
  virtual       void           setValiditySince  ( const Gaudi::Time& ) = 0 ; 
  
  // set the validity time of the Object 
  virtual       void           setValidityTill   ( const Gaudi::Time& ) = 0 ;
  
  // destructor
  virtual ~IValidity(){};

};

//
//
//



#endif   //  __GAUDI_INTERFACES_IVALIDITY_H__
