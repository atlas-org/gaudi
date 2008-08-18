// $Id: StatusCode.h,v 1.9 2007/01/17 17:18:27 hmd Exp $
#ifndef GAUDIKERNEL_STATUSCODE_H
#define GAUDIKERNEL_STATUSCODE_H

#include <ostream>

/**
 * @class StatusCode StatusCode.h GaudiKernel/StatusCode.h
 *
 * This class is used for returning status codes from appropriate routines.
 *
 * @author Iain Last
 * @author Pere Mato
 * @author Sebastien Ponce
 */

class IMessageSvc;
class IStatusCodeSvc;
class IssueSeverity;

class IgnoreError {};

class StatusCode {
public:
  enum {
    FAILURE = 0,
    SUCCESS = 1,
    RECOVERABLE = 2
  };

  /// Constructor.
  StatusCode();
  StatusCode( unsigned long code, const IssueSeverity& );
  StatusCode( unsigned long code, bool checked = false );

  StatusCode( const StatusCode& sc );

  /// Destructor.
  ~StatusCode();

  /** Test for a status code of SUCCESS.
   * N.B. This is the only case where a function has succeeded.
   */
  bool isSuccess() const;

  /** Test for a status code of FAILURE.
   * N.B. This is a specific type of failure where there aren't any more
   * appropriate staus codes. To test for any failure use :
   * if ( !StatusCode.isSuccess() ) ...
   */
  bool isFailure() const;
  bool isRecoverable() const;

  /// Get the status code by value.
  unsigned long getCode() const;

  /// Set the status code by value.
  void setCode( unsigned long );

  /// Ignore the checking code;
  void setChecked() const;
  void ignore() const;

  /// Cast operator.
  operator unsigned long() const;

  /// Severity
  const IssueSeverity& severity() const;

  /// Assignment operator.
  StatusCode& operator=(unsigned long value);
  StatusCode& operator=(const StatusCode& rhs);

  /// Comparison operator.
  friend bool operator< ( const StatusCode& a, const StatusCode& b );

  /// Comparison operator.
  friend bool operator> ( const StatusCode& a, const StatusCode& b );

#ifndef _WIN32
  operator IgnoreError() const {
    m_checked = true;
    return IgnoreError();
  }
#endif

  static void enableChecking();
  static void disableChecking();

protected:
  /// The status code.
  unsigned long   d_code;      ///< The status code
  mutable bool    m_checked;   ///< If the Status code has been checked
  IssueSeverity*  m_severity;  ///< Pointer to a IssueSeverity

  static bool     s_checking;  ///< Global flag to control if StatusCode need to be checked

  static IssueSeverity* cloneSeverity(const IssueSeverity*);
};

inline StatusCode::StatusCode(): 
  d_code(SUCCESS), m_checked(false), m_severity(0) {}

inline StatusCode::StatusCode( unsigned long code, bool checked ) : 
  d_code(code),m_checked(checked), m_severity(0) {}

inline StatusCode::StatusCode( unsigned long code, const IssueSeverity& sev ) : 
  d_code(code),m_checked(false), m_severity(cloneSeverity(&sev)) {
}

inline StatusCode::StatusCode( const StatusCode &rhs ) {
  d_code = rhs.d_code;
  m_checked = rhs.m_checked;
  rhs.m_checked = true;
  m_severity = rhs.m_severity ? cloneSeverity(rhs.m_severity) : 0;
}

inline StatusCode& StatusCode::operator=(const StatusCode& rhs) {
  d_code = rhs.d_code;
  m_checked = rhs.m_checked;
  rhs.m_checked = true;
  m_severity = rhs.m_severity ? cloneSeverity(rhs.m_severity): 0;
  return *this;
}

inline bool StatusCode::isSuccess() const {
  m_checked = true;
  return (d_code == SUCCESS );
}

inline bool StatusCode::isFailure() const {
  m_checked = true;
  return (d_code != SUCCESS );
}

inline bool StatusCode::isRecoverable() const {
  m_checked = true;
  return (d_code == RECOVERABLE);
}

inline unsigned long StatusCode::getCode() const {
  m_checked = true;
  return d_code;
}

inline void StatusCode::setCode(unsigned long value) {
  m_checked = false;
  d_code = value;
}

inline void StatusCode::setChecked() const {
  m_checked = true;
}

inline void StatusCode::ignore() const {
  m_checked = true;
}

inline StatusCode::operator unsigned long() const  {
  m_checked = true;
  return d_code;
}

inline StatusCode& StatusCode::operator=(unsigned long value)   {
  d_code = value;
  m_checked = false;
  return *this;
}

inline bool operator< ( const StatusCode& a, const StatusCode& b ) {
  return a.d_code < b.d_code;
}

inline bool operator> ( const StatusCode& a, const StatusCode& b ) {
  return a.d_code > b.d_code;
}

inline std::ostream& operator<< ( std::ostream& s , const StatusCode& sc )
{
  if ( sc.isSuccess() ) { return s << "SUCCESS" ; }
  else if ( sc.isRecoverable() ) { return s << "RECOVERABLE" ; }
  s << "FAILURE" ;
  if ( StatusCode::FAILURE != sc.getCode() )
    { s << "(" << sc.getCode() << ")" ;}
  return s ;
}

#endif  // GAUDIKERNEL_STATUSCODES_H



