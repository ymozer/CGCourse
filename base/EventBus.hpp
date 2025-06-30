#pragma once

#include <functional>
#include <map>
#include <vector>
#include <memory>
#include <typeindex>
#include <mutex>
#include <atomic>
#include <future>
#include <iterator>

#include "Log.hpp"
#include "ThreadPool.hpp"
#include "EventTypes.hpp"

namespace Base
{
    enum class EventPriority
    {
        High,
        Normal,
        Low
    };

    struct Event;

    using EventHandler = std::function<void(Event &)>;

    struct HandlerGroup
    {
        int groupId;
        EventHandler handler;
        EventPriority priority;
    };

    struct SubscriptionHandle
    {
        SubscriptionHandle() : eventType(typeid(void)), id(0) {}
        SubscriptionHandle(std::type_index type, uint64_t subId)
            : eventType(type), id(subId) {}
        std::type_index eventType;
        uint64_t id = 0;

        bool operator<(const SubscriptionHandle &other) const
        {
            if (eventType != other.eventType)
                return eventType < other.eventType;
            return id < other.id;
        }
    };

    class ParallelEventBus
    {
    public:
        ParallelEventBus(size_t numThreads)
            : m_ThreadPool(numThreads), m_NextSubscriptionId(1)
        {
            m_ThreadPool.Start();
        }
        ~ParallelEventBus() = default;

        ParallelEventBus(const ParallelEventBus &) = delete;
        ParallelEventBus &operator=(const ParallelEventBus &) = delete;
        ParallelEventBus(ParallelEventBus &&) = delete;
        ParallelEventBus &operator=(ParallelEventBus &&) = delete;

        // Subscribe now returns a handle
        template <typename EventType>
        SubscriptionHandle subscribe(std::function<void(EventType &)> handler)
        {
            std::lock_guard<std::mutex> lock(m_HandlersMutex);

            // This lambda erases the type, allowing us to store it in a common container
            auto typedHandler = [handler = std::move(handler)](Event &e)
            {
                handler(static_cast<EventType &>(e));
            };

            const auto eventType = std::type_index(typeid(EventType));
            const uint64_t id = m_NextSubscriptionId++;
            m_Handlers[eventType].emplace(id, std::move(typedHandler));

            return {eventType, id};
        }

        void unsubscribe(SubscriptionHandle handle)
        {
            std::lock_guard<std::mutex> lock(m_HandlersMutex);
            if (auto it = m_Handlers.find(handle.eventType); it != m_Handlers.end())
            {
                it->second.erase(handle.id);
            }
        }

        // Synchronous dispatch for events that must be handled on the main thread
        template <typename EventType>
        void dispatch(EventType &event)
        {
            auto handlersCopy = getHandlersForEvent<EventType>();
            for (const auto &[id, handler] : handlersCopy)
            {
                if (event.handled.load())
                    break; // Use atomic load
                handler(event);
            }
        }

        template <typename EventType>
        void dispatchAsync(std::shared_ptr<EventType> event)
        {
            auto handlersCopy = getHandlersForEvent<EventType>();

            for (const auto &[id, handler] : handlersCopy)
            {
                m_ThreadPool.Enqueue([handler, event]()
                                     {
                    // This atomic exchange is the key to fixing the race condition.
                    // It atomically sets 'handled' to true and returns its *previous* value.
                    // Only the first thread to call this on the event will get 'false' back.
                    bool alreadyHandled = event->handled.exchange(true, std::memory_order_acq_rel);
                    if (!alreadyHandled)
                    {
                        handler(*event);
                    } });
            }
        }

        template <typename EventType>
        void dispatchAsync(EventType &&event)
        {
            dispatchAsync(std::make_shared<EventType>(std::move(event)));
        }

        ThreadPool &getThreadPool() { return m_ThreadPool; }

    private:
        template <typename EventType>
        std::map<uint64_t, EventHandler> getHandlersForEvent()
        {
            std::lock_guard<std::mutex> lock(m_HandlersMutex);
            auto it = m_Handlers.find(typeid(EventType));
            if (it != m_Handlers.end())
            {
                return it->second;
            }
            return {};
        }
        std::map<std::type_index, std::map<uint64_t, EventHandler>> m_Handlers;
        mutable std::mutex m_HandlersMutex;
        std::atomic<uint64_t> m_NextSubscriptionId;
        ThreadPool m_ThreadPool;
    };
}