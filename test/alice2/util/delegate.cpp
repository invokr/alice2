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

class mTest1 {
public:
    mTest1() {

    }
    mTest1(const mTest1&& m) {
        // Include this to make sure perfect forwarding works
        REQUIRE("Delegate Perfect Forwarding" == 0);
    }

    mTest1(mTest1&&) = default;

    int add(int x, int y, mTest1 m) {
        return x + y;
    }
};

TEST_CASE( "delegate", "[util/delegate.hpp]" ) {
    mTest1 t1;
    mTest1 t2;
    auto d = delegate<int (int, int, mTest1)>::fromMember<mTest1, &mTest1::add>(&t1);

    REQUIRE(d(1, 2, std::move(t2)) == 3);
}