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
	
Timeout::Timeout(unsigned long milli) :
	_fired(false)
{
    _due = TimeStamp::now() + milli;
    TimedEventService::instance()->registerEvent(this);
}

Timeout::~Timeout()
{
    if (!_fired) TimedEventService::instance()->unregisterEvent(this);
}

void Timeout::cancel()
{
    _fired = true;
}

bool Timeout::isExpired() const
{
    return (_due < TimeStamp::now());
}

void Timeout::reset(unsigned long milli)
{
    if (!_fired) TimedEventService::instance()->unregisterEvent(this);
    
    _fired = false;
    _due = TimeStamp::now() + milli;
    TimedEventService::instance()->registerEvent(this);
}

void Timeout::extend(unsigned long milli)
{
    _due += milli;
}

void Timeout::expired()
{
    _fired = true;
    Expired();
}

}
