/**
 * @file expect.cpp
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

#include <catch.hpp>
#include "../../../src/alice2/util/expect.hpp"

TEST_CASE( "expect", "[util/expect.hpp]" ) {
    uint32_t i = 999;
    if (expect(i == 999)) {
        // everything ok
    } else {
        // fail because expect should resolve to (expr) even if builtin_expect is not defined
        REQUIRE( 1 != 1 );
    }
}