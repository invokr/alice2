/**
 * @file ztime.hpp
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

#ifndef _UTIL_ZTIME_TEST_HPP_
#define _UTIL_ZTIME_TEST_HPP_

#include <chrono>
#include <thread>

#include <catch.hpp>

// Make sure we are testing the version under src/
#include "../../../src/alice2/util/ztime.hpp"

using namespace alice;

TEST_CASE( "ztime nanosecond resolution", "[util/ztime.hpp]" ) {
    auto ztime1 = getZTime();

    // need to sleep 1 ns or ztime1 and 2 might be equal
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    auto ztime2 = getZTime();

    REQUIRE( ztime1 != ztime2 );
    REQUIRE( ztime1 != 0 );
}

#endif /* _UTIL_ZTIME_TEST_HPP_ */