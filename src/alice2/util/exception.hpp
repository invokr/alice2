/**
 * @file exception.hpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 * @version 1.0
 *
 * @par License
 *    Alice Replay Parser
 *    Copyright 2014-2015 Robin Dietrich
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef _ALICE_UTIL_EXCEPTION_HPP_
#define _ALICE_UTIL_EXCEPTION_HPP_

#include <exception>
#include <sstream>
#include <memory>
#include <utility>
#include <iostream>

/// Creates a new exception called __name whos what()
/// method returns __msg by default.
#define ALICE_CREATE_EXCEPTION(__name, __msg)   \
struct __name : virtual alice::exception {      \
    virtual const char* what() const throw() {  \
        msg = std::string(__msg);               \
        msg.append(": ");                       \
        msg.append(this->data());               \
        return msg.c_str();                     \
    }                                           \
private:                                        \
    mutable std::string msg;                    \
};

/// Throws an exception created with ALICE_CREATE_EXCEPTION.
#define ALICE_THROW(__exception, __data) \
do {                                     \
    __exception e;                       \
    e << __data;                         \
    throw e;                             \
} while(0)

// Conditional block when we want to disable exceptions
#ifdef ALICE_NO_EXCEPTIONS
#undef ALICE_THROW
#define ALICE_THROW(__exception, __data)                                \
    do {                                                                \
        std::cout << "Exception " << #__exception << "thrown in line "  \
                  << __FILE__ << " : " << __LINE__ << std::endl;        \
        std::cout << __data << std::endl;                               \
        exit(1);                                                        \
    } while(0)
#endif /* ALICE_NO_EXCEPTIONS */

namespace alice {
    /** Exception class that forwards parameters to the handler */
    struct exception {
        /** Exception constructor */
        exception() : msg(std::make_shared<std::ostringstream>("")) {}

        /** Default copy constructor */
        exception(const exception&) = default;

        /** Default move constructor */
        exception(exception&&) = default;

        /** Default destructor */
        ~exception() = default;

        /** Default copy assignment operator */
        exception& operator=(const exception&) = default;

        /** Add data to the exception */
        template <typename T>
        exception& operator<<(T&& data) {
            (*msg) << std::forward<T>(data);
            return *this;
        }

        /** Returns data from msg as string, invalidates after call */
        std::string data() const noexcept {
            return msg->str();
        }
    private:
        /** Internal data */
        std::shared_ptr<std::ostringstream> msg;
    };
}

#endif /* _ALICE_UTIL_EXCEPTION_HPP_ */