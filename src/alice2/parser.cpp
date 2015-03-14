/**
 * @file parser.cpp
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

#include <fstream>

#include "util/constexpr_hash.hpp"
#include "util/delegate.hpp"
#include "util/noncopyable.hpp"

#include "config.hpp"
#include "parser.hpp"

namespace alice {
    parser::parser(const char* path)
        : data(nullptr), dataSize(0), dataPos(0), dataSnappy(new char[ALICE_SNAPPY_BUFFER_SIZE])
    {
        std::ifstream input(path, std::ios::in | std::ios::binary);

        if (!input.is_open())
            throw 1;

        const std::streampos fstart = input.tellg();
        input.seekg (0, std::ios::end);
        dataSize = input.tellg() - fstart;
        input.seekg(fstart);

        if (dataSize < 12)
            throw 2;

        // read everything into the buffer
        data = new char[dataSize];
        input.read(data, dataSize);
        input.close();

        // verify header
        parse_header();
    }

    parser::parser(char* data, std::size_t size)
        : data(data), dataSize(size), dataPos(0), dataSnappy(new char[ALICE_SNAPPY_BUFFER_SIZE])
    {
        // verify header
        parse_header();
    }

    parser::~parser() {
        if (data)
            delete[] data;

        if (dataSnappy)
            delete[] dataSnappy;
    }

    void parser::parse_header() {
        // load header
        dem_header head;
        memcpy((char*) &head, data, sizeof(dem_header));

        // set offset and version
        offset = head.offset;

        switch (constexpr_hash_rt(head.headerid)) {
            case ALICE_S1_HEADER:
                source_version = engine::sone;
                break;
            case ALICE_S2_HEADER:
                source_version = engine::stwo;
                break;
            default:
                throw 3;
        }

        // increase position
        dataPos += sizeof(dem_header);
    }
}