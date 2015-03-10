/**
 * @file exception.cpp
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
#include "../../../src/alice2/util/exception.hpp"

using namespace alice;

ALICE_CREATE_EXCEPTION(MyException, "MyMessage"); 

TEST_CASE( "Exception handling", "[util/exception.hpp]" ) {
    // Check the base object
    alice::exception e;
    e << "This" << "Is" << "Data" << 123;
    REQUIRE(e.data() == "ThisIsData123");

    // Check the exception without any parameters
    MyException e2;
    REQUIRE(strcmp(e2.what(), "MyMessage") == 0);

    // Throw and catch an exception and see if the data matches
    try {
        throw MyException();
    } catch (MyException &e3) {
        REQUIRE(strcmp(e3.what(), "MyMessage") == 0);
    }

    // Add some data and throw
    try {
        ALICE_THROW(MyException, "Anything" << "Goes" << 123);
    } catch (MyException &e4) {
        REQUIRE(strcmp(e4.what(), "MyMessageAnythingGoes123") == 0);
    }
}