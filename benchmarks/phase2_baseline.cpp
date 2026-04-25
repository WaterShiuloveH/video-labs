#include "video_labs/benchmark_timer.hpp"
#include "video_labs/thread_safe_queue.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <thread>

namespace {

constexpr int poison_pill = -1;
constexpr int default_item_count = 1'000'000;

int parse_item_count(int argc, char* argv[])
{
    if (argc == 1) {
        return default_item_count;
    }

    if (argc == 3 && std::string_view(argv[1]) == "--items") {
        const int item_count = std::atoi(argv[2]);
        if (item_count > 0) {
            return item_count;
        }
    }

    std::cerr << "Usage: " << argv[0] << " [--items positive_integer]\n";
    return -1;
}

} // namespace

int main(int argc, char* argv[])
{
    const int item_count = parse_item_count(argc, argv);
    if (item_count <= 0) {
        return 1;
    }

    video_labs::ThreadSafeQueue<int> queue;
    int consumed_count = 0;
    long long checksum = 0;

    video_labs::BenchmarkTimer timer;

    std::thread producer([&queue, item_count] {
        for (int value = 0; value < item_count; ++value) {
            queue.push(value);
        }
        queue.push(poison_pill);
    });

    std::thread consumer([&queue, &consumed_count, &checksum] {
        while (true) {
            const int value = queue.pop();
            if (value == poison_pill) {
                break;
            }

            ++consumed_count;
            checksum += value;
        }
    });

    producer.join();
    consumer.join();

    const double elapsed_seconds = timer.elapsed_seconds();
    const double throughput = static_cast<double>(consumed_count) / elapsed_seconds;
    const long long expected_checksum =
        (static_cast<long long>(item_count - 1) * item_count) / 2;

    std::cout << "phase=2_baseline\n";
    std::cout << "producers=1\n";
    std::cout << "consumers=1\n";
    std::cout << "items=" << item_count << '\n';
    std::cout << "consumed=" << consumed_count << '\n';
    std::cout << "elapsed_seconds=" << elapsed_seconds << '\n';
    std::cout << "throughput_items_per_second=" << throughput << '\n';
    std::cout << "checksum=" << checksum << '\n';

    if (consumed_count != item_count || checksum != expected_checksum) {
        std::cerr << "Benchmark validation failed\n";
        return 1;
    }

    return 0;
}
