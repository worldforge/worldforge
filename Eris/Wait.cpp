#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <Atlas/Objects/Root.h>

#include "atlas_utils.h"
#include "Utils.h"

#include "Wait.h"
#include "Connection.h"
#include "Dispatcher.h"
#include "SignalDispatcher.h"

namespace Eris {

void WaitForBase::fire()
{
	assert(!_pending);
	_pending = true;
	
	std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(_msg) ));
	Log("firing WaitFor %p, content is %s", this, summary.c_str());
	Connection::Instance()->postForDispatch(_msg);
}
	
//////////////////////////////////////////////////////////////////////////////////////////
	
WaitForDispatch::WaitForDispatch(const Atlas::Message::Object &msg,  
	const std::string &ppath,
	Dispatcher *dsp) :
	WaitForBase(msg),
	_parentPath(ppath),
	_dsp(dsp)
{
	Dispatcher *pr = Connection::Instance()->getDispatcherByPath(ppath);
	pr->addSubdispatch( dsp );
	dsp->addSubdispatch(new SignalDispatcher0("sig", SigC::slot(this, &WaitForBase::fire)));
}

WaitForDispatch::WaitForDispatch(const Atlas::Objects::Root &obj,  
	const std::string &ppath,
	Dispatcher *dsp) :
	WaitForBase(obj.AsObject()),
	_parentPath(ppath),
	_dsp(dsp)
{
	Dispatcher *pr = Connection::Instance()->getDispatcherByPath(ppath);
	pr->addSubdispatch( dsp );
	dsp->addSubdispatch(new SignalDispatcher0("sig", SigC::slot(this, &WaitForBase::fire)));
}


WaitForDispatch::~WaitForDispatch()
{
	Dispatcher *pr = Connection::Instance()->getDispatcherByPath(_parentPath);
	pr->rmvSubdispatch( _dsp );
}

//////////////////////////////////////////////////////////////////////////////////////

WaitForSignal::WaitForSignal(Signal &sig, const Atlas::Message::Object &msg) :
	WaitForBase(msg)
{
	sig.connect(SigC::slot(this, &WaitForBase::fire));
}

};