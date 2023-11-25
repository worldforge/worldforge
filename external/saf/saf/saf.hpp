// Copyright (c) 2022 Mohammad Nejati, Klemens D. Morgenstern, Ricahrd Hodges
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <mutex>
#include <variant>

#ifdef SAF_ASIO_STANDALONE
#include <asio/any_io_executor.hpp>
#include <asio/append.hpp>
#include <asio/associated_cancellation_slot.hpp>
#include <asio/post.hpp>
namespace saf
{
namespace net    = asio;
using error_code = std::error_code;
} // namespace saf
#else
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/append.hpp>
#include <boost/asio/associated_cancellation_slot.hpp>
#include <boost/asio/post.hpp>
namespace saf
{
namespace net    = boost::asio;
using error_code = boost::system::error_code;
} // namespace saf
#endif

namespace saf
{
enum class future_errc
{
    no_state = 1,
    unready_future,
    promise_already_satisfied,
    future_already_retrieved,
    broken_promise,
};
} // namespace saf

namespace std
{
template<>
struct is_error_code_enum<saf::future_errc> : true_type
{
};
} // namespace std

namespace saf
{
inline const std::error_category& future_category()
{
    static const struct : std::error_category
    {
        const char* name() const noexcept override
        {
            return "future";
        }

        std::string message(int ev) const override
        {
            switch (static_cast<future_errc>(ev))
            {
                case future_errc::no_state:
                    return "No associated state";
                case future_errc::unready_future:
                    return "Unready Future";
                case future_errc::promise_already_satisfied:
                    return "Promise already satisfied";
                case future_errc::future_already_retrieved:
                    return "Future already retrieved";
                case future_errc::broken_promise:
                    return "Broken promise";
                default:
                    return "Unknown error";
            }
        }
    } category;

    return category;
};

inline std::error_code make_error_code(future_errc e)
{
    return { static_cast<int>(e), future_category() };
}

class future_error : public std::system_error
{
  public:
    using std::system_error::system_error;
};

namespace detail
{
struct bilist_node
{
    bilist_node* next_{ this };
    bilist_node* prev_{ this };

    bilist_node() noexcept = default;

    bilist_node(bilist_node const&) = delete;

    bilist_node(bilist_node&& lhs) noexcept
        : next_(lhs.next_)
        , prev_(lhs.prev_)
    {
        lhs.next_ = &lhs;
        lhs.prev_ = &lhs;

        if (next_ == &lhs)
            next_ = this;
        else
            next_->prev_ = this;

        if (prev_ == &lhs)
            prev_ = this;
        else
            prev_->next_ = this;
    }

    void unlink() const noexcept
    {
        next_->prev_ = prev_;
        prev_->next_ = next_;
    }

    void link_before(bilist_node* next) noexcept
    {
        next_        = next;
        prev_        = next->prev_;
        prev_->next_ = this;
        next->prev_  = this;
    }
};

class service_member : public bilist_node
{
  public:
    virtual void shutdown() noexcept = 0;
    virtual ~service_member()        = default;
};

template<bool CC>
class locking_strategy
{
    struct null_mutex
    {
        void lock()
        {
        }

        void unlock() noexcept
        {
        }

        bool try_lock()
        {
            return true;
        }
    };

    std::conditional_t<CC, std::mutex, null_mutex> mutex_;

  public:
    auto internal_lock() noexcept
    {
        return std::lock_guard{ mutex_ };
    }
};

template<bool CC>
class service final
    : public locking_strategy<CC>
    , public net::detail::execution_context_service_base<service<CC>>
{
    bilist_node entries_;

  public:
    explicit service(net::execution_context& ctx) noexcept
        : net::detail::execution_context_service_base<service>(ctx)
    {
    }

    void register_queue(bilist_node* sm) noexcept
    {
        auto lg = this->internal_lock();
        sm->link_before(&entries_);
    }

    void unregister_queue(bilist_node* sm) noexcept
    {
        auto lg = this->internal_lock();
        sm->unlink();
    }

    void shutdown() noexcept override
    {
        auto e   = std::move(entries_);
        auto* nx = e.next_;
        while (nx != &e)
        {
            auto* nnx = nx->next_;
            static_cast<service_member*>(nx)->shutdown();
            e.next_ = nx = nnx;
        }
    }
};

struct wait_op : bilist_node
{
    virtual void shutdown() noexcept  = 0;
    virtual void complete(error_code) = 0;
    virtual ~wait_op()                = default;
};

template<class Executor, class Handler>
class wait_op_model final : public wait_op
{
    net::executor_work_guard<Executor> work_guard_;
    Handler handler_;

  public:
    wait_op_model(Executor e, Handler handler)
        : work_guard_(std::move(e))
        , handler_(std::move(handler))
    {
    }

    [[nodiscard]] auto get_cancellation_slot() const noexcept
    {
        return net::get_associated_cancellation_slot(handler_);
    }

    static wait_op_model* construct(Executor e, Handler handler)
    {
        auto halloc = net::get_associated_allocator(handler);
        auto alloc  = typename std::allocator_traits<
            decltype(halloc)>::template rebind_alloc<wait_op_model>(halloc);
        using traits = std::allocator_traits<decltype(alloc)>;
        auto pmem    = traits::allocate(alloc, 1);

        try
        {
            return new (pmem) wait_op_model(std::move(e), std::move(handler));
        }
        catch (...)
        {
            traits::deallocate(alloc, pmem, 1);
            throw;
        }
    }

    static void destroy(
        wait_op_model* self,
        net::associated_allocator_t<Handler> halloc)
    {
        auto alloc = typename std::allocator_traits<
            decltype(halloc)>::template rebind_alloc<wait_op_model>(halloc);
        self->~wait_op_model();
        auto traits = std::allocator_traits<decltype(alloc)>();
        traits.deallocate(alloc, self, 1);
    }

    void complete(error_code ec) override
    {
        get_cancellation_slot().clear();
        auto g = std::move(work_guard_);
        auto h = std::move(handler_);
        this->unlink();
        destroy(this, net::get_associated_allocator(h));
        net::post(g.get_executor(), net::append(std::move(h), ec));
    }

    void shutdown() noexcept override
    {
        get_cancellation_slot().clear();
        this->unlink();
        destroy(this, net::get_associated_allocator(this->handler_));
    }
};

template<typename T, typename Executor, bool CC>
class shared_state final
    : public locking_strategy<CC>
    , public service_member
{
    Executor executor_;
    service<CC>* service_;
    bilist_node waiters_;
    std::variant<
        std::monostate,
        std::exception_ptr,
        std::conditional_t<std::is_void_v<T>, std::monostate, T>>
        value_;

  public:
    explicit shared_state(Executor executor)
        : executor_{ std::move(executor) }
        , service_{ &net::use_service<service<CC>>(
              { net::query(executor_, net::execution::context) }) }
    {
        service_->register_queue(this);
    }

    shared_state(const shared_state&)            = delete;
    shared_state& operator=(const shared_state&) = delete;

    shared_state(shared_state&&)            = delete;
    shared_state& operator=(shared_state&&) = delete;

    [[nodiscard]] Executor get_executor() const
    {
        return executor_;
    }

    [[nodiscard]] bool is_ready() const noexcept
    {
        return value_.index() != 0;
    }

    template<typename... Args>
    void set_value(Args&&... args)
    {
        if (is_ready())
            throw future_error{ future_errc::promise_already_satisfied };

        value_.template emplace<2>(std::forward<Args>(args)...);

        complete_all({});
    }

    void set_exception(std::exception_ptr exception_ptr)
    {
        if (is_ready())
            throw future_error{ future_errc::promise_already_satisfied };

        value_.template emplace<1>(exception_ptr);

        complete_all({});
    }

    decltype(auto) get()
    {
        if (!is_ready())
            throw future_error{ future_errc::unready_future };

        if (auto* p = std::get_if<2>(&value_))
        {
            if constexpr (std::is_same_v<T, void>)
            {
                return;
            }
            else
            {
                return *p;
            }
        }

        std::rethrow_exception(std::get<1>(value_));
    }

    decltype(auto) get() const
    {
        if (!is_ready())
            throw future_error{ future_errc::unready_future };

        if (auto* p = std::get_if<2>(&value_))
        {
            if constexpr (std::is_same_v<T, void>)
            {
                return;
            }
            else
            {
                return std::as_const(*p);
            }
        }

        std::rethrow_exception(std::get<1>(value_));
    }

    template<typename CompletionToken>
    auto async_wait(CompletionToken&& token)
    {
        return net::async_initiate<decltype(token), void(error_code)>(
            [this](auto handler)
            {
                auto exec = get_associated_executor(handler, executor_);

                auto lg = this->internal_lock();

                if (is_ready())
                    return net::post(
                        std::move(exec),
                        net::append(std::move(handler), error_code{}));

                using handler_type = std::decay_t<decltype(handler)>;
                using model_type  = wait_op_model<decltype(exec), handler_type>;
                model_type* model = model_type ::construct(
                    std::move(exec), std::forward<decltype(handler)>(handler));
                auto c_slot = model->get_cancellation_slot();
                if (c_slot.is_connected())
                {
                    c_slot.assign(
                        [this, c_slot, model](net::cancellation_type type)
                        {
                            if (type != net::cancellation_type::none)
                            {
                                auto lg = this->internal_lock();
                                // already completed
                                if (!c_slot.is_connected())
                                    return;
                                model->complete(net::error::operation_aborted);
                            }
                        });
                }
                model->link_before(&waiters_);
            },
            token);
    }

    void complete_all(error_code ec)
    {
        auto& nx = waiters_.next_;
        while (nx != &waiters_)
            static_cast<wait_op*>(nx)->complete(ec);
    }

    ~shared_state() override
    {
        if (service_)
            service_->unregister_queue(this);
    }

  private:
    void shutdown() noexcept override
    {
        service_ = nullptr;
        auto& nx = waiters_.next_;
        while (nx != &waiters_)
            static_cast<wait_op*>(nx)->shutdown();
    }
};

/// Provides shared access to the result of asynchronous operations
/**
 * The class template shared_future provides a mechanism to access the
 * result of asynchronous operations, similar to future, except multiple
 * instances of shared_future are allowed to wait for the same shared state.
 *
 * Unlike future, which is only moveable (so only one instance can refer to any
 * particular asynchronous result), shared_future is copyable and multiple
 * shared future objects may refer to the same shared state.
 *
 * @tparam T The type of the result of operations which can be void too.
 *
 * @tparam Executor The type of executor associated with the object.
 *
 * @tparam CC a bool indicating whether the shared state is used in concurrent
 * environment or not.
 */
template<typename T, typename Executor, bool CC>
class shared_future
{
    std::shared_ptr<shared_state<T, Executor, CC>> shared_state_;

  public:
    /// Default constructor.
    /**
     * This constructor creates a shared_future with an empty shared state.
     */
    shared_future() = default;

    /// Constructor.
    /**
     * This constructor creates a shared_future from a shared state. this
     * constructor is only used in the share() function on future.
     *
     * @param shared_state a shared pointer to the shared state.
     */
    explicit shared_future(
        std::shared_ptr<shared_state<T, Executor, CC>> shared_state) noexcept
        : shared_state_{ std::move(shared_state) }
    {
    }

    /// Gets the executor associated with the object.
    /**
     * @returns the executor associated with the object.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    [[nodiscard]] Executor get_executor() const
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        return shared_state_->get_executor();
    }

    /// Checks if the future contains a shared state.
    /**
     * This function checks if the future contains a shared state, a default
     * constructed, moved-from and a future that is converted to a shared_future
     * does not contain a shared state.
     *
     * @returns true if the future contains a shared state, otherwise false.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
        return !!shared_state_;
    }

    /// Checks if the result is ready.
    /**
     * This function checks if the promise side set a result or an exception in
     * the shared state.
     *
     * @returns true if the result is ready, otherwise false.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if shared_future
     * does not contain a shared state.
     */
    [[nodiscard]] bool is_ready() const
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        auto lg = shared_state_->internal_lock();

        return shared_state_->is_ready();
    }

    /// Returns the result.
    /**
     * This function returns the result or throws the exception that the promise
     * side set.
     *
     * @returns const T& or void if T is void
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if shared_future
     * does not contain a shared state.
     *
     * @throws `future_error{ future_errc::unready_future }` Thrown on if
     * promise side has not set a value or an exception yet.
     *
     * @throws stored exception if the promise side set an exception.
     */
    decltype(auto) get() const
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        auto lg = shared_state_->internal_lock();

        return std::as_const(*shared_state_).get();
    }

    /// Starts an asynchronous wait on the shared_future.
    /**
     * This function initiates an asynchronous wait against the shared_future
     * and always returns immediately.
     *
     * For each call to async_wait(), the completion handler will be called
     * exactly once. The completion handler will be called when:
     *
     * @li The promise sets a value.
     *
     * @li The promise sets an exception.
     *
     * @li The promise goes out of scope and an exception of `future_error{
     * future_errc::broken_promise }` set automatically.
     *
     * @li The shared_future was canceled, in which case the handler is passed
     * the error code net::error::operation_aborted.
     *
     * @param token The completion_token that will be used to produce a
     * completion handler.
     */
    template<typename CompletionToken>
    auto async_wait(CompletionToken&& token)
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        return shared_state_->async_wait(std::forward<CompletionToken>(token));
    }
};

/// Provides a mechanism to access the result of asynchronous operations
/**
 * The class template future provides a mechanism to access the result of
 * asynchronous operations.
 *
 * Unlike shared_future, future gives a mutable reference to the result because
 * it is guaranteed we have only one future instance in the code.
 *
 * @tparam T The type of the result of operations which can be void too.
 *
 * @tparam Executor The type of executor associated with the object.
 *
 * @tparam CC a bool indicating whether the shared state is used in concurrent
 * environment or not.
 */
template<typename T, typename Executor, bool CC>
class future
{
    std::shared_ptr<shared_state<T, Executor, CC>> shared_state_;

  public:
    /// Default constructor.
    /**
     * This constructor creates a future with an empty shared state.
     */
    future() = default;

    /// Constructor.
    /**
     * This constructor creates a future from a shared state. this constructor
     * is only called in the get_future() function in the promise.
     *
     * @param shared_state a shared pointer to the shared state.
     */
    explicit future(
        std::shared_ptr<shared_state<T, Executor, CC>> shared_state) noexcept
        : shared_state_{ std::move(shared_state) }
    {
    }

    future(const future&)            = delete;
    future& operator=(const future&) = delete;

    future(future&&) noexcept            = default;
    future& operator=(future&&) noexcept = default;

    /// Gets the executor associated with the object.
    /**
     * @returns the executor associated with the object.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    [[nodiscard]] Executor get_executor() const
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        return shared_state_->get_executor();
    }

    /// Checks if the future contains a shared state.
    /**
     * This function checks if the future contains a shared state, a default
     * constructed, moved-from and a future that is converted to a shared_future
     * does not contain a shared state.
     *
     * @returns true if the future contains a shared state, otherwise false.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
        return !!shared_state_;
    }

    /// Checks if the result is ready.
    /**
     * This function checks if the promise side set a result or an exception in
     * the shared state.
     *
     * @returns true if the result is ready, otherwise false.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    [[nodiscard]] bool is_ready() const
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        auto lg = shared_state_->internal_lock();

        return shared_state_->is_ready();
    }

    /// Returns the result.
    /**
     * This function returns the result or throws the exception that the promise
     * side set.
     *
     * @returns T& or void if T is void
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     *
     * @throws `future_error{ future_errc::unready_future }` Thrown on if
     * promise side has not set a value or an exception yet.
     *
     * @throws stored exception if the promise side set an exception.
     */
    decltype(auto) get()
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        auto lg = shared_state_->internal_lock();

        return shared_state_->get();
    }

    /// Creates a shared_future.
    /**
     * This function transfers the shared state of the future to an instance of
     * shared_future and leaves the future with an empty shared state
     * (is_valid() on future returns false after this function).
     *
     * @returns an instance of shared_future.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    [[nodiscard]] shared_future<T, Executor, CC> share()
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        return shared_future<T, Executor, CC>{ std::move(shared_state_) };
    }

    /// Starts an asynchronous wait on the future.
    /**
     * This function initiates an asynchronous wait against the future and
     * always returns immediately.
     *
     * For each call to async_wait(), the completion handler will be called
     * exactly once. The completion handler will be called when:
     *
     * @li The promise sets a value.
     *
     * @li The promise sets an exception.
     *
     * @li The promise goes out of scope and an exception of `future_error{
     * future_errc::broken_promise }` set automatically.
     *
     * @li The future was canceled, in which case the handler is passed the
     * error code net::error::operation_aborted.
     *
     * @param token The completion_token that will be used to produce a
     * completion handler.
     */
    template<typename CompletionToken>
    auto async_wait(CompletionToken&& token)
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        return shared_state_->async_wait(std::forward<CompletionToken>(token));
    }
};

/// Provides a mechanism to store a value or an exception as a result of an
/// asynchronous operations
/**
 * The class template promise provides a mechanism to store a value or an
 * exception as a result of asynchronous operations.
 *
 * A future pair can be retrieved by calling the member function get_future(),
 * after the call, both objects share the same shared state.
 *
 * If an unset promise with a retrieved future drops out of scope, an exception
 * of `future_error{ future_errc::broken_promise }` will be set in the shared
 * state and signal all the waiting futures.
 *
 * @tparam T The type of the result of operations which can be void too.
 *
 * @tparam Executor The type of executor associated with the object.
 *
 * @tparam CC a bool indicating whether the shared state is used in concurrent
 * environment or not.
 */
template<typename T, typename Executor, bool CC>
class promise
{
    std::shared_ptr<shared_state<T, Executor, CC>> shared_state_;
    bool retrieved_{ false };

  public:
    /// Default constructor.
    /**
     * This constructor creates a promise with an empty shared state.
     */
    promise() = default;

    /// Constructor.
    /**
     * @param executor The I/O executor that the future will use, by default, to
     * dispatch handlers for any asynchronous operations performed on the timer.
     */
    explicit promise(Executor executor)
        : shared_state_{ std::make_shared<shared_state<T, Executor, CC>>(
              std::move(executor)) }
    {
    }

    /// Constructor.
    /**
     * @param context An execution context that provides the I/O executor that
     * the future will use, by default, to dispatch handlers for any
     * asynchronous operations performed on the future.
     */
    template<
        typename ExecutionContext,
        typename std::enable_if_t<std::is_convertible_v<
            ExecutionContext&,
            net::execution_context&>>* = nullptr>
    explicit promise(ExecutionContext& context)
        : promise{ context.get_executor() }
    {
    }

    /// Constructor.
    /**
     * @param executor The I/O executor that the future will use, by default, to
     * dispatch handlers for any asynchronous operations performed on the timer.
     *
     * @param alloc The allocator will be used for allocating shared state
     * between promise and future
     */
    template<typename Alloc>
    promise(Executor executor, const Alloc& alloc)
        : shared_state_{
            std::allocate_shared<shared_state<T, Executor, CC>, Alloc>(
                alloc,
                std::move(executor))
        }
    {
    }

    /// Constructor.
    /**
     * @param context An execution context that provides the I/O executor that
     * the future will use, by default, to dispatch handlers for any
     * asynchronous operations performed on the future.
     *
     * @param alloc The allocator will be used for allocating shared state
     * between promise and future
     */
    template<
        typename ExecutionContext,
        typename Alloc,
        typename std::enable_if_t<std::is_convertible_v<
            ExecutionContext&,
            net::execution_context&>>* = nullptr>
    promise(ExecutionContext& context, const Alloc& alloc)
        : promise{ context.get_executor(), alloc }
    {
    }

    promise(const promise&)            = delete;
    promise& operator=(const promise&) = delete;

    promise(promise&&) noexcept            = default;
    promise& operator=(promise&&) noexcept = default;

    /// Gets the executor associated with the object.
    /**
     * @returns the executor associated with the object.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if promise does
     * not contain a shared state.
     */
    [[nodiscard]] Executor get_executor() const
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        return shared_state_->get_executor();
    }

    /// Checks if the promise contains a shared state.
    /**
     * This function checks if the promise contains a shared state, a default
     * constructed, and a moved-from promise does not contain a shared state.
     *
     * @returns true if the promise contains a shared state, otherwise false.
     */
    [[nodiscard]] bool is_valid() const noexcept
    {
        return !!shared_state_;
    }

    /// Sets a value in the shared state.
    /**
     * This function sets a value to be used by the future and signals all the
     * waiting for handlers on the future side.
     *
     * @throws `future_error{ future_errc::promise_already_satisfied }` Thrown
     * if a value or an exception has already been set.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    template<typename... Args>
    void set_value(Args&&... args)
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        auto lg = shared_state_->internal_lock();

        shared_state_->set_value(std::forward<Args>(args)...);
    }

    /// Sets an exception in the shared state.
    /**
     * This function sets an exception to be used by the future and signals all
     * the waiting handlers on the future side.
     *
     * @throws `future_error{ future_errc::promise_already_satisfied }` Thrown
     * if a value or an exception has already been set.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    void set_exception(std::exception_ptr exception_ptr)
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        auto lg = shared_state_->internal_lock();

        shared_state_->set_exception(exception_ptr);
    }

    /// Creates a future.
    /**
     * This function creates a future associated with the same shared state.
     *
     * @returns an instance of the future.
     *
     * @throws `future_error{ future_errc::future_already_retrieved }` Thrown if
     * get_future() has already been called.
     *
     * @throws `future_error{ future_errc::no_state }` Thrown if future does not
     * contain a shared state.
     */
    [[nodiscard]] future<T, Executor, CC> get_future()
    {
        if (!shared_state_)
            throw future_error{ future_errc::no_state };

        if (std::exchange(retrieved_, true))
            throw future_error{ future_errc::future_already_retrieved };

        return future<T, Executor, CC>{ shared_state_ };
    }

    /// Destroys the promise.
    /**
     * This function destroys the promise if no value or exception is set an
     * exception of `future_error{ future_errc::broken_promise }` will be set in
     * the shared state and signal all the waiting futures.
     */
    ~promise()
    {
        if (shared_state_ && retrieved_)
        {
            auto lg = shared_state_->internal_lock();
            if (!shared_state_->is_ready())
                shared_state_->set_exception(std::make_exception_ptr(
                    future_error{ future_errc::broken_promise }));
        }
    }
};
} // namespace detail

template<typename T, typename Executor = net::any_io_executor>
using cc_future = detail::future<T, Executor, true>;

template<typename T, typename Executor = net::any_io_executor>
using cc_shared_future = detail::shared_future<T, Executor, true>;

template<typename T, typename Executor = net::any_io_executor>
using cc_promise = detail::promise<T, Executor, true>;

template<typename T, typename Executor = net::any_io_executor>
using future = detail::future<T, Executor, false>;

template<typename T, typename Executor = net::any_io_executor>
using shared_future = detail::shared_future<T, Executor, false>;

template<typename T, typename Executor = net::any_io_executor>
using promise = detail::promise<T, Executor, false>;

} // namespace saf