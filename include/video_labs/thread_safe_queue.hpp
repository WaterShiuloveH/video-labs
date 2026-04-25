#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

namespace video_labs {

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void push(T value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex_);
            m_queue_.push(std::move(value));
        }
        m_condition_.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex_);
        m_condition_.wait(lock, [this] { return !m_queue_.empty(); });

        T value = std::move(m_queue_.front());
        m_queue_.pop();
        return value;
    }

    std::optional<T> try_pop()
    {
        std::lock_guard<std::mutex> lock(m_mutex_);
        if (m_queue_.empty()) {
            return std::nullopt;
        }

        T value = std::move(m_queue_.front());
        m_queue_.pop();
        return value;
    }

    bool is_empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex_);
        return m_queue_.empty();
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex_);
        return m_queue_.size();
    }

private:
    mutable std::mutex m_mutex_;
    std::condition_variable m_condition_;
    std::queue<T> m_queue_;
};

} // namespace video_labs
