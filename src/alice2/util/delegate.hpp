/**
 * @file delegate.hpp
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

#ifndef _ALICE_UTIL_DELEGATE_HPP_
#define _ALICE_UTIL_DELEGATE_HPP_

#include <memory>

namespace alice {
    template <typename T>
    class delegate;

    /** A simple and fast delegate implementation */
    template<typename R, typename ...A>
    class delegate<R (A...)> {
    using ptr_type = R (*)(void*, A&&...);
    public:
        /** Constructor, zero's object and function pointer */
        delegate() : mObj(nullptr), mFunc(nullptr) { }

        /** Default copy constructor. */
        delegate(const delegate&) = default;

        /** Default move constructor */
        delegate(delegate&&) = default;

        /** Default destructor */
        ~delegate() = default;

        /** Default copy assignment operator */
        delegate& operator=(const delegate&) = default;

        /** Compares two delegates with each other */
        bool operator== (const delegate &d1) {
            return (d1.mObj == mObj && d1.mFunc == mFunc);
        }

        /** Redirects all arguments to the function called and returns it's result */
        inline R operator()(A... args) const {
            return (*mFunc)(mObj, std::forward<A>(args)...);
        }

        /** Create a delegate from a member function */
        template <typename T, R (T::*method)(A...)>
        static delegate fromMember(T* obj) {
            delegate ret;
            ret.mObj = obj;
            ret.mFunc = &callFwd<T, method>;
            return ret;
        }
    private:
        /** Pointer to object. */
        void* mObj;
        /** Pointer to member function. */
        ptr_type mFunc;

        /** Internally used to forward the function call. */
        template <typename T, R (T::*method)(A...)>
        static R callFwd(void* mObj, A&&... args) {
            T* p = static_cast<T*>(mObj);
            return (p->*method)(std::forward<A>(args)...);
        }
    };
}

#endif	/* _ALICE_UTIL_DELEGATE_HPP_ */
