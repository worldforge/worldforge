#ifndef ERIS_TIMED_EVENT_SERVICE_H
#define ERIS_TIMED_EVENT_SERVICE_H

#include <sigc++/signal.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace Eris
{

/**
@brief Class for things which occur after a period of time.
*/
class TimedEvent
{
public:
    
    TimedEvent(const boost::posix_time::time_duration& duration, const std::function<void()>& callback);
    ~TimedEvent();

private:
    boost::asio::deadline_timer* m_timer;
};

class TimedEventService
{
public:

    TimedEventService(boost::asio::io_service& io_service);
    ~TimedEventService();

    static TimedEventService& instance();

    void post(const std::function<void()>& handler);

private:

    friend class TimedEvent;
    
    boost::asio::deadline_timer* createTimer();

    static TimedEventService* static_instance;
    
    boost::asio::io_service& m_io_service;

};

} // of namespace Eris

#endif // of ERIS_TIMED_EVENT_SERVICE_H
