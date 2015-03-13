/**
 * @file delegate.cpp
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

#include <iostream>
#include <cstring>
#include <cstddef>
#include <catch.hpp>
#include "../../../src/alice2/util/delegate.hpp"

using namespace alice;

// Normal test
TEST_CASE( "delegate", "[util/delegate.hpp]" ) {
    class mTest1 {
    public:
        int add(int x, int y) {
            return x + y;
        }
    };

    mTest1 t1;
    auto d = delegate<int (int, int)>::fromMember<mTest1, &mTest1::add>(&t1);
    REQUIRE(d(1, 2) == 3);
}

// See if move constructor is prefered over the copy constructor when moving
TEST_CASE( "delegate_perfect_forwarding", "[util/delegate.hpp]" ) {
    class mTest2 {
    public:
        mTest2() {}

        mTest2(const mTest2& m) {
            #pragma unused(m)
            REQUIRE("Perfect forwarding for delegates not working" == 0);
        }

        mTest2(mTest2&&) = default;

        int add(int x, int y, mTest2 m) {
            #pragma unused(m)
            return x + y;
        }
    };

    mTest2 t2;
    auto d = delegate<int (int, int, mTest2)>::fromMember<mTest2, &mTest2::add>(&t2);
    REQUIRE(d(4, 5, std::move(t2)) == 9);
}