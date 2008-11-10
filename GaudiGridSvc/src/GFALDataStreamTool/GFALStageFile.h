// $Id: GFALStageFile.h,v 1.2 2007/09/17 18:34:53 hmd Exp $
#ifndef GAUDIGRIDSVC_GFALSTAGEFILE_H 
#define GAUDIGRIDSVC_GFALSTAGEFILE_H 1

// Include files
#include <string>
#include <vector>
#include <iterator>

//From Gaudi
#include <GaudiKernel/StatusCode.h>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

//GFAL_API utilities
#include <fcntl.h>
#include "gfal_api.h"

/** @class GFALStageFile GFALStageFile.h GaudiGridSvc/GFALStageFile.h
 *  
 *
 *  @author Andres Felipe Osorio Oliveros
 *  @author Greig Cowan
 *
 *
 *  @date   2006-11-11
 */


class GFALStageFile {
public: 

  enum Status {FAILED=-1,STGING,STGED,EXIST,UNKNOWN};
  
  /// Standard constructor
  GFALStageFile( ); 
  GFALStageFile(std::string name,IMessageSvc* ms);
  GFALStageFile(const GFALStageFile&);
  GFALStageFile& operator=(const GFALStageFile&);
  
  virtual ~GFALStageFile( );

  StatusCode initialize();

  Status getStatus() const ; 
  int getRequestID() const;
  std::string getSurl() const ;
  std::string getTurl() const ;
  bool isPFN() const;
  
  void setStatus(int );
  void setSurl( const std::string &);
  void setTurl( const std::string &);
  void setRequestID( int );
  
  static const std::string name() { return "StageFile" ; }

  const std::string fileName() const { return m_filename ; }

  const std::string rawFileName() const { return m_rawfilename ; }

  bool isValid() const;
  
  IMessageSvc* msgSvc() const ;
  
protected:

private:
  
  std::string m_filename ;
  std::string m_rawfilename ;
  
  Status m_status;
  int m_reqid    ;
  
  std::string m_surl ;
  std::string m_turl ;
  
  bool is_pfn  ;
  bool is_lfn  ;
  bool is_guid ;
  bool is_surl ;
  
  bool m_isvalid;
  
  mutable IMessageSvc* m_MS ;
  
};
#endif // GAUDIGRIDSVC_GFALSTAGEFILE_H
