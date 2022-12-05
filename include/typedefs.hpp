/***************************************************************************
 *            typedefs.hpp
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

/*! \file typedefs.hpp
 *  \brief General typedefs.
 */

#ifndef BETTERTHREADS_TYPEDEFS_HPP
#define BETTERTHREADS_TYPEDEFS_HPP

#include <utility>
#include <vector>
#include <mutex>
#include <thread>
#include <future>
#include <memory>
#include "conclog/logging.hpp"

namespace BetterThreads {

template<class SIG> struct ResultOfTrait;
template<class F, class... AS> struct ResultOfTrait<F(AS...)> { typedef typename std::invoke_result<F,AS...>::type Type; };
template<class SIG> using ResultOf = typename ResultOfTrait<SIG>::Type;

using ConcLog::SizeType;
using String = std::string;
template<class T> using SharedPointer = std::shared_ptr<T>;
template<class T> using List = std::vector<T>;

using ConditionVariable = std::condition_variable;
using Mutex = std::mutex;
template<class T> using LockGuard = std::lock_guard<T>;
template<class T> using UniqueLock = std::unique_lock<T>;
using ThreadId = std::thread::id;
using VoidFunction = std::function<void()>;
template<class T> using Future = std::future<T>;
template<class T> using Promise = std::promise<T>;
template<class T> using PackagedTask = std::packaged_task<T>;


} // namespace BetterThreads

#endif // BETTERTHREADS_TYPEDEFS_HPP
