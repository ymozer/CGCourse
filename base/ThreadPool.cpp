#include "ThreadPool.hpp"
#include <Log.hpp>
#ifndef PLATFORM_EMSCRIPTEN
    #include <tracy/Tracy.hpp>
#endif

namespace Base
{
#ifndef PLATFORM_EMSCRIPTEN
    ThreadPool::ThreadPool(size_t numThreads)
    {
        if (numThreads == 0)
        {
            numThreads = 1;
        }
        LOG_INFO("ThreadPool Constructor with {} threads.", numThreads);
    }

    ThreadPool::~ThreadPool()
    {
        if (m_Running)
        {
            Stop();
        }
        LOG_INFO("ThreadPool Destructor.");
    }

    void ThreadPool::Start()
    {
        if (m_Running)
            return;

        LOG_INFO("Starting ThreadPool...");
        m_Stop = false;
        size_t numThreads = m_Workers.capacity();
        if (numThreads == 0)
            numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0)
            numThreads = 1;

        m_Workers.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i)
        {
            m_Workers.emplace_back([this, i]
                                   { this->WorkerThread(i); });
        }
        m_Running = true;
        LOG_INFO("ThreadPool started with {} threads.", numThreads);
    }

    void ThreadPool::Stop()
    {
        if (!m_Running)
            return;

        LOG_INFO("Stopping ThreadPool...");
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }

        m_Condition.notify_all();

        for (std::thread &worker : m_Workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
        m_Workers.clear();
        m_Running = false;
        LOG_INFO("ThreadPool stopped.");
    }

    void ThreadPool::WorkerThread(size_t workerId)
    {
        char threadNameBuffer[32];
        snprintf(threadNameBuffer, sizeof(threadNameBuffer), "Worker %zu", workerId);
        #ifndef PLATFORM_EMSCRIPTEN
        tracy::SetThreadName(threadNameBuffer);
        ZoneScopedN("ThreadPool::WorkerThread");
        #endif
        while (true)
        {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);

                m_Condition.wait(lock, [this]
                                 { return this->m_Stop || !this->m_Tasks.empty(); });

                if (this->m_Stop && this->m_Tasks.empty())
                {
                    return;
                }

                task = std::move(this->m_Tasks.front());
                this->m_Tasks.pop();
            }
            task();
        }
    }
#endif
}