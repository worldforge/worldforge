#ifndef ERIS_WAIT_H
#define ERIS_WAIT_H

#include <sigc++/object.h>
#include "Types.h"

namespace Atlas { namespace Objects {
	class Root;
}}

namespace Eris
{

// forward declerations
class Dispatcher;
	
class WaitForBase : public SigC::Object
{
public:
	WaitForBase(const Atlas::Message::Object &m);
	virtual ~WaitForBase() {;}
		
	bool isPending() const
	{ return _pending; }
	
	void fire();
	
	/** Predicate matching STL UnaryFunction, indicating whether the wait has been fired. This
	is used in STL remove_if and so on. */
	static bool hasFired(WaitForBase *w)
	{ return w->_pending; }
	
protected:
	bool _pending;
	Atlas::Message::Object _msg;
};

class WaitForDispatch : public WaitForBase
{
public:
	WaitForDispatch(const Atlas::Message::Object &msg,  
		const std::string &ppath,
		Dispatcher *dsp);

	WaitForDispatch(const Atlas::Objects::Root &msg, 
		const std::string &ppath,
		Dispatcher *dsp);

	virtual ~WaitForDispatch();

protected:
	std::string _parentPath;
	Dispatcher* _dsp;
};

class WaitForSignal : public WaitForBase
{
public:	
	WaitForSignal(Signal &sig, const Atlas::Message::Object &msg);
	virtual ~WaitForSignal();
protected:
	
};

}

#endif