/**
 * @file constexpr_hash.hpp
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

#ifndef _UTIL_CONSTEXPR_HASH_TEST_HPP_
#define _UTIL_CONSTEXPR_HASH_TEST_HPP_

#include <cstring>
#include <catch.hpp>

// Make sure we are testing the version under src/
#include "../../../src/alice2/util/constexpr_hash.hpp"

using namespace alice;

TEST_CASE( "constexpr_hash equality", "[util/constexpr_hash.hpp]" ) {
    auto hash1 = "HashMePlease"_chash; // From string literal
    auto hash2 = constexpr_hash_rt("HashMePlease"); // At compile time
    auto hash3 = constexpr_hash("HashMePlease");  // At run time

    REQUIRE( hash1 == hash2 );
    REQUIRE( hash2 == hash3 );
    REQUIRE( hash1 != 0 );
}

#endif /* _UTIL_CONSTEXPR_HASH_TEST_HPP_ */