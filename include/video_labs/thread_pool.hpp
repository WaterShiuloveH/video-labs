#pragma once

#include <cstddef>
#include <stdexcept>
#include <thread>
#include <vector>

namespace video_labs {

class ThreadPool {
public:
    explicit ThreadPool(std::size_t worker_count)
        : m_worker_count_(worker_count)
    {
        if (worker_count == 0) {
            throw std::invalid_argument("ThreadPool worker_count must be positive");
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool()
    {
        join();
    }

    template <typename WorkerFunction>
    void start(WorkerFunction worker_function)
    {
        if (!m_threads_.empty()) {
            throw std::logic_error("ThreadPool workers have already been started");
        }

        m_threads_.reserve(m_worker_count_);
        for (std::size_t index = 0; index < m_worker_count_; ++index) {
            m_threads_.emplace_back(worker_function, index);
        }
    }

    void join()
    {
        for (auto& thread : m_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    std::size_t size() const
    {
        return m_worker_count_;
    }

private:
    std::size_t m_worker_count_;
    std::vector<std::thread> m_threads_;
};

} // namespace video_labs
