#pragma once

#include <chrono>

namespace video_labs {

class BenchmarkTimer {
public:
    using clock = std::chrono::steady_clock;

    BenchmarkTimer()
        : m_start_(clock::now())
    {
    }

    void reset()
    {
        m_start_ = clock::now();
    }

    double elapsed_seconds() const
    {
        const auto elapsed = clock::now() - m_start_;
        return std::chrono::duration<double>(elapsed).count();
    }

private:
    clock::time_point m_start_;
};

} // namespace video_labs
