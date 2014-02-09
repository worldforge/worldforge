#include <Eris/TimedEventService.h>

#include <boost/asio.hpp>

#include <cassert>


namespace Eris
{


TimedEventService* TimedEventService::static_instance = nullptr;


TimedEvent::TimedEvent(const boost::posix_time::time_duration& duration, const std::function<void()>& callback)
: m_timer(TimedEventService::instance().createTimer())
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


TimedEventService::TimedEventService(boost::asio::io_service& io_service): m_io_service(io_service)
{
    assert(!static_instance);
    static_instance = this;
}

TimedEventService::~TimedEventService()
{
    static_instance = nullptr;
}

TimedEventService& TimedEventService::instance()
{
    assert(static_instance);
    
    return *static_instance;
}

boost::asio::deadline_timer* TimedEventService::createTimer()
{
    return new boost::asio::deadline_timer(m_io_service);
}

void TimedEventService::post(const std::function<void()>& handler)
{
    m_io_service.post(handler);
}


} // of namespace Eris
