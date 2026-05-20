#include "video_labs/benchmark_timer.hpp"
#include "video_labs/thread_pool.hpp"
#include "video_labs/thread_safe_queue.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <thread>
#include <vector>

namespace {

constexpr int poison_pill = -1;
constexpr int default_item_count = 1'000'000;

struct BenchmarkConfig {
    int item_count = default_item_count;
    int producer_delay_us = 0;
    int consumer_work = 0;
    int burst_size = 1;
    int consumer_count = 1;
};

struct ConsumerStats {
    int consumed_count = 0;
    long long checksum = 0;
    long long work_checksum = 0;
};

void print_usage(std::string_view program_name)
{
    std::cerr << "Usage: " << program_name
              << " [--items positive_integer]"
              << " [--producer-delay-us non_negative_integer]"
              << " [--consumer-work non_negative_integer]"
              << " [--burst-size positive_integer]"
              << " [--consumers positive_integer]\n";
}

bool parse_non_negative_int(char* raw_value, int& parsed_value)
{
    const int value = std::atoi(raw_value);
    if (value < 0) {
        return false;
    }

    parsed_value = value;
    return true;
}

bool parse_positive_int(char* raw_value, int& parsed_value)
{
    if (!parse_non_negative_int(raw_value, parsed_value)) {
        return false;
    }

    return parsed_value > 0;
}

bool parse_config(int argc, char* argv[], BenchmarkConfig& config)
{
    for (int index = 1; index < argc; index += 2) {
        if (index + 1 >= argc) {
            print_usage(argv[0]);
            return false;
        }

        const std::string_view option = argv[index];
        char* raw_value = argv[index + 1];

        if (option == "--items") {
            if (!parse_positive_int(raw_value, config.item_count)) {
                print_usage(argv[0]);
                return false;
            }
        } else if (option == "--producer-delay-us") {
            if (!parse_non_negative_int(raw_value, config.producer_delay_us)) {
                print_usage(argv[0]);
                return false;
            }
        } else if (option == "--consumer-work") {
            if (!parse_non_negative_int(raw_value, config.consumer_work)) {
                print_usage(argv[0]);
                return false;
            }
        } else if (option == "--burst-size") {
            if (!parse_positive_int(raw_value, config.burst_size)) {
                print_usage(argv[0]);
                return false;
            }
        } else if (option == "--consumers") {
            if (!parse_positive_int(raw_value, config.consumer_count)) {
                print_usage(argv[0]);
                return false;
            }
        } else {
            print_usage(argv[0]);
            return false;
        }
    }

    return true;
}

int simulate_consumer_work(int value, int work_iterations)
{
    int result = value;
    for (int iteration = 0; iteration < work_iterations; ++iteration) {
        result = (result * 31 + iteration) % 1'000'003;
    }

    return result;
}

} // namespace

int main(int argc, char* argv[])
{
    BenchmarkConfig config;
    if (!parse_config(argc, argv, config)) {
        return 1;
    }

    video_labs::ThreadSafeQueue<int> queue;
    std::vector<ConsumerStats> consumer_stats(static_cast<std::size_t>(config.consumer_count));

    video_labs::BenchmarkTimer timer;

    std::thread producer([&queue, &config] {
        for (int value = 0; value < config.item_count; ++value) {
            queue.push(value);

            const bool burst_complete = (value + 1) % config.burst_size == 0;
            if (config.producer_delay_us > 0 && burst_complete) {
                std::this_thread::sleep_for(
                    std::chrono::microseconds(config.producer_delay_us));
            }
        }
        for (int index = 0; index < config.consumer_count; ++index) {
            queue.push(poison_pill);
        }
    });

    video_labs::ThreadPool consumer_pool(static_cast<std::size_t>(config.consumer_count));
    consumer_pool.start([&queue, &config, &consumer_stats](std::size_t index) {
        ConsumerStats local_stats;

        while (true) {
            const int value = queue.pop();
            if (value == poison_pill) {
                break;
            }

            ++local_stats.consumed_count;
            local_stats.checksum += value;
            local_stats.work_checksum += simulate_consumer_work(value, config.consumer_work);
        }

        consumer_stats[index] = local_stats;
    });

    producer.join();
    consumer_pool.join();

    ConsumerStats total_stats;
    for (const auto& stats : consumer_stats) {
        total_stats.consumed_count += stats.consumed_count;
        total_stats.checksum += stats.checksum;
        total_stats.work_checksum += stats.work_checksum;
    }

    const double elapsed_seconds = timer.elapsed_seconds();
    const double throughput = static_cast<double>(total_stats.consumed_count) / elapsed_seconds;
    const long long expected_checksum =
        (static_cast<long long>(config.item_count - 1) * config.item_count) / 2;

    std::cout << "phase=2_baseline\n";
    std::cout << "producers=1\n";
    std::cout << "consumers=" << config.consumer_count << '\n';
    std::cout << "items=" << config.item_count << '\n';
    std::cout << "producer_delay_us=" << config.producer_delay_us << '\n';
    std::cout << "consumer_work=" << config.consumer_work << '\n';
    std::cout << "burst_size=" << config.burst_size << '\n';
    std::cout << "consumed=" << total_stats.consumed_count << '\n';
    std::cout << "elapsed_seconds=" << elapsed_seconds << '\n';
    std::cout << "throughput_items_per_second=" << throughput << '\n';
    std::cout << "checksum=" << total_stats.checksum << '\n';
    std::cout << "work_checksum=" << total_stats.work_checksum << '\n';

    if (total_stats.consumed_count != config.item_count
        || total_stats.checksum != expected_checksum) {
        std::cerr << "Benchmark validation failed\n";
        return 1;
    }

    return 0;
}
