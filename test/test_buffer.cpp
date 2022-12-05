/***************************************************************************
 *            test_buffer.cpp
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
#include "buffer.hpp"
#include "test.hpp"

using namespace BetterThreads;

class TestBuffer {
  public:

    void test_construct() {
        Buffer<SizeType> buffer(2);
        BETTERTHREADS_TEST_EQUALS(buffer.size(),0);
        BETTERTHREADS_TEST_EQUALS(buffer.capacity(),2);
    }

    void test_construct_invalid() {
        BETTERTHREADS_TEST_FAIL(Buffer<SizeType>(0));
    }

    void test_set_capacity_when_empty() {
        Buffer<SizeType> buffer(2);
        buffer.set_capacity(5);
        BETTERTHREADS_TEST_EQUALS(buffer.capacity(),5);
        buffer.set_capacity(3);
        BETTERTHREADS_TEST_EQUALS(buffer.capacity(),3);
        BETTERTHREADS_TEST_FAIL(buffer.set_capacity(0));
    }

    void test_set_capacity_when_filled() {
        Buffer<SizeType> buffer(2);
        buffer.push(4);
        buffer.push(2);
        BETTERTHREADS_TEST_EXECUTE(buffer.set_capacity(5));
        BETTERTHREADS_TEST_FAIL(buffer.set_capacity(1));
        buffer.pull();
        BETTERTHREADS_TEST_EXECUTE(buffer.set_capacity(1));
    }

    void test_single_buffer() {
        Buffer<SizeType> buffer(2);
        buffer.push(4);
        buffer.push(2);
        BETTERTHREADS_TEST_EQUALS(buffer.size(),2);
        auto o1 = buffer.pull();
        auto o2 = buffer.pull();
        BETTERTHREADS_TEST_EQUALS(buffer.size(),0);
        BETTERTHREADS_TEST_EQUALS(o1,4);
        BETTERTHREADS_TEST_EQUALS(o2,2);
    }

    void test_io_buffer() {
        Buffer<SizeType> ib(2);
        Buffer<SizeType> ob(2);

        std::thread thread([&ib,&ob]() {
            while (true) {
                try {
                    auto i = ib.pull();
                    ob.push(i);
                } catch (BufferInterruptPullingException&) {
                    break;
                }
            }
        });
        ib.push(4);
        ib.push(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        BETTERTHREADS_TEST_EQUALS(ib.size(),0);
        BETTERTHREADS_TEST_EQUALS(ob.size(),2);
        auto o1 = ob.pull();
        BETTERTHREADS_TEST_EQUALS(ob.size(),1);
        BETTERTHREADS_TEST_EQUALS(o1,4);
        auto o2 = ob.pull();
        BETTERTHREADS_TEST_EQUALS(ob.size(),0);
        BETTERTHREADS_TEST_EQUALS(o2,2);
        ib.interrupt_consuming();
        thread.join();
    }

    void test() {
        BETTERTHREADS_TEST_CALL(test_construct());
        BETTERTHREADS_TEST_CALL(test_construct_invalid());
        BETTERTHREADS_TEST_CALL(test_set_capacity_when_empty());
        BETTERTHREADS_TEST_CALL(test_set_capacity_when_filled());
        BETTERTHREADS_TEST_CALL(test_single_buffer());
        BETTERTHREADS_TEST_CALL(test_io_buffer());
    }
};

int main() {
    TestBuffer().test();
    return BETTERTHREADS_TEST_FAILURES;
}
