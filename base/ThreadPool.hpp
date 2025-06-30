#pragma once

#include <queue>
#include <mutex>
#include <vector>
#include <future>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>

namespace Base
{
#ifndef PLATFORM_EMSCRIPTEN
    class ThreadPool
    {
    public:
        ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
        ~ThreadPool();

        template <class F, class... Args>
        auto Enqueue(F &&f, Args &&...args)
            -> std::future<typename std::invoke_result<F, Args...>::type>;

        void Start();
        void Stop();

    private:
        void WorkerThread(size_t workerId);

        std::vector<std::thread> m_Workers;
        std::queue<std::function<void()>> m_Tasks;

        std::mutex m_QueueMutex;
        std::condition_variable m_Condition;
        std::atomic<bool> m_Stop = false;
        std::atomic<bool> m_Running = false;
    };

    template <class F, class... Args>
    auto ThreadPool::Enqueue(F &&f, Args &&...args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);

            if (m_Stop)
            {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }

            m_Tasks.emplace([task]()
                            { (*task)(); });
        }

        m_Condition.notify_one();
        return res;
    }
#else
#include <functional>
#include <future>
#include <memory>
    // A "fake" ThreadPool that doesn't use threads.
    // It executes tasks immediately on the calling thread.
    class ThreadPool
    {
    public:
        // Constructor and Start/Stop do nothing.
        ThreadPool(size_t /*numThreads*/) {}
        ~ThreadPool() = default;
        void Start() {}
        void Stop() {}

        // Enqueue simply executes the task immediately and returns a ready future.
        template <class F, class... Args>
        auto Enqueue(F &&f, Args &&...args)
            -> std::future<typename std::invoke_result<F, Args...>::type>
        {
            using return_type = typename std::invoke_result<F, Args...>::type;

            // Use a promise to create a future that we can set the value for.
            std::promise<return_type> promise;
            std::future<return_type> future = promise.get_future();

            // Execute the function immediately.
            try
            {
                // If the function returns a value, set it in the promise.
                if constexpr (!std::is_same_v<return_type, void>)
                {
                    promise.set_value(f(std::forward<Args>(args)...));
                }
                else // If the function is void, just call it and set the promise.
                {
                    f(std::forward<Args>(args)...);
                    promise.set_value();
                }
            }
            catch (...)
            {
                // If the function throws, capture the exception in the promise.
                promise.set_exception(std::current_exception());
            }

            return future;
        }
    };

#endif
}