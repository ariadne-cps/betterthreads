/***************************************************************************
 *            utility.hpp
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

/*! \file utility.hpp
 *  \brief General utilities.
 */

#ifndef BETTERTHREADS_UTILITY_HPP
#define BETTERTHREADS_UTILITY_HPP

#include <sstream>
#include <ostream>
#include <utility>

namespace BetterThreads {

template<class T> inline std::string to_string(const T& t) { std::stringstream ss; ss << t; return ss.str(); }

template<class T1, class T2> constexpr std::pair<T1&,T2&> make_lpair(T1& t1, T2& t2) { return std::pair<T1&,T2&>(t1,t2); }

template<class T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    bool first=true;
    for(auto x : v) {
        os << (first ? "[" : ",") << x;
        first = false;
    }
    if(first) { os << "["; }
    return os << "]";
}

} // namespace BetterThreads

#endif // BETTERTHREADS_UTILITY_HPP
