/**
 * @file dem.cpp
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

#include <cstring>
#include <cstdint>
#include <cstddef>

#include <snappy.h>

#include "util/expect.hpp"
#include "util/varint.hpp"
#include "proto/source2/demo.pb.h"

#include "dem.hpp"

namespace alice {
    void dem_packet::compress(dem_packet& msg) {
        // compress
        std::string compressed;
        snappy::Compress(msg.data, msg.size, &compressed);

        // assign data to msg
        char* nbuffer = new char[compressed.size()];
        memcpy(nbuffer, compressed.data(), compressed.size());
        msg.data = nbuffer;
        msg.size = compressed.size();
        msg.type = msg.type | ps2::DEM_IsCompressed;
    }

    void dem_packet::uncompress(dem_packet& msg, char* buffer, size_t buffer_size) {
        size_t size_uncompressed = 0;

        const char* data = msg.data;
        msg.data = nullptr;

        if (!snappy::IsValidCompressedBuffer(data, msg.size))
            return;

        if (!snappy::GetUncompressedLength(data, msg.size, &size_uncompressed))
            return;

        if (buffer_size < size_uncompressed)
            return;

        if (!snappy::RawUncompress(data, msg.size, buffer))
            return;

        msg.type = msg.type & ~ps2::DEM_IsCompressed;
        msg.data = buffer;
        msg.size = size_uncompressed;
    }

    size_t dem_packet::from_buffer(dem_packet& msg, char* buffer, size_t buffer_size) {
        if (expect(buffer_size > 15)) { // fast version
            uint8_t* data = reinterpret_cast<uint8_t*>(buffer);

            data = readVarUInt32_fast(data, msg.type);
            data = readVarUInt32_fast(data, msg.tick);
            data = readVarUInt64_fast(data, msg.size);

            msg.data = reinterpret_cast<const char*>(data);
            return (msg.data - buffer) + msg.size;
        } else { // slow version
            uint8_t r;
            char* buf = buffer;
            msg.type = readVarUInt32(buf, r, buffer_size); buf += r;
            msg.tick = readVarUInt32(buf, r, buffer_size); buf += r;
            msg.size = readVarUInt32(buf, r, buffer_size); buf += r;
            msg.data = buf;

           return (buf - buffer) + msg.size;
        }
    }

    size_t dem_packet::to_buffer(dem_packet& msg, char* buffer, size_t buffer_size, bool pack) {
        if (pack)
            compress(msg);

        // Account for the maximum serialization length instead of asserting twice
        assert(buffer_size >= (msg.size+15));

        uint8_t* buf = reinterpret_cast<uint8_t*>(buffer);
        buf = writeVarUInt32(buf, msg.type);
        buf = writeVarUInt32(buf, msg.tick);
        buf = writeVarUInt32(buf, msg.size);
        memcpy(buf, msg.data, msg.size);

        return ((buf+msg.size) - reinterpret_cast<uint8_t*>(buffer));
    }
}