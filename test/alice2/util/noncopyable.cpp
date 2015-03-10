/**
 * @file noncopyable.cpp
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

#include <type_traits>
#include <catch.hpp>
#include "../../../src/alice2/util/noncopyable.hpp"

using namespace alice;

TEST_CASE( "noncopyable", "[util/noncopyable.hpp]" ) {
    class test : noncopyable {};

    REQUIRE( std::is_copy_constructible<test>::value == false );
    REQUIRE( std::is_trivially_copy_constructible<test>::value == false );
    REQUIRE( std::is_copy_assignable<test>::value == false );
    REQUIRE( std::is_trivially_copy_assignable<test>::value == false ); 
}