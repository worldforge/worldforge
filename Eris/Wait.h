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
	WaitForBase(const Atlas::Message::Object &m) :
		_pending(false),
		_msg(m)
	{ ; }

	bool isPending() const
	{ return _pending; }
	
	void fire();
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

	~WaitForDispatch();

protected:
	std::string _parentPath;
	Dispatcher* _dsp;
};

class WaitForSignal : public WaitForBase
{
public:	
	WaitForSignal(Signal &sig, const Atlas::Message::Object &msg);

protected:
	
};

}

#endif