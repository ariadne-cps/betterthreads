/***************************************************************************
 *            test_buffered_thread.cpp
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

#include "buffered_thread.hpp"
#include "utility.hpp"
#include "conclog/logging.hpp"
#include "conclog/thread_registry_interface.hpp"
#include "test.hpp"

using namespace BetterThreads;

class ThreadRegistry : public ConcLog::ThreadRegistryInterface {
public:
    ThreadRegistry() : _threads_registered(0) { }
    bool has_threads_registered() const override { return _threads_registered > 0; }
    void set_threads_registered(unsigned int threads_registered) { _threads_registered = threads_registered; }
private:
    unsigned int _threads_registered;
};

class TestBufferedThread {
  public:

    void test_create() const {
        BufferedThread thread1("thr");
        BETTERTHREADS_TEST_EXECUTE(thread1.id());
        BETTERTHREADS_TEST_EQUALS(thread1.name(),"thr");
        BETTERTHREADS_TEST_EQUALS(thread1.queue_size(),0);
        BETTERTHREADS_TEST_EQUALS(thread1.queue_capacity(),1);
        BufferedThread thread2;
        BETTERTHREADS_TEST_EQUALS(to_string(thread2.id()),thread2.name());
    }

    void test_set_queue_capacity() const {
        BufferedThread thread;
        BETTERTHREADS_TEST_FAIL(thread.set_queue_capacity(0));
        BETTERTHREADS_TEST_EXECUTE(thread.set_queue_capacity(2));
        BETTERTHREADS_TEST_EXECUTE(thread.set_queue_capacity(1));
    }

    void test_destroy_before_completion() const {
        BufferedThread thread;
        thread.enqueue([] { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
    }

    void test_exception() const {
        BufferedThread thread;
        auto future = thread.enqueue([] { throw new std::exception(); });
        BETTERTHREADS_TEST_FAIL(future.get());
    }

    void test_has_queued_tasks() const {
        BufferedThread thread;
        thread.set_queue_capacity(2);
        thread.enqueue([] { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
        thread.enqueue([] { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
        BETTERTHREADS_TEST_ASSERT(thread.queue_size()>0);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        BETTERTHREADS_TEST_EQUALS(thread.queue_size(),0);
    }

    void test_set_queue_capacity_down_failure() const {
        BufferedThread thread;
        thread.set_queue_capacity(3);
        VoidFunction fn([]{ std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
        thread.enqueue(fn);
        thread.enqueue(fn);
        thread.enqueue(fn);
        BETTERTHREADS_TEST_FAIL(thread.set_queue_capacity(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        BETTERTHREADS_TEST_EXECUTE(thread.set_queue_capacity(1));
    }

    void test_task_return() const {
        BufferedThread thread;
        auto result = thread.enqueue([] { return 42; });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        BETTERTHREADS_TEST_EQUALS(result.get(),42);
    }

    void test_task_capture() const {
        int a = 0;
        BufferedThread thread;
        thread.enqueue([&a] { a++; });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        BETTERTHREADS_TEST_EQUALS(a,1);
    }

    void test_task_arguments() const {
        int x = 3;
        int y = 5;
        BufferedThread thread;
        auto future = thread.enqueue([](int a, int b) { return a * b; }, x, y);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto r = future.get();
        BETTERTHREADS_TEST_EQUALS(r,15);
    }

    void test_multiple_tasks() const {
        BufferedThread thread;
        int a = 4;
        thread.enqueue([&a] {
            a += 2;
            return a;
        });
        auto future = thread.enqueue([&a] {
            a *= 7;
            return a;
        });
        int r = future.get();
        BETTERTHREADS_TEST_EQUALS(r,42);
    }

    void test_atomic_multiple_threads() const {
        SizeType n_threads = 10*std::thread::hardware_concurrency();
        BETTERTHREADS_TEST_PRINT(n_threads);
        List<SharedPointer<BufferedThread>> threads;

        std::atomic<SizeType> a = 0;
        for (SizeType i=0; i<n_threads; ++i) {
            threads.push_back(make_shared<BufferedThread>(("add" + to_string(i))));
            threads.at(i)->enqueue([&a] { a++; });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        BETTERTHREADS_TEST_EQUALS(a,n_threads);
        threads.clear();
    }

    void test() {
        BETTERTHREADS_TEST_CALL(test_create());
        BETTERTHREADS_TEST_CALL(test_set_queue_capacity());
        BETTERTHREADS_TEST_CALL(test_destroy_before_completion());
        BETTERTHREADS_TEST_CALL(test_exception());
        BETTERTHREADS_TEST_CALL(test_has_queued_tasks());
        BETTERTHREADS_TEST_CALL(test_set_queue_capacity_down_failure());
        BETTERTHREADS_TEST_CALL(test_task_return());
        BETTERTHREADS_TEST_CALL(test_task_capture());
        BETTERTHREADS_TEST_CALL(test_task_arguments());
        BETTERTHREADS_TEST_CALL(test_multiple_tasks());
        BETTERTHREADS_TEST_CALL(test_atomic_multiple_threads());
    }

};

int main() {
    ThreadRegistry registry;
    ConcLog::Logger::instance().attach_thread_registry(&registry);
    TestBufferedThread().test();
    return BETTERTHREADS_TEST_FAILURES;
}
