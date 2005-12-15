#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Timeout.h>

#include <Eris/Poll.h>
#include <Eris/Exceptions.h>
#include <Eris/LogStream.h>

#include <limits.h>
#include <set>

using WFMath::TimeStamp;
using WFMath::TimeDiff;

namespace Eris
{

static TimeDiff TD_ZERO(0);
static TimeDiff TD_MAX(ULONG_MAX);

typedef std::set<Timeout*> TimeoutSet;
static TimeoutSet global_allTimeouts;
	
Timeout::Timeout(unsigned long milli) :
	_fired(false)
{
	global_allTimeouts.insert(this);
    _due = TimeStamp::now() + milli;
	Poll::newTimeout();
}

Timeout::~Timeout()
{
    unsigned int erased = global_allTimeouts.erase(this);
    assert(erased == 1);
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

unsigned long Timeout::pollAll()
{
    TimeStamp now = TimeStamp::now();
    TimeDiff wait = TD_MAX;
    
    TimeoutSet::iterator T=global_allTimeouts.begin();
    for (; T != global_allTimeouts.end(); ++T)
    {
        TimeDiff this_wait = (*T)->poll(now);
        if (this_wait < wait) wait = this_wait;
    }
    
    return wait.milliseconds();
}

}
