/***************************************************************************
 *            workload_advancement.cpp
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
#include "workload_advancement.hpp"

namespace BetterThreads {

WorkloadAdvancement::WorkloadAdvancement(SizeType initial) : _num_waiting(initial), _num_processing(0), _num_completed(0) { }

SizeType WorkloadAdvancement::waiting() const {
    LockGuard<Mutex> lock(_mux);
    return _num_waiting;
}

SizeType WorkloadAdvancement::processing() const {
    LockGuard<Mutex> lock(_mux);
    return _num_processing;
}

SizeType WorkloadAdvancement::completed() const {
    LockGuard<Mutex> lock(_mux);
    return _num_completed;
}

SizeType WorkloadAdvancement::total() const {
    LockGuard<Mutex> lock(_mux);
    return _num_waiting + _num_processing + _num_completed;
}

void WorkloadAdvancement::add_to_waiting(SizeType n) {
    BETTERTHREADS_PRECONDITION(n > 0);
    LockGuard<Mutex> lock(_mux);
    _num_waiting+=n;
}

void WorkloadAdvancement::add_to_processing(SizeType n) {
    BETTERTHREADS_PRECONDITION(n <= _num_waiting);
    LockGuard<Mutex> lock(_mux);
    _num_waiting-=n;
    _num_processing+=n;
}

void WorkloadAdvancement::add_to_completed(SizeType n) {
    BETTERTHREADS_PRECONDITION(n <=_num_processing);
    LockGuard<Mutex> lock(_mux);
    _num_processing-=n;
    _num_completed+=n;
}

double WorkloadAdvancement::completion_rate() const {
    LockGuard<Mutex> lock(_mux);
    double total = _num_waiting + _num_processing + _num_completed;
    return ((double)_num_completed) / total;
}

bool WorkloadAdvancement::has_finished() const {
    LockGuard<Mutex> lock(_mux);
    return _num_processing == 0 and _num_waiting == 0;
}

} // namespace BetterThreads
