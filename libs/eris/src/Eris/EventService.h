#ifndef ERIS_EVENT_SERVICE_H
#define ERIS_EVENT_SERVICE_H

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/noncopyable.hpp>

#include <queue>
#include <functional>

namespace Eris {

class EventService;

class ActiveMarker;

/**
@brief Class for things which occur after a period of time.
*/
class TimedEvent {
public:

	TimedEvent(EventService& eventService, const std::chrono::steady_clock::duration& duration, const std::function<void()>& callback);

	~TimedEvent();

private:
	std::unique_ptr<boost::asio::steady_timer> m_timer;
};

template<typename T>
class WaitFreeQueue;

/**
 * @brief Handles polling of the IO system as well as making sure that registered handlers are run on the main thread.
 *
 * Call runEvents in your main loop.
 * Use runOnMainThread to posts function from background threads.
 */
class EventService : boost::noncopyable {
public:

	/**
	 * @brief Ctor.
	 * @param io_service The main io_service of the system.
	 */
	explicit EventService(boost::asio::io_context& io_service);

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
	 * @param activeMarker An active marker which is used for cancellation of tasks. If it evaluates to "false" the handler won't be invoked. Use ActiveMarker for convenience.
	 */
	void runOnMainThread(const std::function<void()>& handler,
						 std::shared_ptr<bool> activeMarker = std::make_shared<bool>(true));


	/**
	 * Runs a handler on the main thread after a certain delay.
	 * @param handler A function.
	 * @param duration The duration to wait.
	 * @param activeMarker An active marker which is used for cancellation of tasks. If it evaluates to "false" the handler won't be invoked. Use ActiveMarker for convenience.
	 */
	void runOnMainThreadDelayed(const std::function<void()>& handler,
								const std::chrono::steady_clock::duration& duration,
								std::shared_ptr<bool> activeMarker = std::make_shared<bool>(true));

	/**
	 * @brief Processes all registered handlers.
	 *
	 * @see runOnMainThread
	 *
	 * @return The number of handles that were run.
	 */
	size_t processAllHandlers();

	/**
	 * @brief Processes one handler, if possible
	 *
	 * @see runOnMainThread
	 *
	 * @return The number of handles that were run.
	 */
	size_t processOneHandler();

private:

	friend class TimedEvent;

	boost::asio::io_context& m_io_service;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work;

	/**
	 * @brief A queue of handlers which are to be run on the main thread.
	 * These are collected on the main thread from the m_background_handlers_queue field.
	 */
	std::deque<std::function<void()>> m_handlers;

	/**
	 * @brief A queue of handlers, meant only to have values pushed on to it.
	 *
	 * These values are then popped through the collectHandlersQueue() method
	 * and put onto the m_handlers queue.
	 */
	std::unique_ptr<WaitFreeQueue<std::function<void()>>> m_background_handlers_queue;

	/**
	 * @brief Creates a timer, mainly used by TimedEvent
	 * @return A deadline timer.
	 */
	std::unique_ptr<boost::asio::steady_timer> createTimer();

	/**
	 * @brief Transfers all handlers from the m_background_handlers_queue to the m_handlers queue.
	 */
	size_t collectHandlersQueue();


};

} // of namespace Eris

#endif // of ERIS_EVENT_SERVICE_H
