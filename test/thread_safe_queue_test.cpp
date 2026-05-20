#include "video_labs/consumer.hpp"
#include "video_labs/producer.hpp"
#include "video_labs/thread_pool.hpp"
#include "video_labs/thread_safe_queue.hpp"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {

constexpr int poison_pill = -1;

void test_push_pop_preserves_order()
{
    video_labs::ThreadSafeQueue<int> queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);

    assert(queue.size() == 3);
    assert(!queue.is_empty());
    assert(queue.pop() == 1);
    assert(queue.pop() == 2);
    assert(queue.pop() == 3);
    assert(queue.is_empty());
}

void test_try_pop()
{
    video_labs::ThreadSafeQueue<int> queue;

    assert(!queue.try_pop().has_value());

    queue.push(42);
    const auto value = queue.try_pop();

    assert(value.has_value());
    assert(*value == 42);
    assert(queue.is_empty());
}

void test_blocking_pop_waits_for_producer()
{
    video_labs::ThreadSafeQueue<int> queue;

    std::thread producer([&queue] {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        queue.push(7);
    });

    assert(queue.pop() == 7);
    producer.join();
}

void test_producer_consumer_with_poison_pill()
{
    video_labs::ThreadSafeQueue<int> queue;
    video_labs::Producer producer_worker(queue, 100, poison_pill);
    video_labs::Consumer consumer_worker(queue, poison_pill);

    std::thread producer(&video_labs::Producer::run, &producer_worker);
    std::thread consumer(&video_labs::Consumer::run, &consumer_worker);

    producer.join();
    consumer.join();

    const auto& received = consumer_worker.received();
    assert(received.size() == 100);
    for (int expected = 0; expected < 100; ++expected) {
        assert(received[static_cast<std::size_t>(expected)] == expected);
    }
    assert(queue.is_empty());
}

void test_thread_pool_runs_each_worker()
{
    constexpr std::size_t worker_count = 4;
    std::vector<int> visits(worker_count, 0);

    video_labs::ThreadPool pool(worker_count);
    assert(pool.size() == worker_count);

    pool.start([&visits](std::size_t index) {
        visits[index] = 1;
    });
    pool.join();

    for (const int visit_count : visits) {
        assert(visit_count == 1);
    }
}

void test_thread_pool_rejects_zero_workers()
{
    bool threw = false;
    try {
        video_labs::ThreadPool pool(0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
}

} // namespace

int main()
{
    test_push_pop_preserves_order();
    test_try_pop();
    test_blocking_pop_waits_for_producer();
    test_producer_consumer_with_poison_pill();
    test_thread_pool_runs_each_worker();
    test_thread_pool_rejects_zero_workers();

    return 0;
}
