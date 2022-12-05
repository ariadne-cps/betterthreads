/***************************************************************************
 *            task_manager.cpp
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

#include "macros.hpp"
#include "conclog/logging.hpp"
#include "task_manager.hpp"

namespace BetterThreads {

TaskManager::TaskManager() : _maximum_concurrency(std::thread::hardware_concurrency()), _concurrency(0), _pool(0) {}

bool TaskManager::has_threads_registered() const {
    return _concurrency;
}

SizeType TaskManager::maximum_concurrency() const {
    return _maximum_concurrency;
}

SizeType TaskManager::concurrency() const {
    LockGuard<Mutex> lock(_concurrency_mutex);
    return _concurrency;
}

void TaskManager::set_concurrency(SizeType value) {
    ARIADNE_PRECONDITION(value <= _maximum_concurrency);
    LockGuard<Mutex> lock(_concurrency_mutex);
    _concurrency = value;
    _pool.set_num_threads(value);
}

void TaskManager::set_maximum_concurrency() {
    set_concurrency(_maximum_concurrency);
}

void TaskManager::set_logging_immediate_scheduler() const {
    ARIADNE_PRECONDITION(_concurrency == 0)
    Logger::instance().use_immediate_scheduler();
}

void TaskManager::set_logging_blocking_scheduler() const {
    ARIADNE_PRECONDITION(_concurrency == 0)
    Logger::instance().use_blocking_scheduler();
}

void TaskManager::set_logging_nonblocking_scheduler() const {
    ARIADNE_PRECONDITION(_concurrency == 0)
    Logger::instance().use_nonblocking_scheduler();
}

}