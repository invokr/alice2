/**
 * @file bitstream.cpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 * @version 1.3
 *
 * @par License
 *    Alice Replay Parser
 *    Copyright 2014 Robin Dietrich
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
#include "bitstream.hpp"

namespace alice {
    namespace detail {
        constexpr uint64_t masks[64] = {
                0x0,             0x1,              0x3,              0x7,
                0xf,             0x1f,             0x3f,             0x7f,
                0xff,            0x1ff,            0x3ff,            0x7ff,
                0xfff,           0x1fff,           0x3fff,           0x7fff,
                0xffff,          0x1ffff,          0x3ffff,          0x7ffff,
                0xfffff,         0x1fffff,         0x3fffff,         0x7fffff,
                0xffffff,        0x1ffffff,        0x3ffffff,        0x7ffffff,
                0xfffffff,       0x1fffffff,       0x3fffffff,       0x7fffffff,
                0xffffffff,      0x1ffffffff,      0x3ffffffff,      0x7ffffffff,
                0xfffffffff,     0x1fffffffff,     0x3fffffffff,     0x7fffffffff,
                0xffffffffff,    0x1ffffffffff,    0x3ffffffffff,    0x7ffffffffff,
                0xfffffffffff,   0x1fffffffffff,   0x3fffffffffff,   0x7fffffffffff,
                0xffffffffffff,  0x1ffffffffffff,  0x3ffffffffffff,  0x7ffffffffffff,
                0xfffffffffffff, 0x1fffffffffffff, 0x3fffffffffffff, 0x7fffffffffffff
            };
    }

    uint32_t bitstream::read(const bitstream::size_type n) {
        // make sure the data read fits the return type
        if (n > size-pos)
            ALICE_THROW( bitstreamDataSize, pos << " (position) " << size << " (size) " << n << " (reading)");

        if (n > 32)
            ALICE_THROW( bitstreamDataSize, size);

        static constexpr uint32_t bitSize = sizeof(uint32_t) << 3; // size of chunk in bits
        const uint32_t start   = pos / bitSize;           // current active chunk
        const uint32_t end     = (pos + n - 1) / bitSize; // next chunk if data needs to be wrapped
        const uint32_t shift   = (pos % bitSize);         // shift amount
        uint32_t ret;                                     // return value

        if (start == end) {
            ret = (data[start] >> shift) & detail::masks[n];
        } else { // wrap around
            ret = ((data[start] >> shift) | (data[end] << (bitSize - shift))) & detail::masks[n];
        }

        pos += n;
        return ret;
    }

    uint32_t bitstream::readVarUInt32() {
        uint32_t readCount = 0;
        uint32_t value = 0;

        char tmpBuf;
        do {
            if (readCount == VARINT32_MAX) // return when maximum number of iterations is reached
                return value;

            tmpBuf = (char)read(8);
            value |= (uint32_t)(tmpBuf & 0x7F) << (7*readCount);
            ++readCount;
        } while (tmpBuf & 0x80);

        return value;
    }

    uint64_t bitstream::readVarUInt64() {
        uint32_t readCount = 0;
        uint64_t value = 0;

        uint32_t tmpBuf;
        do {
            if (readCount == VARINT64_MAX)
                return value;

            tmpBuf = read(8);
            value |= static_cast<uint64_t>(tmpBuf & 0x7F) << (7*readCount);
            ++readCount;
        } while (tmpBuf & 0x80);

        return value;
    }

    void bitstream::readString(char *buffer, const bitstream::size_type n) {
        for (std::size_t i = 0; i < n; ++i) {
            buffer[i] = static_cast<char>(read(8));

            if (buffer[i] == '\0')
                return;
        }

        buffer[n - 1] = '\0';
    }

    void bitstream::readBits(char *buffer, const bitstream::size_type n) {
        size_type remaining = n;
        size_type i = 0;

        while (remaining >= 8) {
            buffer[i++] = read(8);
            remaining -= 8;
        }

        if (remaining > 0)
            buffer[i++] = read(remaining);
    }
}