#include <Eris/TimedEventService.h>
#include <Eris/Poll.h>

#include <cassert>

using WFMath::TimeStamp;
using WFMath::TimeDiff;

namespace Eris
{

static TimeDiff TD_ZERO(0);

TimedEventService* TimedEventService::static_instance = NULL;

TimedEventService::TimedEventService()
{
}

TimedEventService* TimedEventService::instance()
{
    if (!static_instance)
    {
        static_instance = new TimedEventService;
    }
    
    return static_instance;
}

void TimedEventService::del()
{
    if (static_instance)
    {
        delete static_instance;
        static_instance = 0;
    }
}

unsigned long TimedEventService::tick()
{
    TimeStamp n(TimeStamp::now());
    TimedEventsByDue::iterator it = m_events.begin();
    
    unsigned long waitMsecs = 0xffff; // arbitrary big number
    while (it != m_events.end())
    {
        TimeDiff d = (*it)->due() - n;
        if (d <= TD_ZERO)
        {
            // expired
            TimedEvent* e = *it;
            m_events.erase(it++);
            
            // must not use e after calling expired(), it may delete self
            e->expired();
        } else {
            // all later events can wait too
            return d.milliseconds();
        }
    }
    
    return waitMsecs;
}

void TimedEventService::registerEvent(TimedEvent* te)
{
    assert(te);
    m_events.insert(te); // STL rocks, sometimes
    
    Poll::newTimedEvent(); // this could die?
}

void TimedEventService::unregisterEvent(TimedEvent* te)
{
    assert(te);    
    m_events.erase(te);
}

} // of namespace Eris
