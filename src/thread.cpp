/***************************************************************************
 *            thread.cpp
 *
 *  Copyright  2022  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Threading.
 *
 *  Threading is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Threading is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Threading.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "logging/logging.hpp"
#include "threading/thread.hpp"

namespace Threading {

using Ariadne::Logging::Logger;
using Ariadne::Utility::to_string;

Thread::Thread(VoidFunction task, String name, bool active)
        : _name(std::move(name)), _got_id_future(_got_id_promise.get_future()), _active(active), _ready_for_task_future(_ready_for_task_promise.get_future()),
          _exception(nullptr)
{
#ifdef _WIN32
    std::cerr << "[threading] Thread ctor: before std::thread" << std::endl;
#endif
    _thread = std::thread([=,this]() {
        _id = std::this_thread::get_id();
#ifdef _WIN32
        std::cerr << "[threading] worker: id acquired" << std::endl;
#endif
        _got_id_promise.set_value();
#ifdef _WIN32
        std::cerr << "[threading] worker: waiting ready" << std::endl;
#endif
        _ready_for_task_future.get();
#ifdef _WIN32
        std::cerr << "[threading] worker: ready" << std::endl;
#endif
        if (_active) {
            try { task(); }
            catch(...) {
                std::lock_guard<std::mutex> lock(_exception_mutex);
                _exception = std::current_exception();
            }
        }
    });
    std::cerr << "[threading] Thread ctor: waiting id" << std::endl;
    _got_id_future.get();
    std::cerr << "[threading] Thread ctor: id ready" << std::endl;
    if (_name.empty()) _name = to_string(_id);
    if (active) {
        std::cerr << "[threading] Thread ctor: before register " << _name << std::endl;
        Logger::instance().register_thread(_id,_name);
        std::cerr << "[threading] Thread ctor: after register " << _name << std::endl;
        _ready_for_task_promise.set_value();
    }
}

Thread::Thread(VoidFunction task, String name) : Thread(task, name, true)
{ }

thread::id Thread::id() const {
    return _id;
}

String Thread::name() const {
    return _name;
}

void Thread::activate()  {
    bool expected = false;
    if (_active.compare_exchange_strong(expected,true)) {
        Logger::instance().register_thread(_id,_name);
        _ready_for_task_promise.set_value();
    }
}

exception_ptr Thread::exception() const {
    std::lock_guard<std::mutex> lock(_exception_mutex);
    return _exception;
}

Thread::~Thread() {
    auto active = _active.load();
    if (not active) _ready_for_task_promise.set_value();
    std::cerr << "[threading] Thread dtor: before join " << _name << std::endl;
    _thread.join();
    std::cerr << "[threading] Thread dtor: after join " << _name << std::endl;
    if (active) {
        std::cerr << "[threading] Thread dtor: before unregister " << _name << std::endl;
        Logger::instance().unregister_thread(_id);
        std::cerr << "[threading] Thread dtor: after unregister " << _name << std::endl;
    }
}

} // namespace Threading
