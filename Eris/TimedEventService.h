#ifndef ERIS_TIMED_EVENT_SERVICE_H
#define ERIS_TIMED_EVENT_SERVICE_H

#include <wfmath/timestamp.h>

#include <sigc++/signal.h>

#include <set>

namespace Eris
{

/**
@brief Abstract interface for things which occur after a period of time.
*/
class TimedEvent
{
public:
    virtual ~TimedEvent()
    {
    }
    
    /**
    @brief Implement the expiry behaviour of this object.
    The TimedEvent is automatically removed from the service before this
    method is called, so deleting the object, or re-registering it are
    permitted.
    */
    virtual void expired() = 0;
    
    /**
    The time value when this event is due
    */
    virtual const WFMath::TimeStamp& due() const = 0;
};

class EventsByDueOrdering
{
public:
    bool operator()(const TimedEvent* a, const TimedEvent* b) const
    {
        return a->due() < b->due();
    }
};

class TimedEventService
{
public:

    static TimedEventService* instance();

    static void del();

    /**
    @brief Tick all the timed events registered with the service instance.
    @ret The period in milliseconds until the next event is due
    */
    unsigned long tick(bool idle = false);

    /**
    */
    void registerEvent(TimedEvent* te);

    /**
    */
    void unregisterEvent(TimedEvent* te);

    /**
    @brief Signal emitted when tick is idle
    */
    sigc::signal<void> Idle;
private:
    TimedEventService();
    
    static TimedEventService* static_instance;
    
    typedef std::set<TimedEvent*, EventsByDueOrdering> TimedEventsByDue;
    TimedEventsByDue m_events;
};

} // of namespace Eris

#endif // of ERIS_TIMED_EVENT_SERVICE_H
