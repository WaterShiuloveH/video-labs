#pragma once

#include "video_labs/thread_safe_queue.hpp"

namespace video_labs {

class Producer {
public:
    Producer(ThreadSafeQueue<int>& queue, int item_count, int poison_pill)
        : m_queue_(queue)
        , m_item_count_(item_count)
        , m_poison_pill_(poison_pill)
    {
    }

    void run()
    {
        for (int value = 0; value < m_item_count_; ++value) {
            m_queue_.push(value);
        }
        m_queue_.push(m_poison_pill_);
    }

private:
    ThreadSafeQueue<int>& m_queue_;
    int m_item_count_;
    int m_poison_pill_;
};

} // namespace video_labs
