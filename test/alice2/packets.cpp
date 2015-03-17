/**
 * @file packets.cpp
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
#include "../../../src/alice2/packets.hpp"

using namespace alice;

struct p1 {
    int add(int i, int j) {
        return i + j;
    }

    // We only use packet_list for protobuf, so this is required
    // Whether this function returns the correct result or not is not important
    // in regards to testing packet_list
    void ParseFromArray(const char*, size_t) {}
};

struct p2 {
    int substract(int i, int j) {
        return i - j;
    }

    void ParseFromArray(const char*, size_t) {}
};

TEST_CASE( "packets", "[packets.cpp]" ) {
    auto p = packet_list::instance();
    p->add<p1>(0, 0);
    p->add<p2>(0, 1);

    p1* pt1 = p->get<p1>(0, 0, nullptr, 0);
    p2* pt2 = p->get<p2>(0, 1, nullptr, 0);

    REQUIRE(pt1->add(1, 2) == 3);
    REQUIRE(pt2->substract(5, 4) == 1);
}