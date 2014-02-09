#include <Eris/EventService.h>

#include <boost/asio.hpp>

#include <cassert>


namespace Eris
{


TimedEvent::TimedEvent(EventService& eventService, const boost::posix_time::time_duration& duration, const std::function<void()>& callback)
: m_timer(eventService.createTimer())
{
    assert(m_timer);
    m_timer->expires_from_now(duration);
    m_timer->async_wait([&, callback](const boost::system::error_code& ec){
        if (!ec) {
            callback();
        }
    });
}

TimedEvent::~TimedEvent()
{
    delete m_timer;
}


EventService::EventService(boost::asio::io_service& io_service): m_io_service(io_service)
{
}

EventService::~EventService()
{
}

boost::asio::deadline_timer* EventService::createTimer()
{
    return new boost::asio::deadline_timer(m_io_service);
}

void EventService::post(const std::function<void()>& handler)
{
    m_io_service.post(handler);
}


} // of namespace Eris
