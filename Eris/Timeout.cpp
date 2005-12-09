#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Timeout.h>

#include <Eris/Poll.h>
#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>

#include <limits.h>

using WFMath::TimeStamp;
using WFMath::TimeDiff;

namespace Eris
{

static TimeDiff TD_ZERO(0);
static TimeDiff TD_MAX(ULONG_MAX);

std::map<Timeout::Label, Timeout*> Timeout::_allTimeouts;
	
Timeout::Timeout(const std::string &label, unsigned long milli) :
	_label(label),
	_fired(false)
{
	TimeoutMap::iterator T = _allTimeouts.find(_label);
	if (T != _allTimeouts.end())
	    throw InvalidOperation("Duplicate label '" + label + "' for timeout");
       
	_allTimeouts.insert(_allTimeouts.begin(),
	    TimeoutMap::value_type(_label, this));
	
	_due = TimeStamp::now() + milli;

	Poll::newTimeout();
}

Timeout::Timeout(const std::string &label, void* inst, unsigned long milli) :
	_label(label, inst),
	_fired(false)
{
    if (_allTimeouts.count(_label)) {
        throw InvalidOperation("Duplicate label '" + label + "' for timeout attached to instace");
    }
    
    _allTimeouts.insert(TimeoutMap::value_type(_label, this));
    _due = TimeStamp::now() + milli;
    Poll::newTimeout();
}

Timeout::~Timeout()
{
    assert(_allTimeouts.count(_label) == 1);
    _allTimeouts.erase(_label);
}

void Timeout::cancel()
{
    _fired = true;
}

bool Timeout::isExpired() const
{
    return (_due < TimeStamp::now());
}

TimeDiff Timeout::poll(const TimeStamp &t)
{
    if (!_fired)
    {
        TimeDiff delta = _due - t;
        if (delta > TD_ZERO) return delta; // not finished yet
        
        _fired = true;
        Expired();	// invoke the signal
    }

    return TD_MAX; // Doesn't need to be called again
}

void Timeout::reset(unsigned long milli)
{
    _fired = false;
    _due = TimeStamp::now() + milli;
}

void Timeout::extend(unsigned long milli)
{
    _due += milli;
}

/*
const Timeout* Timeout::findByName(const std::string &nm, void* inst)
{
	TimeoutMap::iterator T = _allTimeouts.find(Label(nm, inst));
	if (T == _allTimeouts.end())
		return NULL;
	return T->second;
}
*/

unsigned long Timeout::pollAll()
{
    TimeStamp now = TimeStamp::now();
    TimeDiff wait = TD_MAX;
    
    for (TimeoutMap::iterator T=_allTimeouts.begin(); T != _allTimeouts.end(); ++T)
    {
        TimeDiff this_wait = T->second->poll(now);
        if(this_wait < wait)
            wait = this_wait;
    }
    
    return wait.milliseconds();
}


}
