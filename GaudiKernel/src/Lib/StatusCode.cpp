#define GAUDIKERNEL_STATUSCODE_CPP 1

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IStatusCodeSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IssueSeverity.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

bool StatusCode::s_checking(false);

void StatusCode::enableChecking() {
  s_checking = true;
}

void StatusCode::disableChecking() {
  s_checking = false;
}

IssueSeverity* StatusCode::cloneSeverity(const IssueSeverity* sev)
{
  if( sev ) return new IssueSeverity(*sev);
  else      return 0;
}

const IssueSeverity& StatusCode::severity() const {
  static IssueSeverity dummy;
  if (m_severity) return *m_severity;
  else            return dummy;
}

StatusCode& StatusCode::operator=(const StatusCode& rhs) {
  if (this == &rhs) return *this; // Protection against self-assignment
  d_code = rhs.d_code;
  m_checked = rhs.m_checked;
  rhs.m_checked = true;
  if (m_severity) delete m_severity;
  m_severity = rhs.m_severity ? cloneSeverity(rhs.m_severity): 0;
  return *this;
}

StatusCode::~StatusCode() {
  if(s_checking) {

    if (!m_checked && !GaudiException::s_proc) {

      IMessageSvc* msg = 0 ;
      if(Gaudi::svcLocator()->service("MessageSvc", msg, true).isFailure()) {
        msg = 0 ; // one cannot rely on the returned value on the previous line
      }

      IStatusCodeSvc *scs = 0 ;
      if (Gaudi::svcLocator()->service("StatusCodeSvc",scs,true).isFailure()) {
        scs = 0 ; // one cannot rely on the returned value on the previous line
      }

      const size_t depth = 21;
      void* addresses[depth];

      std::string lib, fnc;
      void* addr = 0;
      /// @FIXME : (MCl) use backTrace(std::string&, const int, const int) instead
      if (System::backTrace(addresses, depth)) {

        if (System::getStackLevel(addresses[2], addr, fnc, lib)) {

          if (scs) {
            scs->regFnc(fnc,lib);
          } else {
            if (msg) {
              MsgStream log(msg,"StatusCode");
              log << MSG::WARNING << "Unchecked in " << fnc << " " << lib << endmsg;
            } else {
              std::cout << MSG::WARNING << "Unchecked in " << fnc << " " << lib << std::endl;
            }
          }

        }

      }
      if (msg) msg->release();
      if (scs) scs->release();
    }
  }
  if (m_severity) delete m_severity;
}

