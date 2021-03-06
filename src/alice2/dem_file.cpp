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
#include "packets.hpp"
#include "util/varint.hpp"

namespace alice {
    dem_file::dem_file(const char* path)
        : data(nullptr), dataSize(0), dataPos(0), dataSnappy(nullptr),
          packets(packet_list::instance()), ownsBuffer(true)
    {
        std::ifstream input(path, std::ifstream::in | std::ifstream::binary);

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
          packets(packet_list::instance()), ownsBuffer(false)
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
        dataPos += dem_packet::from_buffer(ret, data+dataPos, dataSize-dataPos, true);

        if (ret.type & ps2::DEM_IsCompressed)
            dem_packet::uncompress(ret, dataSnappy, ALICE_SNAPPY_BUFFER_SIZE);

        /** Reads type from the header */
        static auto read_type = [](bitstream &b) {
            uint32_t initial = b.read(6);
            uint32_t header  = initial >> 4; // low 2 bits for header

            if (header) {
                uint32_t bits = header * 4 + (((2 - header) >> 31) & 16);
                return (initial & 15) | (b.read( bits ) << 4);
            } else {
                return initial;
            }
        };

        switch (ret.type)  {
            case ps2::DEM_ClassInfo:
                break;
            case ps2::DEM_Packet: {
            case ps2::DEM_SignonPacket:
                // Both types are wrapped in a CDemoPacket
                ps2::CDemoPacket* packet = packets->get<ps2::CDemoPacket>(
                    PACKET_DEM, ret.type, ret.data, ret.size
                );

                // Read the packet
                bitstream stream(packet->data());
                while (stream.left() > 10) {
                    uint32_t type = read_type(stream);
                    uint32_t size = stream.readVarUInt32();

                    // create packet here
                    stream.seekForward(size << 3);
                }
            } break;
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
                packet_register_s1();
                break;
            case ALICE_S2_HEADER:
                source_version = engine::two;
                packet_register_s2();
                break;
            default:
                source_version = engine::unkown;
                break;
        }

        // increase position
        dataPos += sizeof(dem_header);
    }
}