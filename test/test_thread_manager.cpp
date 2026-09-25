/***************************************************************************
 *            test_task_manager.cpp
 *
 *  Copyright  2022  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Threading.
 *
 *  Threading is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Threading is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Threading.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <chrono>
#include <thread>
#include "helper/test.hpp"
#include "threading/thread_manager.hpp"

using namespace Threading;
using namespace std::chrono_literals;

class TestThreadManager {
  public:

    void test_set_concurrency() {
        auto max_concurrency = ThreadManager::instance().maximum_concurrency();
        ThreadManager::instance().set_concurrency(max_concurrency);
        HELPER_TEST_EQUALS(ThreadManager::instance().concurrency(), max_concurrency)
        ThreadManager::instance().set_maximum_concurrency();
        HELPER_TEST_EQUALS(ThreadManager::instance().concurrency(), max_concurrency)
        HELPER_TEST_FAIL(ThreadManager::instance().set_concurrency(1 + max_concurrency))
    }

    void test_run_task_with_one_thread() {
        ThreadManager::instance().set_concurrency(1);
        int a = 10;
        auto result = ThreadManager::instance().enqueue(std::function<int()>([&a]{ return a * a; })).get();
        HELPER_TEST_EQUALS(result,100)
    }

    void test_run_task_with_multiple_threads() {
        ThreadManager::instance().set_concurrency(ThreadManager::instance().maximum_concurrency());
        int a = 10;
        auto result = ThreadManager::instance().enqueue(std::function<int()>([&a]{ return a * a; })).get();
        HELPER_TEST_EQUALS(result,100)
    }

    void test_run_task_with_no_threads() {
        ThreadManager::instance().set_concurrency(0);
        int a = 10;
        auto result = ThreadManager::instance().enqueue(std::function<int()>([&a]{ return a * a; })).get();
        HELPER_TEST_EQUALS(result,100)
    }


    void test_void_task_with_no_threads() {
        ThreadManager::instance().set_concurrency(0);
        std::atomic<bool> executed = false;
        ThreadManager::instance().enqueue(VoidFunction([&] { executed = true; })).get();
        HELPER_TEST_ASSERT(executed.load())
    }

    void test_concurrency_read_during_shrink() {
        if (ThreadManager::instance().maximum_concurrency() < 2) return;
        ThreadManager::instance().set_concurrency(2);
        std::atomic<bool> task_started = false;
        std::atomic<bool> allow_read = false;

        auto future = ThreadManager::instance().enqueue(VoidFunction([&] {
            task_started = true;
            while (not allow_read.load()) std::this_thread::yield();
            HELPER_TEST_EQUALS(ThreadManager::instance().concurrency(),1)
        }));

        while (not task_started.load()) std::this_thread::yield();

        auto shrink = std::async(std::launch::async,[] {
            ThreadManager::instance().set_concurrency(1);
        });

        while (ThreadManager::instance().concurrency() != 1) std::this_thread::yield();
        allow_read = true;
        future.get();
        shrink.get();
        ThreadManager::instance().set_concurrency(0);
    }


    void test_worker_cannot_shrink_manager() {
        if (ThreadManager::instance().maximum_concurrency() == 0) return;
        ThreadManager::instance().set_concurrency(1);
        auto future = ThreadManager::instance().enqueue(VoidFunction([] {
            ThreadManager::instance().set_concurrency(0);
        }));
        HELPER_TEST_FAIL(future.get())
        HELPER_TEST_EQUALS(ThreadManager::instance().concurrency(),1)
        ThreadManager::instance().set_concurrency(0);
    }

    void test_change_concurrency_and_log_scheduler() {
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_concurrency(1))
        HELPER_TEST_FAIL(ThreadManager::instance().set_logging_immediate_scheduler())
        HELPER_TEST_FAIL(ThreadManager::instance().set_logging_blocking_scheduler())
        HELPER_TEST_FAIL(ThreadManager::instance().set_logging_nonblocking_scheduler())
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_concurrency(0))
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_logging_immediate_scheduler())
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_logging_blocking_scheduler())
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_logging_nonblocking_scheduler())
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_concurrency(1))
        HELPER_TEST_EXECUTE(ThreadManager::instance().set_concurrency(0))
    }

    void test() {
        HELPER_TEST_CALL(test_set_concurrency())
        HELPER_TEST_CALL(test_run_task_with_one_thread())
        HELPER_TEST_CALL(test_run_task_with_multiple_threads())
        HELPER_TEST_CALL(test_run_task_with_no_threads())
        HELPER_TEST_CALL(test_void_task_with_no_threads())
        HELPER_TEST_CALL(test_concurrency_read_during_shrink())
        HELPER_TEST_CALL(test_worker_cannot_shrink_manager())
        HELPER_TEST_CALL(test_change_concurrency_and_log_scheduler())
    }
};

int main() {
    TestThreadManager().test();
    return HELPER_TEST_FAILURES;
}
