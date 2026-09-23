/***************************************************************************
 *            test_thread_pool.cpp
 *
 *  Copyright  2022  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of BetterThreads, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "helper/test.hpp"
#include "conclog/logging.hpp"
#include "conclog/thread_registry_interface.hpp"
#include "thread_pool.hpp"

using namespace BetterThreads;

using namespace std::chrono_literals;

class ThreadRegistry : public ConcLog::ThreadRegistryInterface {
public:
    ThreadRegistry() : _threads_registered(0) { }
    bool has_threads_registered() const override { return _threads_registered > 0; }
    void set_threads_registered(unsigned int threads_registered) { _threads_registered = threads_registered; }
private:
    unsigned int _threads_registered;
};

class TestSmartThreadPool {
  public:

    void test_construct_thread_name() const {
        HELPER_TEST_EQUALS(construct_thread_name("name",9,9),"name9");
        HELPER_TEST_EQUALS(construct_thread_name("name",9,10),"name09");
        HELPER_TEST_EQUALS(construct_thread_name("name",10,11),"name10");
    }

    void test_construct() {
        auto max_concurrency = std::thread::hardware_concurrency();
        ThreadPool pool(max_concurrency);
        HELPER_TEST_EQUALS(pool.num_threads(),max_concurrency);
        HELPER_TEST_EQUALS(pool.queue_size(),0);
    }

    void test_construct_empty() {
        ThreadPool pool(0);
        HELPER_TEST_EQUALS(pool.num_threads(),0);
        VoidFunction fn([]{ std::this_thread::sleep_for(100ms); });
        pool.enqueue(fn);
        HELPER_TEST_EQUALS(pool.queue_size(),1);
    }

    void test_construct_with_name() {
        ThreadPool pool(1);
        HELPER_TEST_EQUALS(pool.name(),THREAD_POOL_DEFAULT_NAME);
        ThreadPool pool2(1,"name");
        HELPER_TEST_EQUALS(pool2.name(),"name");
    }

    void test_execute_single() {
        ThreadPool pool(1);
        HELPER_TEST_EQUALS(pool.num_threads(),1);
        VoidFunction fn([]{ std::this_thread::sleep_for(100ms); });
        pool.enqueue(fn);
        std::this_thread::sleep_for(200ms);
        HELPER_TEST_EQUALS(pool.queue_size(),0);
    }

    void test_exception() {
        ThreadPool pool(1);
        auto future = pool.enqueue([]{ throw new std::exception(); });
        HELPER_TEST_FAIL(future.get());
    }

    void test_destroy_before_completion() {
        ThreadPool pool(1);
        pool.enqueue([]{ std::this_thread::sleep_for(100ms); });
    }

    void test_execute_multiple_sequentially() {
        ThreadPool pool(1);
        HELPER_TEST_EQUALS(pool.num_threads(),1);
        HELPER_TEST_EQUALS(pool.queue_size(),0);
        VoidFunction fn([]{ std::this_thread::sleep_for(100ms); });
        for (size_t i=0; i<2; ++i) pool.enqueue(fn);
        HELPER_TEST_ASSERT(pool.queue_size() > 0);
        std::this_thread::sleep_for(400ms);
        HELPER_TEST_EQUALS(pool.queue_size(),0);
    }

    void test_execute_multiple_concurrently() {
        size_t num_threads = 2;
        ThreadPool pool(num_threads);
        HELPER_TEST_EQUALS(pool.num_threads(),2);
        VoidFunction fn([]{ std::this_thread::sleep_for(100ms); });
        for (size_t i=0; i<2; ++i) pool.enqueue(fn);
        std::this_thread::sleep_for(std::chrono::milliseconds(400*num_threads));
    }

    void test_execute_multiple_concurrently_sequentially() {
        size_t num_threads = 2;
        ThreadPool pool(num_threads);
        VoidFunction fn([]{ std::this_thread::sleep_for(100ms); });
        for (size_t i=0; i<2*num_threads; ++i) pool.enqueue(fn);
        HELPER_TEST_ASSERT(pool.queue_size() > 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(400*num_threads));
        HELPER_TEST_EQUALS(pool.queue_size(),0);
    }

    void test_process_on_atomic_type() {
        auto max_concurrency = std::thread::hardware_concurrency();
        ThreadPool pool(max_concurrency);
        std::vector<future<size_t>> results;
        std::atomic<size_t> x;

        for (size_t i = 0; i < 2 * max_concurrency; ++i) {
            results.emplace_back(pool.enqueue([&x] {
                                     size_t r = ++x;
                                     return r * r;
                                 })
            );
        }
        std::this_thread::sleep_for(100ms);
        HELPER_TEST_EQUALS(x,2*max_concurrency);

        size_t actual_sum = 0, expected_sum = 0;
        for (size_t i = 0; i < 2 * max_concurrency; ++i) {
            actual_sum += results[i].get();
            expected_sum += (i+1)*(i+1);
        }
        HELPER_TEST_EQUAL(actual_sum,expected_sum);
    }

    void test_set_num_threads_up_statically() const {
        ThreadPool pool(0);
        HELPER_TEST_EXECUTE(pool.set_num_threads(1));
        HELPER_TEST_EQUALS(pool.num_threads(),1);
        HELPER_TEST_EXECUTE(pool.set_num_threads(3));
        HELPER_TEST_EQUALS(pool.num_threads(),3);
    }

    void test_set_num_threads_same_statically() const {
        ThreadPool pool(3);
        HELPER_TEST_EXECUTE(pool.set_num_threads(3));
        HELPER_TEST_EQUALS(pool.num_threads(),3);
    }

    void test_set_num_threads_down_statically() const {
        ThreadPool pool(3);
        HELPER_TEST_EXECUTE(pool.set_num_threads(1));
        HELPER_TEST_EQUAL(pool.num_threads(),1);
    }

    void test_set_num_threads_up_dynamically() const {
        ThreadPool pool(0);
        VoidFunction fn([] { std::this_thread::sleep_for(100ms); });
        pool.enqueue(fn);
        std::this_thread::sleep_for(100ms);
        HELPER_TEST_EQUALS(pool.queue_size(),1);
        HELPER_TEST_EXECUTE(pool.set_num_threads(1));
        HELPER_TEST_EQUALS(pool.num_threads(),1);
        std::this_thread::sleep_for(100ms);
        HELPER_TEST_EQUALS(pool.queue_size(),0);
        pool.enqueue(fn);
        pool.enqueue(fn);
        HELPER_TEST_EXECUTE(pool.set_num_threads(3));
        HELPER_TEST_EQUALS(pool.num_threads(),3);
    }

    void test_set_num_threads_down_dynamically() const {
        ThreadPool pool(3);
        VoidFunction fn([] { std::this_thread::sleep_for(100ms); });
        for (size_t i=0; i<5; ++i)
            pool.enqueue(fn);
        HELPER_TEST_EXECUTE(pool.set_num_threads(2));
        HELPER_TEST_EQUAL(pool.num_threads(),2);
        std::this_thread::sleep_for(200ms);
        HELPER_TEST_EQUALS(pool.queue_size(),0);
    }



    void test_shrink_does_not_drain_backlog_on_retiring_workers() {
        ThreadPool pool(2);
        std::atomic<size_t> started = 0;
        std::atomic<bool> release_initial = false;
        std::atomic<bool> backlog_started = false;
        std::atomic<bool> release_backlog = false;

        auto initial = [&] {
            ++started;
            while (not release_initial.load()) std::this_thread::yield();
        };

        pool.enqueue(initial);
        pool.enqueue(initial);
        while (started.load() < 2) std::this_thread::yield();

        for (size_t i=0; i<4; ++i) {
            pool.enqueue([&] {
                backlog_started = true;
                while (not release_backlog.load()) std::this_thread::yield();
            });
        }

        std::promise<void> shrink_started_promise;
        auto shrink_started = shrink_started_promise.get_future();
        std::promise<void> shrink_done_promise;
        auto shrink_done = shrink_done_promise.get_future();

        std::thread shrinker([&] {
            shrink_started_promise.set_value();
            pool.set_num_threads(1);
            shrink_done_promise.set_value();
        });

        shrink_started.get();
        release_initial = true;

        while (not backlog_started.load()) std::this_thread::yield();

        HELPER_TEST_ASSERT(shrink_done.wait_for(0ms) == std::future_status::ready)

        release_backlog = true;
        shrinker.join();
    }

    void test_shrink_from_worker_is_rejected() {
        ThreadPool pool(2);
        auto future = pool.enqueue([&pool] { pool.set_num_threads(0); });
        HELPER_TEST_FAIL(future.get())
        HELPER_TEST_EQUALS(pool.num_threads(),2)
    }

    void test_resize_repeatedly() const {
        ThreadPool pool(4);
        std::atomic<size_t> completed = 0;
        for (size_t round=0; round<20; ++round) {
            for (size_t i=0; i<16; ++i)
                pool.enqueue([&completed] { ++completed; });
            pool.set_num_threads(2);
            pool.set_num_threads(4);
        }
        pool.set_num_threads(0);
        HELPER_TEST_EQUALS(completed,320)
    }

    void test_set_num_threads_to_zero_dynamically() const {
        ThreadPool pool(3);
        VoidFunction fn([] { std::this_thread::sleep_for(100ms); });
        for (size_t i=0; i<5; ++i)
            pool.enqueue(fn);
        HELPER_TEST_EXECUTE(pool.set_num_threads(0));
        HELPER_TEST_EQUAL(pool.num_threads(),0);
        std::this_thread::sleep_for(100ms);
        HELPER_TEST_ASSERT(pool.queue_size() > 0);
    }

    void test() {
        HELPER_TEST_CALL(test_construct_thread_name());
        HELPER_TEST_CALL(test_construct());
        HELPER_TEST_CALL(test_construct_empty());
        HELPER_TEST_CALL(test_construct_with_name());
        HELPER_TEST_CALL(test_execute_single());
        HELPER_TEST_CALL(test_exception());
        HELPER_TEST_CALL(test_destroy_before_completion());
        HELPER_TEST_CALL(test_execute_multiple_sequentially());
        HELPER_TEST_CALL(test_execute_multiple_concurrently());
        HELPER_TEST_CALL(test_execute_multiple_concurrently_sequentially());
        HELPER_TEST_CALL(test_process_on_atomic_type());
        HELPER_TEST_CALL(test_set_num_threads_up_statically());
        HELPER_TEST_CALL(test_set_num_threads_same_statically());
        HELPER_TEST_CALL(test_set_num_threads_down_statically());
        HELPER_TEST_CALL(test_set_num_threads_up_dynamically());
        HELPER_TEST_CALL(test_set_num_threads_down_dynamically());
        HELPER_TEST_CALL(test_shrink_does_not_drain_backlog_on_retiring_workers());
        HELPER_TEST_CALL(test_shrink_from_worker_is_rejected());
        HELPER_TEST_CALL(test_resize_repeatedly());
        HELPER_TEST_CALL(test_set_num_threads_to_zero_dynamically());
    }
};

int main() {
    ThreadRegistry registry;
    ConcLog::Logger::instance().attach_thread_registry(&registry);
    TestSmartThreadPool().test();
    return HELPER_TEST_FAILURES;
}
