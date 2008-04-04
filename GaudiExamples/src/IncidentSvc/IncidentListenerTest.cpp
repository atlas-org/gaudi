#include "IncidentListenerTest.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "GaudiKernel/MsgStream.h"


#include "GaudiKernel/GaudiException.h"

//=============================================================================
IncidentListenerTest::IncidentListenerTest( const std::string& name,
                                            ISvcLocator* svcloc,
                                            long shots )
  :m_name(name),m_svcloc(svcloc),m_shots(shots),m_refCount(0)
{
	if (svcloc->service("MessageSvc",m_msgSvc).isFailure())
		throw GaudiException("Cannot find MessageSvc",m_name,StatusCode::FAILURE);
	if (svcloc->service("IncidentSvc",m_incSvc).isFailure())
		throw GaudiException("Cannot find IncidentSvc",m_name,StatusCode::FAILURE);	
}

IncidentListenerTest::~IncidentListenerTest(){
	m_msgSvc->release();
	m_incSvc->release();
}

//=============================================================================
StatusCode IncidentListenerTest::queryInterface(const InterfaceID& riid,
                                                void** ppvInterface)
{
	if ( IIncidentListener::interfaceID() == riid )    {
    *ppvInterface = (IIncidentListener*)this;
  }
  else  {
  	*ppvInterface = NULL;
		return StatusCode::FAILURE;
  }
  addRef();
  return StatusCode::SUCCESS;
}

//=============================================================================
void IncidentListenerTest::handle(const Incident &incident) {
	MsgStream log( m_msgSvc, m_name );
	log << MSG::INFO << "Handling incident '" << incident.type() << "'" << endmsg;
	if ( ( m_shots > 0 ) && ( --m_shots == 0 ) ) {
		log << MSG::INFO << "deregistering" << endmsg;
		m_incSvc->removeListener(this,incident.type());
	}
}


//=============================================================================
unsigned long IncidentListenerTest::addRef()   {
  m_refCount++;
  return m_refCount;
}

//=============================================================================
unsigned long IncidentListenerTest::release()   {
  unsigned long count = --m_refCount;
  if( count == 0) {
    delete this;
  }
  return count;
}
