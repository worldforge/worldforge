#include "EventService.h"
#include "Log.h"
#include "WaitFreeQueue.h"
#include "ActiveMarker.h"

#include <boost/asio.hpp>

#include <cassert>
#include <iostream>

namespace Eris
{

TimedEvent::TimedEvent(EventService& eventService,
        const boost::posix_time::time_duration& duration,
        const std::function<void()>& callback) :
        m_timer(eventService.createTimer())
{
    assert(m_timer);
    m_timer->expires_from_now(duration);
    m_timer->async_wait([&, callback](const boost::system::error_code& ec) {
        if (!ec) {
            callback();
        }
    });
}

TimedEvent::~TimedEvent()
{
    delete m_timer;
}

EventService::EventService(boost::asio::io_service& io_service) :
        m_io_service(io_service),
        m_work(new boost::asio::io_service::work(io_service)),
        m_background_handlers_queue(new WaitFreeQueue<std::function<void()>>())
{
}

EventService::~EventService()
{
    delete m_work;
    //Poll to make sure that all pending asio handlers are processed, since these might create handlers which needs to be processed.
    m_io_service.poll();
    processAllHandlers();
    delete m_background_handlers_queue;
}

boost::asio::deadline_timer* EventService::createTimer()
{
    return new boost::asio::deadline_timer(m_io_service);
}

void EventService::runOnMainThread(const std::function<void()>& handler,
                                   std::shared_ptr<bool> activeMarker)
{
    m_background_handlers_queue->push([handler, activeMarker]() {
        if (*activeMarker) {
            handler();
        }
    });
}

void EventService::runOnMainThreadDelayed(const std::function<void()>& handler,
                                          const boost::posix_time::time_duration& duration,
                                          std::shared_ptr<bool> activeMarker) {
    auto timer = std::make_shared<boost::asio::deadline_timer>(m_io_service);
    timer->expires_from_now(duration);
    timer->async_wait([&, handler, activeMarker, timer](const boost::system::error_code& ec) {
        if (!ec) {
            runOnMainThread(handler, activeMarker);
        }
    });

}

size_t EventService::processAllHandlers()
{
	collectHandlersQueue();

    size_t count = 0;
    while (!m_handlers.empty()) {
        std::function<void()> handler = std::move(this->m_handlers.front());
		m_handlers.pop_front();
        count++;
        handler();
		collectHandlersQueue();
    }
    return count;
}

size_t EventService::collectHandlersQueue()
{
    size_t count = 0;
    WaitFreeQueue<std::function<void()>>::node * x = m_background_handlers_queue->pop_all();
    while (x) {
        WaitFreeQueue<std::function<void()>>::node* tmp = x;
        x = x->next;
        m_handlers.push_back(std::move(tmp->data));
        delete tmp;
        count++;
    }
    return count;
}

size_t EventService::processOneHandler() {
	collectHandlersQueue();
    //If there are handlers registered, execute one of them now
    if (!m_handlers.empty()) {
        std::function<void()> handler = std::move(this->m_handlers.front());
		m_handlers.pop_front();
		handler();
        return 1;
    }
    return 0;
}

} // of namespace Eris
