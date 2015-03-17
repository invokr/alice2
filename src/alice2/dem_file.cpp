/**
 * @file dem_file.cpp
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
#include <cstdint>
#include <cassert>

#include <snappy.h>

#include "proto/source2/demo.pb.h"

#include "util/constexpr_hash.hpp"
#include "util/delegate.hpp"
#include "util/noncopyable.hpp"
#include "util/varint.hpp"

#include "config.hpp"
#include "dem_file.hpp"

namespace alice {
    dem_file::dem_file(const char* path)
        : data(nullptr), dataSize(0), dataPos(0), dataSnappy(nullptr), ownsBuffer(true)
    {
        std::ifstream input(path, std::ios::in | std::ios::binary);

        if (!input.is_open())
            ALICE_THROW(DemFileIO, path);

        const std::streampos fstart = input.tellg();
        input.seekg (0, std::ios::end);
        dataSize = input.tellg() - fstart;
        input.seekg(fstart);

        if (dataSize < sizeof(dem_header))
            ALICE_THROW(DemFileSize, path);

        // read everything into the buffer
        data = new char[dataSize];
        input.read(data, dataSize);
        input.close();

        // verify header
        parse_header();

        if (source_version == engine::unkown) {
            delete[] data;
            ALICE_THROW(DemInvalid, path);
        }

        // create buffer for snappy
        dataSnappy = new char[ALICE_SNAPPY_BUFFER_SIZE];
    }

    dem_file::dem_file(char* data, std::size_t size)
        : data(data), dataSize(size), dataPos(0), dataSnappy(new char[ALICE_SNAPPY_BUFFER_SIZE]),
          ownsBuffer(false)
    {
        // verify header
        parse_header();

        if (source_version == engine::unkown) {
            delete[] dataSnappy;
            ALICE_THROW(DemInvalid, "Construction from buffer");
        }
    }

    dem_file::~dem_file() {
        if (data && ownsBuffer)
            delete[] data;

        if (dataSnappy)
            delete[] dataSnappy;
    }

    dem_packet dem_file::get() {
        assert(dataPos <= dataSize);

        dem_packet ret;
        dataPos += dem_packet::from_buffer(ret, data+dataPos, dataSize-dataPos);

        if (ret.type & ps2::DEM_IsCompressed)
            dem_packet::uncompress(ret, dataSnappy, ALICE_SNAPPY_BUFFER_SIZE);

        switch (ret.type)  {
            case ps2::DEM_ClassInfo:
                break;
            case ps2::DEM_SignonPacket:
            case ps2::DEM_Packet:
                break;
            case ps2::DEM_SendTables:
                break;
        }

        return ret;
    } 

    bool dem_file::good() {
        return (dataPos < dataSize);
    }

    void dem_file::parse_header() {
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

    uint32_t dem_file::read_varint() {
        uint8_t bytes_read = 0;
        const uint32_t ret = readVarUInt32(data, bytes_read, dataSize, dataPos);
        dataPos += bytes_read;
        return ret;
    }
}