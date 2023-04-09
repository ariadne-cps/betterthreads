/***************************************************************************
 *            test_workload_advancement.cpp
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

#include "utility/test.hpp"
#include "workload_advancement.hpp"

using namespace BetterThreads;

class TestWorkloadAdvancement {
  public:

    void test_creation() {
        WorkloadAdvancement wp(5);
        UTILITY_TEST_EQUALS(wp.completion_rate(),0.0)
        UTILITY_TEST_EQUALS(wp.waiting(),5)
        UTILITY_TEST_EQUALS(wp.processing(),0)
        UTILITY_TEST_EQUALS(wp.completed(),0)
        UTILITY_TEST_EQUALS(wp.total(),5)
        UTILITY_TEST_ASSERT(not wp.has_finished())
    }

    void test_advance() {
        WorkloadAdvancement wp(3);
        wp.add_to_waiting();
        UTILITY_TEST_EQUALS(wp.waiting(),4);
        UTILITY_TEST_EQUALS(wp.total(),4)
        wp.add_to_processing();
        UTILITY_TEST_EQUALS(wp.waiting(),3);
        UTILITY_TEST_EQUALS(wp.processing(),1);
        UTILITY_TEST_EQUALS(wp.total(),4)
        wp.add_to_completed();
        UTILITY_TEST_EQUALS(wp.processing(),0);
        UTILITY_TEST_EQUALS(wp.completed(),1);
        UTILITY_TEST_EQUALS(wp.total(),4)
        UTILITY_TEST_EQUALS(wp.completion_rate(),0.25);
    }

    void test_finished() {
        WorkloadAdvancement wp;
        UTILITY_TEST_ASSERT(wp.has_finished());
        wp.add_to_waiting(2);
        UTILITY_TEST_ASSERT(not wp.has_finished());
        wp.add_to_processing(2);
        wp.add_to_completed(2);
        UTILITY_TEST_EQUALS(wp.completion_rate(),1.0);
        UTILITY_TEST_ASSERT(wp.has_finished());
    }

    void test_invalid_transitions() {
        WorkloadAdvancement wp(4);
        UTILITY_TEST_FAIL(wp.add_to_processing(5));
        UTILITY_TEST_FAIL(wp.add_to_completed());
        wp.add_to_processing(2);
        UTILITY_TEST_FAIL(wp.add_to_completed(3));
        wp.add_to_completed(1);
        UTILITY_TEST_EQUALS(wp.completion_rate(),0.25);
    }

    void test() {
        UTILITY_TEST_CALL(test_creation());
        UTILITY_TEST_CALL(test_advance());
        UTILITY_TEST_CALL(test_finished());
        UTILITY_TEST_CALL(test_invalid_transitions());
    }

};

int main() {
    TestWorkloadAdvancement().test();
    return UTILITY_TEST_FAILURES;
}
