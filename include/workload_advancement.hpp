/***************************************************************************
 *            workload_advancement.hpp
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

/*! \file workload_advancement.hpp
 *  \brief Synchronised class to manage the status of multiple elements to process
 */

#ifndef BETTERTHREADS_WORKLOAD_ADVANCEMENT_HPP
#define BETTERTHREADS_WORKLOAD_ADVANCEMENT_HPP

#include <algorithm>
#include "typedefs.hpp"

namespace BetterThreads {

//! \brief Synchronised class to manage the status of multiple elements to process
class WorkloadAdvancement {
  public:
    WorkloadAdvancement(SizeType initial = 0);

    //! \brief The elements waiting to be processed
    SizeType waiting() const;
    //! \brief The elements under processing
    SizeType processing() const;
    //! \brief The completed elements
    SizeType completed() const;

    //! \brief All the elements (sum of waiting, processing and completed)
    SizeType total() const;

    //! \brief Add n elements to waiting
    void add_to_waiting(SizeType n = 1);
    //! \brief Move n waiting to processing
    void add_to_processing(SizeType n = 1);
    //! \brief Move n processing to completed
    void add_to_completed(SizeType n = 1);

    //! \brief The rate of completion r (0<=r<=1) related to the progress
    double completion_rate() const;

    //! \brief If no other processing remains
    bool has_finished() const;

  private:
    SizeType _num_waiting;
    SizeType _num_processing;
    SizeType _num_completed;

    Mutex mutable _mux;
};

} // namespace BetterThreads

#endif // BETTERTHREADS_WORKLOAD_ADVANCEMENT_HPP
