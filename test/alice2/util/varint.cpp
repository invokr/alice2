/**
 * @file varint.cpp
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
#include "../../../src/alice2/util/varint.hpp"

using namespace alice;

TEST_CASE( "varint", "[util/varint.hpp]" ) {
    uint8_t read;

    // Try reading from a char array
    unsigned char c[] = {0x9E, 0xA7, 0x05};
    const char* c2 = reinterpret_cast<const char*>(c);

    REQUIRE(readVarUInt32(c2, read, 3) == 86942);
    REQUIRE(read == 3);

    // Try reading from a std::string
    std::string av2 = {(char)0x8E, 0x02};

    std::string av3 = {(char)0x96, 0x01};
    REQUIRE(readVarUInt64(av3, read) == 150);

    // Try the 64 version
    REQUIRE(readVarUInt64(av2, read) == 270);
    REQUIRE(read == 2);

    // Write a varint
    uint8_t cWrite[3];
    writeVarUInt32(&cWrite[0], 86942);

    REQUIRE(cWrite[0] == 0x9E);
    REQUIRE(cWrite[1] == 0xA7);
    REQUIRE(cWrite[2] == 0x05);

    // Try the fast 32 bit version
    uint32_t res = 0;
    uint8_t *rBuf = readVarUInt32_fast((uint8_t*)c2, res);
    REQUIRE(res == 86942);
    REQUIRE((rBuf - (uint8_t*)c2) == 3);

    // Try the fast 64 bit version
    uint64_t res64 = 0;
    readVarUInt64_fast((uint8_t*)c2, res64); 
    REQUIRE(res64 == 86942);
}