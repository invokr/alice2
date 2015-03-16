/**
* @file varint.hpp
* @author Robin Dietrich <me (at) invokr (dot) org>
* @version 1.0
*
* @par License
*   Alice Replay Parser
*   Copyright 2014-2015 Robin Dietrich
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#ifndef _UTIL_VARINT_HPP_
#define _UTIL_VARINT_HPP_

#include <fstream>
#include <string>
#include <memory>
#include <cassert>

#include <iostream>

#include "noncopyable.hpp"

namespace alice {
    namespace detail {
        template <typename T>
        class varIntBuffer : private noncopyable {
        public:
            /** Constructor */
            varIntBuffer(T&& buffer, std::size_t size = 0, std::size_t position = 0)
                : mBuffer(std::forward<T>(buffer)), mSize(size), mPos(position) {}

            /** Default move constructor */
            varIntBuffer(varIntBuffer&&) = default;

            /** Default destructor */
            ~varIntBuffer() = default;

            /** Reads one char from the underlying data type, specialized for istreams. */
            char get();
        private:
            /** Internal buffer (string, array) to read from. */
            T&& mBuffer;
            /** Maximum size of buffer type */
            std::size_t mSize;
            /** Current position */
            std::size_t mPos;
        };

        template<>
        inline char varIntBuffer<const char*&>::get() {
            assert(mPos != mSize); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }

        template<>
        inline char varIntBuffer<char*&>::get() {
            assert(mPos != mSize); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }

        template<>
        inline char varIntBuffer<const unsigned char*&>::get() {
            assert(mPos != mSize); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }

        template<>
        inline char varIntBuffer <unsigned char*&>::get() {
            assert(mPos != mSize); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }

        template<>
        inline char varIntBuffer<std::istream&>::get() {
            assert(mBuffer.good()); // Trying to read past buffer boundaries
            return mBuffer.get();
        }

        template<>
        inline char varIntBuffer<std::string&>::get() {
            assert(mPos != mBuffer.size()); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }
    }

    /** Reads an uint32_t, encoded by protobuf */
    template <typename T>
    uint32_t readVarUInt32(T&& t, uint8_t& read, std::size_t size = 0, std::size_t position = 0) {
        uint32_t result = 0; // result
        char tmp;            // holds last read value
        read = 0;            // make sure read is zeroed

        // stream
        detail::varIntBuffer<T> buf(std::forward<T>(t), size, position);

        do {
            assert(read != 5); // Trying to push more than 4 bytes, corrupt?

            tmp = buf.get();
            result |= (uint32_t)(tmp & 0x7F) << ( 7 * read );
            ++read;
        } while (tmp & 0x80);

        return result;
    }

    /** Read a varint from a buffer, fast version. */
    inline uint8_t* readVarUInt32_fast(uint8_t* buffer, uint32_t& value) {
        uint8_t* ptr = buffer; // avoids indirection, indicates bytes read
        uint32_t b;
        uint32_t result = 0;

        assert(ptr);

        b = *(ptr++); result  = b      ; if (!(b & 0x80)) goto done;
        result -= 0x80;
        b = *(ptr++); result += b <<  7; if (!(b & 0x80)) goto done;
        result -= 0x80 << 7;
        b = *(ptr++); result += b << 14; if (!(b & 0x80)) goto done;
        result -= 0x80 << 14;
        b = *(ptr++); result += b << 21; if (!(b & 0x80)) goto done;
        result -= 0x80 << 21;
        b = *(ptr++); result += b << 28; if (!(b & 0x80)) goto done;

        // More than 5 bytes read, corrupt
        return nullptr;

        done:
            value = result;
            return ptr;
    }

    /** Writes a varint to the given buffer, requires size > 4 */
    inline uint8_t* writeVarUInt32(uint8_t* buffer, uint32_t value) {
        assert(buffer);

        buffer[0] = static_cast<uint8_t>(value | 0x80);
        if (value >= (1 << 7)) {
            buffer[1] = static_cast<uint8_t>((value >>  7) | 0x80);
            if (value >= (1 << 14)) {
                buffer[2] = static_cast<uint8_t>((value >> 14) | 0x80);
                if (value >= (1 << 21)) {
                    buffer[3] = static_cast<uint8_t>((value >> 21) | 0x80);
                    if (value >= (1 << 28)) {
                        buffer[4] = static_cast<uint8_t>(value >> 28);
                        return buffer + 5;
                    } else {
                        buffer[3] &= 0x7F;
                        return buffer + 4;
                    }
                } else {
                    buffer[2] &= 0x7F;
                    return buffer + 3;
                }
            } else {
                buffer[1] &= 0x7F;
                return buffer + 2;
            }
        } else {
            buffer[0] &= 0x7F;
            return buffer + 1;
        }
    }

    /** Reads an uint64_t, encoded by protobuf */
    template <typename T>
    inline uint64_t readVarUInt64(T&& t, uint8_t& read, std::size_t size = 0, std::size_t position = 0) {
        uint64_t result = 0; // result
        char tmp;            // holds last read value
        read = 0;            // make sure read is zeroed

        // stream
        detail::varIntBuffer<T> buf(std::forward<T>(t), size, position);

        do {
            assert(read != 10); // Trying to push more than 4 bytes, corrupt?

            tmp = buf.get();
            result |= (uint64_t)(tmp & 0x7F) << ( 7 * read );
            ++read;
        } while (tmp & 0x80);

        return result;
    }

    /** Read a varint from a buffer, fast version. */
    inline uint8_t* readVarUInt64_fast(uint8_t* buffer, uint64_t& value) {
        uint8_t* ptr = buffer; // avoids indirection, indicates bytes read
        uint32_t b;
        uint32_t part0 = 0, part1 = 0, part2 = 0;

        b = *(ptr++); part0  = b      ; if (!(b & 0x80)) goto done;
        part0 -= 0x80;
        b = *(ptr++); part0 += b <<  7; if (!(b & 0x80)) goto done;
        part0 -= 0x80 << 7;
        b = *(ptr++); part0 += b << 14; if (!(b & 0x80)) goto done;
        part0 -= 0x80 << 14;
        b = *(ptr++); part0 += b << 21; if (!(b & 0x80)) goto done;
        part0 -= 0x80 << 21;
        b = *(ptr++); part1  = b      ; if (!(b & 0x80)) goto done;
        part1 -= 0x80;
        b = *(ptr++); part1 += b <<  7; if (!(b & 0x80)) goto done;
        part1 -= 0x80 << 7;
        b = *(ptr++); part1 += b << 14; if (!(b & 0x80)) goto done;
        part1 -= 0x80 << 14;
        b = *(ptr++); part1 += b << 21; if (!(b & 0x80)) goto done;
        part1 -= 0x80 << 21;
        b = *(ptr++); part2  = b      ; if (!(b & 0x80)) goto done;
        part2 -= 0x80;
        b = *(ptr++); part2 += b <<  7; if (!(b & 0x80)) goto done;

        // More than 10 bytes read, corrupt
        return nullptr;

        done:
            value = (static_cast<uint64_t>(part0)) |
             (static_cast<uint64_t>(part1) << 28) |
             (static_cast<uint64_t>(part2) << 56);
            return ptr;
    }
}

#endif /* _UTIL_VARINT_HPP_ */