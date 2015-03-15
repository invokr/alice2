/**
 * @file dict.cpp
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
#include "../../../src/alice2/util/dict.hpp"

using namespace alice;

TEST_CASE( "dict", "[util/dict.hpp]" ) {
    typedef uint32_t value_t;

    dict<value_t> MyDict;
    MyDict.insert(0, "V1", 3);
    MyDict.insert(1, "V2", 1);
    MyDict.insert(10, "V3", 2);

    value_t v1 = MyDict.by_index(0).value;

    REQUIRE(MyDict.by_index(0).value == 3);
    REQUIRE(MyDict.by_index(0).key == "V1");
    REQUIRE(MyDict.by_index(1).value == 1);
    REQUIRE(MyDict.has_key("V3"));
    REQUIRE(MyDict.by_key("V3").value == 2);
}