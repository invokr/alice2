/**
 * @file ztime.cpp
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

#include <cstdint>
#include <cstddef>
#include <cstring>

#include <catch.hpp>

// Make sure we are testing the version under src/
#include "../../../src/alice2/dem.hpp"

using namespace alice;

TEST_CASE( "dem_packet", "[util/dem.hpp]" ) {
    char data[] = "IAmTheData";

    // Create a dem packet
    dem_packet p;
    p.tick = 123;
    p.type = 4;
    p.size = 10;
    p.data = &data[0];

    char* buf = new char[1024];
    auto bytes_written = dem_packet::to_buffer(p, buf, 1024);
    buf[bytes_written] = '\0';

    REQUIRE(bytes_written == 13); // 1 bytes for tick, type and size, 10 bytes for data

    dem_packet p2;
    auto bytes_read = dem_packet::from_buffer(p2, buf, 1024);

    REQUIRE(p2.tick == p.tick);
    REQUIRE(p2.type == p.type);
    REQUIRE(p2.size == p.size);
    REQUIRE(*p2.data == *p.data); // just make sure it points to the same memory

    REQUIRE(bytes_read == bytes_written);

    REQUIRE(p2.tick == 123);
    REQUIRE(p2.type == 4);
    REQUIRE(p2.size == 10);

    // let's try compression / decompression
    dem_packet::compress(p2);
    dem_packet::uncompress(p2, buf, 1024);

    // only the raw data is in the buffer this time, obviously
    // we just add the terminator so strcmp works. If the buffer contains garbage, a match will be
    // unlikely anyway
    buf[10] = '\0';

    REQUIRE(p2.tick == 123);
    REQUIRE(p2.type == 4);
    REQUIRE(p2.size == 10);
    REQUIRE(strcmp(p2.data, &data[0]) == 0);
}