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
        : data(nullptr), dataSize(0), dataPos(0), dataSnappy(nullptr), ownsBuffer(true)
    {
        std::ifstream input(path, std::ios::in | std::ios::binary);

        if (!input.is_open())
            ALICE_THROW(ParserFileIO, path);

        const std::streampos fstart = input.tellg();
        input.seekg (0, std::ios::end);
        dataSize = input.tellg() - fstart;
        input.seekg(fstart);

        if (dataSize < sizeof(dem_header))
            ALICE_THROW(ParserFileSize, path);

        // read everything into the buffer
        data = new char[dataSize];
        input.read(data, dataSize);
        input.close();

        // verify header
        parse_header();

        if (source_version == engine::unkown) {
            delete[] data;
            ALICE_THROW(ParserInvalid, path);
        }

        // create buffer for snappy
        dataSnappy = new char[ALICE_SNAPPY_BUFFER_SIZE];
    }

    parser::parser(char* data, std::size_t size)
        : data(data), dataSize(size), dataPos(0), dataSnappy(new char[ALICE_SNAPPY_BUFFER_SIZE]),
          ownsBuffer(false)
    {
        // verify header
        parse_header();

        if (source_version == engine::unkown) {
            delete[] dataSnappy;
            ALICE_THROW(ParserInvalid, "Construction from buffer");
        }
    }

    parser::~parser() {
        if (data && ownsBuffer)
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
                source_version = engine::one;
                break;
            case ALICE_S2_HEADER:
                source_version = engine::two;
                break;
            default:
                source_version = engine::unkown;
                break;
        }

        // increase position
        dataPos += sizeof(dem_header);
    }
}