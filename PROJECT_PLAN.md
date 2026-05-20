# C++ Concurrent Video Processing Pipeline - Project Plan

## Project Overview

Build a production-grade C++ video processing system using concurrent programming, starting from thread-safe primitives to FFmpeg integration.

---

## Phase 1: Basic Pipeline Foundation 🎯

**Core Objective:** Establish a safe concurrent foundation with thread-safe queue primitives

**Implementation Details:**

- Implement `ThreadSafeQueue<T>` with lock-based synchronization
- Use `int` to simulate AVPacket objects
- Create a single producer and single consumer thread configuration
- Implement Poison Pill pattern for graceful shutdown

**Expected Results:**

- Memory-safe concurrent data structure
- Zero data races (verified with ThreadSanitizer)
- Deadlock-free shutdown mechanism via Poison Pill

### Phase 1 Steps

1. **Project Setup**
   - [x] Create C++ project folder structure (`src/`, `include/`, `build/`, `test/`)
   - [x] Set up CMakeLists.txt with C++17/20 standard
   - [x] Configure compiler flags for thread safety (e.g., `-fno-omit-frame-pointer` for debugging)

2. **ThreadSafeQueue Implementation**
   - [x] Design thread-safe queue template class with mutex and condition variables
   - [x] Implement `push()` with thread-safe insertion
   - [x] Implement `pop()` with blocking wait when empty
   - [x] Add `is_empty()` and `size()` inspection methods

3. **Poison Pill Pattern**
   - [x] Define sentinel value (e.g., -1) as Poison Pill
   - [x] Implement graceful shutdown trigger
   - [x] Ensure consumer stops cleanly upon Poison Pill reception

4. **Basic Producer-Consumer**
   - [x] Create `Producer` class that generates `int` values
   - [x] Create `Consumer` class that retrieves values
   - [x] Spawn one producer and one consumer thread
   - [x] Verify data integrity and order

5. **Testing & Validation**
   - [x] Write basic unit tests for queue operations
   - [x] Verify no data races with ThreadSanitizer (`-fsanitize=thread`)
   - [x] Run simple producer-consumer test
   - [x] Document results and observations

### Phase 1 Validation Notes

- `ctest --test-dir build --output-on-failure`: passed
- `ctest --test-dir build-tsan --output-on-failure`: passed with ThreadSanitizer enabled
- `./build/video_labs_phase1`: processed 10 simulated packets in order and exited cleanly

---

## Phase 2: Performance Measurement & Thread Pool 🛠️

**Core Objective:** Scale to multiple consumers with Thread Pool pattern and precise benchmarking

**Implementation Details:**

- Extend single consumer to multiple consumers (Thread Pool)
- Introduce `std::chrono` for steady timing
- Measure throughput and latency improvements
- Compare single-threaded vs multi-threaded performance

**Expected Results:**

- Benchmark data showing N-fold performance improvement
- Thread Pool implementation handling variable consumer counts
- Clear performance visualization (throughput, latency graphs)

### Phase 2 Steps

1. **Thread Pool Implementation**
   - [x] Design `ThreadPool` class with worker threads
   - [x] Implement dynamic worker thread management
   - [x] Add configurable pool size (e.g., 2, 4, 8, 16 workers)
   - [x] Refactor consumer logic for pool integration

2. **Benchmarking Infrastructure**
   - [x] Add `BenchmarkTimer` using `std::chrono::steady_clock`
   - [ ] Implement throughput measurement (items/second)
   - [ ] Implement latency measurement (P50, P99, max)
   - [ ] Add statistics aggregation (mean, stddev, histogram)

### Phase 2 Progress Notes

- Added `video_labs_phase2` as the single producer / single consumer baseline.
- `./build/video_labs_phase2 --items 100000`: processed 100000 items with checksum validation.
- Added `--producer-delay-us`, `--consumer-work`, and `--burst-size` benchmark controls.
- Added `--consumers` to compare one producer against multiple consumer threads.
- Added `ThreadPool` and refactored Phase 2 consumers to use it.

3. **Producer Enhancement**
   - [ ] Adjust producer rate to stress-test the queue
   - [x] Add configurable delay between productions
   - [x] Implement burst production mode

4. **Benchmark Suite**
   - [x] Create benchmarks: 1 producer vs 1-16 consumers
   - [ ] Run with different queue loads (100K, 1M, 10M items)
   - [ ] Measure CPU utilization and memory usage
   - [ ] Log results to CSV for analysis

5. **Analysis & Optimization**
   - [ ] Compare single-consumer vs thread pool performance
   - [ ] Identify bottlenecks (lock contention, cache misses)
   - [ ] Optimize based on profiling data
   - [ ] Generate performance report with charts

---

## Phase 3: Real Video Decoding Integration 📈

**Core Objective:** Integrate FFmpeg for production-grade video processing

**Implementation Details:**

- Read real `.mp4` video files using FFmpeg
- Producer extracts `AVPacket` objects from input stream
- Consumer decodes packets using `libavcodec` to raw pixels (YUV/RGB)
- Ensure industrial-grade pipeline stability

**Expected Results:**

- Fully functional C++ video processing system
- Support various video codecs and resolutions
- Demonstrate real-time or near-real-time performance
- Portfolio-ready system architecture

### Phase 3 Steps

1. **FFmpeg Integration Setup**
   - [ ] Install FFmpeg libraries (`libavformat`, `libavcodec`, `libavutil`)
   - [ ] Link FFmpeg in CMakeLists.txt
   - [ ] Write FFmpeg initialization and cleanup code

2. **Video Input Pipeline**
   - [ ] Implement `VideoReader` class using `AVFormatContext`
   - [ ] Open input `.mp4` file and find video stream
   - [ ] Create producer that extracts `AVPacket` objects
   - [ ] Handle stream information (resolution, fps, duration)

3. **Video Packet Producer**
   - [ ] Queue type adapts from `int` to `std::shared_ptr<AVPacket>`
   - [ ] Implement packet extraction loop with error handling
   - [ ] Add frame counting and timestamp tracking
   - [ ] Implement end-of-file detection

4. **Video Decoder Consumer**
   - [ ] Create `VideoDecoder` class using `AVCodecContext`
   - [ ] Implement `AVFrame` allocation and management
   - [ ] Decode packets to raw pixel data (YUV420P or RGB24)
   - [ ] Implement colorspace conversion pipeline

5. **Output Pipeline**
   - [ ] Option 1: Save decoded frames as images (PNG/PPM)
   - [ ] Option 2: Write decoded video to output file
   - [ ] Option 3: Stream frames for real-time processing
   - [ ] Add performance metrics (fps, processing time per frame)

6. **Quality Assurance**
   - [ ] Test with various video formats (.mp4, .mov, .mkv)
   - [ ] Handle edge cases (corrupted packets, codec switches)
   - [ ] Stress test with large/high-resolution videos
   - [ ] Verify output correctness (frame checksums)

7. **Documentation & Deliverables**
   - [ ] Create architecture documentation
   - [ ] Document API and usage examples
   - [ ] Performance benchmarks vs system ffmpeg
   - [ ] Create sample usage application
   - [ ] Add to portfolio with deployment notes

---

## Technology Stack

| Component    | Technology                                             |
| ------------ | ------------------------------------------------------ |
| Language     | C++17/20                                               |
| Build System | CMake 3.15+                                            |
| Threading    | `std::thread`, `std::mutex`, `std::condition_variable` |
| Video        | FFmpeg (libavformat, libavcodec, libavutil)            |
| Benchmarking | `std::chrono` + custom metrics                         |
| Testing      | Google Test (optional)                                 |
| Code Quality | ThreadSanitizer, AddressSanitizer                      |

## Common Commands

```bash
make build
make test
make run-phase1
make run-phase2 ITEMS=100000 WORK=100 CONSUMERS=4
make test-tsan
make run-tsan ITEMS=10000 WORK=10 CONSUMERS=4
make clean
make rebuild
make rebuild-tsan
make rebuild-all
```

---

## Success Criteria

- ✅ Phase 1: Zero data races, Poison Pill shutdown works
- ✅ Phase 2: Clear performance improvement with thread pool (target: 4x+ with 4 threads)
- ✅ Phase 3: Successfully decode real video files at reasonable performance

## Resume Highlights

- **Concurrent Systems Design**: Implemented thread-safe queue with producer-consumer pattern
- **Performance Engineering**: Benchmarked and optimized multi-threaded pipeline (achieved Nx speedup)
- **FFmpeg Integration**: Built industrial-grade video processing system with libavcodec
- **C++ Systems Programming**: Memory safety, synchronization primitives, and performance profiling
