#ifndef ERIS_EVENT_SERVICE_H
#define ERIS_EVENT_SERVICE_H

#include <sigc++/signal.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace Eris
{

class EventService;
/**
@brief Class for things which occur after a period of time.
*/
class TimedEvent
{
public:
    
    TimedEvent(EventService& eventService, const boost::posix_time::time_duration& duration, const std::function<void()>& callback);
    ~TimedEvent();

private:
    boost::asio::deadline_timer* m_timer;
};

class EventService
{
public:

    EventService(boost::asio::io_service& io_service);
    ~EventService();

    void post(const std::function<void()>& handler);

private:

    friend class TimedEvent;
    
    boost::asio::deadline_timer* createTimer();

    boost::asio::io_service& m_io_service;

};

} // of namespace Eris

#endif // of ERIS_EVENT_SERVICE_H
