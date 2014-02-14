#include "EventService.h"
#include "Log.h"
#include "WaitFreeQueue.h"

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
        m_io_service(io_service), m_work(
                new boost::asio::io_service::work(io_service)), m_background_handlers_queue(
                new WaitFreeQueue<std::function<void()>>())
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

void EventService::runOnMainThread(const std::function<void()>& handler)
{
    m_background_handlers_queue->push(handler);
}

void EventService::processEvents(const boost::posix_time::ptime& runUntil,
        bool& exitFlag)
{
    bool exitLoop = false;
    boost::asio::deadline_timer deadlineTimer(m_io_service);
    deadlineTimer.expires_at(runUntil);
    //First poll all IO handlers.
    m_io_service.poll();


    deadlineTimer.async_wait([&](boost::system::error_code ec) {
        if (!ec) {
            exitLoop = true;
        }
    });
    //Keep on running IO handlers until we need to render again. By using a "do" loop
    //we guarantee that we'll execute at least one handler per frame even if the process
    //can't keep up with the requested FPS.
    do {
        collectHandlersQueue();
        //If there are handlers registered, execute one of them now
        if (!m_handlers.empty()) {
            auto handler = this->m_handlers.front();
            m_handlers.pop_front();
            try {
                handler();
            } catch (const std::exception& ex) {
                error() << "Error when executing handler: " << ex.what();
            }
        } else {
            m_io_service.run_one();
        }
    } while (!exitLoop && !exitFlag
            && (boost::asio::time_traits<boost::posix_time::ptime>::now()
                    < runUntil));
    deadlineTimer.cancel();
}

void EventService::processEvents(const boost::posix_time::time_duration& runFor,
        bool& exitFlag)
{
    processEvents(
            boost::asio::time_traits<boost::posix_time::ptime>::now() + runFor,
            exitFlag);
}

size_t EventService::processAllHandlers()
{
    collectHandlersQueue();

    size_t count = 0;
    while (!m_handlers.empty()) {
        auto handler = this->m_handlers.front();
        m_handlers.pop_front();
        count++;
        try {
            handler();
        } catch (const std::exception& ex) {
            error() << "Error when executing handler: " << ex.what();
        }
        collectHandlersQueue();
   }
    return count;
}

void EventService::collectHandlersQueue()
{
    auto * x = m_background_handlers_queue->pop_all();
    while (x) {
        auto tmp = x;
        x = x->next;
        m_handlers.push_back(tmp->data);
        delete tmp;
    }
}

} // of namespace Eris
