#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Wait.h>

#include <Eris/atlas_utils.h>
#include <Eris/Utils.h>
#include <Eris/Connection.h>
#include <Eris/SignalDispatcher.h>

#include <Atlas/Objects/Root.h>

#include <sigc++/object_slot.h>

#include <cassert>

namespace Eris {

WaitForBase::WaitForBase(const Atlas::Message::Element &m, Connection *conn) :
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
	
WaitForDispatch::WaitForDispatch(const Atlas::Message::Element &msg,  
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
	WaitForBase(obj.asObject(), conn),
	_parentPath(ppath),
	_dsp(dsp)
{
	Dispatcher *pr = conn->getDispatcherByPath(ppath);
	assert(pr);
	pr->addSubdispatch( dsp );
	dsp->addSubdispatch(new SignalDispatcher0("sig", SigC::slot(*this, &WaitForBase::fire)));
}


WaitForDispatch::~WaitForDispatch()
{
	Dispatcher *pr = _conn->getDispatcherByPath(_parentPath);
	pr->rmvSubdispatch( _dsp );
}

//////////////////////////////////////////////////////////////////////////////////////

WaitForSignal::WaitForSignal(SigC::Signal0<void> &sig, const Atlas::Message::Element &msg,
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
