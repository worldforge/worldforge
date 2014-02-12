#ifndef ERIS_EVENT_SERVICE_H
#define ERIS_EVENT_SERVICE_H

#include <sigc++/signal.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <mutex>
#include <queue>

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

/**
 * @brief Handles polling of the IO system as well as making sure that registered handlers are run on the main thread.
 *
 * Call runEvents in your main loop.
 * Use runOnMainThread to posts function from background threads.
 */
class EventService
{
public:

    /**
     * @brief Ctor.
     * @param io_service The main io_service of the system.
     */
    EventService(boost::asio::io_service& io_service);

    /**
     * @brief Dtor.
     */
    ~EventService();

    /**
     * @brief Adds a handler which will be run on the main thread.
     *
     * This method should mainly be called from background threads.
     * The execution of the handler will be interleaved with the IO polling, making sure
     * that at least one handler is executed each frame.
     * @param handler A function.
     */
    void runOnMainThread(const std::function<void()>& handler);

    /**
     * Polls and runs IO events as well as handlers until the deadline.
     *
     * Call this in your main loop.
     * @param runUntil A future time, obtained through boost::asio::time_traits<boost::posix_time::ptime>.
     * @param exitFlag A reference to a flag, which when set will exit the wait.
     */
    void processEvents(const boost::posix_time::ptime& runUntil, bool& exitFlag);

    /**
     * Polls and runs IO events as well as handlers for the specified duration.
     *
     * Call this in your main loop.
     * @param runFor A duration.
     * @param exitFlag A reference to a flag, which when set will exit the wait.
     */
    void processEvents(const boost::posix_time::time_duration& runFor, bool& exitFlag);

    /**
     * @brief Processes all registered handlers.
     *
     * @see runOnMainThread
     *
     * @return The number of handles that were run.
     */
    size_t processAllHandlers();

private:

    friend class TimedEvent;
    boost::asio::io_service& m_io_service;
    boost::asio::io_service::work* m_work;

    /**
     * @brief A queue of handlers which are to be run on the main thread.
     */
    std::deque<std::function<void()>> m_handlers;

    /**
     * @brief Creates a timer, mainly used by TimedEvent
     * @return A deadline timer.
     */
    boost::asio::deadline_timer* createTimer();

};

} // of namespace Eris

#endif // of ERIS_EVENT_SERVICE_H
