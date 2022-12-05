/***************************************************************************
 *            thread.hpp
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

/*! \file thread.hpp
 *  \brief A wrapper for smart handling of a thread
 */

#ifndef BETTERTHREADS_THREAD_HPP
#define BETTERTHREADS_THREAD_HPP

#include <utility>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <functional>
#include "typedefs.hpp"
#include "concurrency_typedefs.hpp"

namespace BetterThreads {

using ExceptionPtr = std::exception_ptr;

//! \brief A class for handling a thread for a pool in a smarter way.
//! \details It allows to wait for the start of the \a task before extracting the thread id, which is held along with
//! a readable \a name.
class Thread {
  public:

    //! \brief Construct with an optional name.
    //! \details The thread will start and store the id
    Thread(VoidFunction task, String name = String());

    //! \brief Get the thread id
    ThreadId id() const;
    //! \brief Get the readable name
    String name() const;

    //! \brief The exception, if it exists
    ExceptionPtr const& exception() const;

    //! \brief Destroy the instance
    ~Thread();

  private:
    String _name;
    ThreadId _id;
    std::thread _thread;
    Promise<Void> _got_id_promise;
    Future<Void> _got_id_future;
    Promise<Void> _registered_thread_promise;
    Future<Void> _registered_thread_future;
    ExceptionPtr _exception;
};

} // namespace BetterThreads

#endif // BETTERTHREADS_THREAD_HPP
