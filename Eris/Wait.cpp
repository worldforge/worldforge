#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>
#include <sigc++/object.h>
#include <sigc++/object_slot.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif
#include <Atlas/Objects/Root.h>

#include "atlas_utils.h"
#include "Utils.h"

#include "Wait.h"
#include "Connection.h"
#include "Dispatcher.h"
#include "SignalDispatcher.h"

namespace Eris {

WaitForBase::WaitForBase(const Atlas::Message::Object &m, Connection *conn) :
		_pending(false),
		_msg(m),
		_conn(conn)
{ 
	conn->addWait(this);
}
	
void WaitForBase::fire()
{
	assert(!_pending);
	_pending = true;
	
	std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(_msg) ));
	//Log("firing WaitFor %p, content is %s", this, summary.c_str());
	_conn->postForDispatch(_msg);
}
	
//////////////////////////////////////////////////////////////////////////////////////////
	
WaitForDispatch::WaitForDispatch(const Atlas::Message::Object &msg,  
	const std::string &ppath,
	Dispatcher *dsp,
	Connection *conn) :
	WaitForBase(msg, conn),
	_parentPath(ppath),
	_dsp(dsp)
{
	Dispatcher *pr = conn->getDispatcherByPath(ppath);
	pr->addSubdispatch( dsp );
	dsp->addSubdispatch(new SignalDispatcher0("sig", SigC::slot(*this, &WaitForBase::fire)));
}

WaitForDispatch::WaitForDispatch(const Atlas::Objects::Root &obj,  
	const std::string &ppath,
	Dispatcher *dsp,
	Connection *conn) :
	WaitForBase(obj.AsObject(), conn),
	_parentPath(ppath),
	_dsp(dsp)
{
	Dispatcher *pr = conn->getDispatcherByPath(ppath);
	pr->addSubdispatch( dsp );
	dsp->addSubdispatch(new SignalDispatcher0("sig", SigC::slot(*this, &WaitForBase::fire)));
}


WaitForDispatch::~WaitForDispatch()
{
	Dispatcher *pr = _conn->getDispatcherByPath(_parentPath);
	pr->rmvSubdispatch( _dsp );
}

//////////////////////////////////////////////////////////////////////////////////////

WaitForSignal::WaitForSignal(Signal &sig, const Atlas::Message::Object &msg,
	Connection *conn) :
	WaitForBase(msg, conn)
{
	//Eris::Log("Created WaitForSignal %p", this);
	sig.connect(SigC::slot(*this, &WaitForBase::fire));
}

WaitForSignal::~WaitForSignal()
{
	;
}

}
