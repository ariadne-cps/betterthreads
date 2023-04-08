/***************************************************************************
 *            test_task_manager.cpp
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

#include <thread>
#include "thread_manager.hpp"
#include "test.hpp"

using namespace BetterThreads;
using namespace std::chrono_literals;

class TestWorkloadAdvancement {
  public:

    void test_set_concurrency() {
        auto max_concurrency = TaskManager::instance().maximum_concurrency();
        TaskManager::instance().set_concurrency(max_concurrency);
        BETTERTHREADS_TEST_EQUALS(TaskManager::instance().concurrency(), max_concurrency)
        TaskManager::instance().set_maximum_concurrency();
        BETTERTHREADS_TEST_EQUALS(TaskManager::instance().concurrency(), max_concurrency)
        BETTERTHREADS_TEST_FAIL(TaskManager::instance().set_concurrency(1 + max_concurrency))
    }

    void test_run_task_with_one_thread() {
        TaskManager::instance().set_concurrency(1);
        int a = 10;
        auto result = TaskManager::instance().enqueue([&a]{ return a * a; }).get();
        BETTERTHREADS_TEST_EQUALS(result,100)
    }

    void test_run_task_with_multiple_threads() {
        TaskManager::instance().set_concurrency(TaskManager::instance().maximum_concurrency());
        int a = 10;
        auto result = TaskManager::instance().enqueue([&a]{ return a * a; }).get();
        BETTERTHREADS_TEST_EQUALS(result,100)
    }

    void test_run_task_with_no_threads() {
        TaskManager::instance().set_concurrency(0);
        int a = 10;
        auto result = TaskManager::instance().enqueue([&a]{ return a * a; }).get();
        BETTERTHREADS_TEST_EQUALS(result,100)
    }

    void test_change_concurrency_and_log_scheduler() {
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_concurrency(1))
        BETTERTHREADS_TEST_FAIL(TaskManager::instance().set_logging_immediate_scheduler())
        BETTERTHREADS_TEST_FAIL(TaskManager::instance().set_logging_blocking_scheduler())
        BETTERTHREADS_TEST_FAIL(TaskManager::instance().set_logging_nonblocking_scheduler())
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_concurrency(0))
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_logging_immediate_scheduler())
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_logging_blocking_scheduler())
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_logging_nonblocking_scheduler())
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_concurrency(1))
        BETTERTHREADS_TEST_EXECUTE(TaskManager::instance().set_concurrency(0))
    }

    void test() {
        BETTERTHREADS_TEST_CALL(test_set_concurrency())
        BETTERTHREADS_TEST_CALL(test_run_task_with_one_thread())
        BETTERTHREADS_TEST_CALL(test_run_task_with_multiple_threads())
        BETTERTHREADS_TEST_CALL(test_run_task_with_no_threads())
        BETTERTHREADS_TEST_CALL(test_change_concurrency_and_log_scheduler())
    }
};

int main() {
    TestWorkloadAdvancement().test();
    return BETTERTHREADS_TEST_FAILURES;
}
