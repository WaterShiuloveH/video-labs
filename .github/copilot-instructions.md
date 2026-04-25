---
name: video-labs
description: "C++ concurrent video processing pipeline project. Use when working on: thread-safe queue implementation (Phase 1), performance benchmarking (Phase 2), FFmpeg video decoding (Phase 3). Reference PROJECT_PLAN.md for implementation roadmap."
---

# Video-Labs: C++ Concurrent Video Processing Pipeline

## Project Overview

**Goal**: Build a production-grade concurrent video processing system in C++ across three phases:

1. Thread-safe queue with producer-consumer pattern
2. Multi-threaded performance optimization with benchmarks
3. FFmpeg integration for real video decoding

**Key Technologies**: C++17/20, std::thread, FFmpeg (libavformat, libavcodec)

## Project Structure

```
video-labs/
├── PROJECT_PLAN.md          # Phase-by-phase implementation roadmap
├── CMakeLists.txt           # Build configuration
├── src/
│   ├── main.cpp
│   ├── queue/               # ThreadSafeQueue implementation
│   ├── producer/            # Producer implementations
│   ├── consumer/            # Consumer and ThreadPool
│   └── video/               # FFmpeg integration (Phase 3)
├── include/                 # Header files
├── test/                    # Unit tests
├── build/                   # CMake build output
└── benchmarks/              # Performance test results
```

## Implementation Phases

### Phase 1: Basic Pipeline (ThreadSafeQueue + Poison Pill)

- **Status**: Starting point
- **Scope**: Thread safety fundamentals, single producer/consumer
- **Key Features**: ThreadSafeQueue<T>, mutex-based locking, Poison Pill pattern
- **Success Criteria**: Zero data races (ThreadSanitizer clean), graceful shutdown

### Phase 2: Performance Measurement (Thread Pool + Benchmarks)

- **Status**: Follows Phase 1
- **Scope**: Multi-consumer thread pool, high-resolution timing
- **Key Features**: ThreadPool class, std::chrono benchmarking, throughput/latency metrics
- **Success Criteria**: 4x+ speedup with 4 threads, comprehensive benchmark data

### Phase 3: Real Video Decoding (FFmpeg Integration)

- **Status**: Final phase
- **Scope**: Production video processing
- **Key Features**: FFmpeg wrapper, AVPacket/AVFrame handling, colorspace conversion
- **Success Criteria**: Decode real .mp4 files, handle multiple codecs

## Conventions & Patterns

### Coding Style

- C++17 minimum (use auto, range-based for, smart pointers)
- Member variables: `m_name_` for private, `name` for public
- Classes: PascalCase, functions: snake_case
- Thread-safe code: Always document synchronization assumptions

### Thread Safety

- Always use RAII (lock_guard, unique_lock) for mutex management
- Avoid nested locks to prevent deadlock
- Document shared state explicitly in class headers
- Use ThreadSanitizer during development: `cmake -DSANITIZE_THREAD=ON`

### Benchmarking

- Use `std::chrono::high_resolution_clock` for timing
- Measure: throughput (items/sec), latency (P50, P99, max)
- Run multiple iterations to reduce variance
- Log results to CSV for analysis

### FFmpeg Guidelines (Phase 3)

- Always call `av_frame_free()` and `av_packet_free()` for cleanup
- Check return codes; FFmpeg uses negative values for errors
- Use error string via `av_err2str()` for debugging
- Profile codec context creation (expensive) and cache when possible

## Build & Test Commands

```bash
# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Build with thread sanitizer
cmake -B build -DSANITIZE_THREAD=ON
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Run benchmarks (Phase 2+)
./build/video-labs-benchmark --threads=4 --items=1000000

# Decode video (Phase 3)
./build/video-labs-decode input.mp4 output_frame_%04d.png
```

## Common Patterns

### ThreadSafeQueue API

```cpp
template<typename T>
class ThreadSafeQueue {
    void push(T value);           // Block-safe insertion
    std::optional<T> try_pop();   // Non-blocking pop
    T pop();                      // Blocking pop until available
    size_t size() const;          // Atomic size check
    bool is_empty() const;        // Atomic empty check
};
```

### Producer-Consumer

- Producer: Calls `queue.push(item)` in loop, sends Poison Pill to signal EOF
- Consumer: Calls `queue.pop()`, checks for Poison Pill, processes items
- Thread Pool: Multiple consumers over same queue

### Benchmarking Template

```cpp
auto start = std::chrono::high_resolution_clock::now();
// ... work to measure ...
auto end = std::chrono::high_resolution_clock::now();
auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

## Debugging Tips

1. **Data Race Issues**: Run with `-fsanitize=thread` and check output
2. **Deadlock Suspicion**: Add logging to mutex acquire/release points
3. **Performance Issues**: Profile with `perf` or `Instruments.app` on macOS
4. **FFmpeg Errors**: Always check return codes and call `av_err2str(errno)`
5. **Memory Leaks**: Run with `-fsanitize=address` or Valgrind

## References

- [PROJECT_PLAN.md](./PROJECT_PLAN.md) — Detailed phase breakdown
- C++ Reference: https://en.cppreference.com/w/cpp/thread
- FFmpeg Wiki: https://trac.ffmpeg.org/wiki/CompilationGuide
- Modern CMake: https://cmake.org/cmake/help/latest/

## When to Load This

This workspace instruction auto-loads for all work in `video-labs/`. Key trigger phrases:

- "ThreadSafeQueue implementation"
- "Producer-consumer pattern"
- "Thread pool benchmarking"
- "FFmpeg video decoding"
- Any Phase 1/2/3 work in this project
