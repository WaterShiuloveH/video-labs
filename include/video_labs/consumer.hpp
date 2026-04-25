#pragma once

#include "video_labs/thread_safe_queue.hpp"

#include <vector>

namespace video_labs {

class Consumer {
public:
    Consumer(ThreadSafeQueue<int>& queue, int poison_pill)
        : m_queue_(queue)
        , m_poison_pill_(poison_pill)
    {
    }

    void run()
    {
        while (true) {
            const int value = m_queue_.pop();
            if (value == m_poison_pill_) {
                break;
            }
            m_received_.push_back(value);
        }
    }

    const std::vector<int>& received() const
    {
        return m_received_;
    }

private:
    ThreadSafeQueue<int>& m_queue_;
    int m_poison_pill_;
    std::vector<int> m_received_;
};

} // namespace video_labs
