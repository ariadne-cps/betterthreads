/***************************************************************************
 *            concurrency_typedefs.hpp
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

/*! \file concurrency_typedefs.hpp
 *  \brief Typedefs for the module.
 */

#ifndef BETTERTHREADS_CONCURRENCY_TYPEDEFS_HPP
#define BETTERTHREADS_CONCURRENCY_TYPEDEFS_HPP

#include <utility>
#include <mutex>
#include <thread>
#include <future>
#include "typedefs.hpp"

namespace BetterThreads {

using ConditionVariable = std::condition_variable;
using Mutex = std::mutex;
template<class T> using LockGuard = std::lock_guard<T>;
template<class T> using UniqueLock = std::unique_lock<T>;
using ThreadId = std::thread::id;
using VoidFunction = std::function<Void()>;
template<class T> using Future = std::future<T>;
template<class T> using Promise = std::promise<T>;
template<class T> using PackagedTask = std::packaged_task<T>;


} // namespace BetterThreads

#endif // BETTERTHREADS_CONCURRENCY_TYPEDEFS_HPP
