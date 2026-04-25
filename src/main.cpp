#include "video_labs/consumer.hpp"
#include "video_labs/producer.hpp"
#include "video_labs/thread_safe_queue.hpp"

#include <iostream>
#include <thread>

namespace {

constexpr int poison_pill = -1;
constexpr int item_count = 10;

} // namespace

int main()
{
    video_labs::ThreadSafeQueue<int> queue;
    video_labs::Producer producer_worker(queue, item_count, poison_pill);
    video_labs::Consumer consumer_worker(queue, poison_pill);

    std::thread producer(&video_labs::Producer::run, &producer_worker);
    std::thread consumer(&video_labs::Consumer::run, &consumer_worker);

    producer.join();
    consumer.join();

    const auto& received = consumer_worker.received();
    std::cout << "Processed " << received.size() << " simulated packets\n";
    for (const int value : received) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    return received.size() == item_count ? 0 : 1;
}
