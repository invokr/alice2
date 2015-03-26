/**
 * @file bitstream.hpp
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
 *
 * @par Source
 *    You can find most of the things implemented in this bitstream in the public
 *    Source Engine SDK released by Valve.
 */

#ifndef _UTIL_BITSTREAM_HPP_
#define _UTIL_BITSTREAM_HPP_

/// Maximum number of bytes to read for a 32 bit varint (32/8 + 1)
#define VARINT32_MAX 5
/// Maximum number of bytes to read for a 64 bit varint (64/8 + 2)
#define VARINT64_MAX 10

#include <string>
#include <vector>

#include "exception.hpp"

namespace alice {
    /// Thrown when the stream overflows
    ALICE_CREATE_EXCEPTION( bitstreamOverflow, "More bits requested than available" );
    /// Thrown when data submited is to large
    ALICE_CREATE_EXCEPTION( bitstreamDataSize, "Data submitted is to large");

    /** Read-Only bitstream implementation */
    class bitstream {
        public:
            /** Type used to keep track of the stream position */
            typedef std::size_t size_type;

            /** Creates an empty bitstream */
            bitstream() : data{}, pos{0}, size{0} { }

            /** Creates a bitstream from a std::string */
            bitstream(const std::string &str) : data{}, pos{0}, size{str.size() << 3} {
                if (size > 0xffffffff)
                    ALICE_THROW( bitstreamDataSize, size);

                // Reserve the memory in beforehand so we can just memcpy everything
                data.resize((str.size() + 3) / 4 + 1);
                memcpy(&data[0], str.c_str(), str.size());
            }

            /** Copy-Constructor */
            bitstream(const bitstream& b) : data(b.data), pos(b.pos), size(b.size) {}

            /** Move-Constructor */
            bitstream(bitstream&& b) : data(std::move(b.data)), pos(b.pos), size(b.size) {
                b.data.clear();
                b.pos = 0;
                b.size = 0;
            }

            /** Destructor */
            ~bitstream() = default;

            /** Assignment operator */
            bitstream& operator= (bitstream t) {
                swap(t);
                return *this;
            }

            /** Swap this bitstream with given one */
            void swap(bitstream& b) {
                std::swap(data, b.data);
                std::swap(pos, b.pos);
                std::swap(size, b.size);
            }

            /** Checkes whether there is still data left to be read. */
            inline bool good() const {
                return pos < size;
            }

            /** Returns number of bits left */
            inline size_type left() const {
                return size - pos;
            }

            /** Returns the size of the stream in bits. */
            inline size_type end() const {
                return size;
            }

            /** Returns the current position of the stream in bits. */
            inline size_type position() const {
                return pos;
            }

            /** Sets bitstream position */
            inline void setPosition(size_t s) {
                pos = s;
            }

            /**
             * Returns result of reading n bits into an uint32_t.
             *
             * This function can read a maximum of 32 bits at once. If the amount of data requested
             * exceeds the remaining size of the current chunk it wraps around to the next one.
             */
            uint32_t read(const size_type n);

            /**
             * Seek n bits forward.
             *
             * If the resulting position would overflow, it is set to the maximum one possible.
             */
            void seekForward(const size_type n) {
                pos += n;

                if (pos > size)
                    pos = size;
            }

            /**
             * Seek n bits backward.
             *
             * If the resulting position would underflow, it is set to 0.
             */
            void seekBackward(const size_type n) {
                if ((pos - n) > pos) {
                    pos = 0;
                } else {
                    pos -= n;
                }
            }

            /**
             * Reads a variable sized uint32_t from the stream.
             *
             * A variable int is read in chunks of 8. The first 7 bits are added to return value
             * while the last bit is the indicator whether to continue reading.
             */
            uint32_t readVarUInt32();

            /** Reads a variable sized uint64_t from the stream. */
            uint64_t readVarUInt64();

            /**
             * Reads a variable sized int32_t from the stream.
             *
             * The signed version of the varint uses protobuf's zigzag encoding for
             * the sign part.
             */
            int32_t readVarSInt32() {
                const uint32_t value = readVarUInt32();
                return (value >> 1) ^ -static_cast<int32_t>(value & 1);
            }

            /** Reads a variable sized int64_t from the stream. */
            int64_t readVarSInt64() {
                const uint64_t value = readVarUInt64();
                return (value >> 1) ^ -static_cast<int64_t>(value & 1);
            }

            /**
             * Reads a null-terminated string into the buffer, stops once it reaches \0 or n chars.
             *
             * n is treated as the number of bytes to be read.
             * n can be arbitrarily large in this context. The underlying read method throws in case an overflow
             * happens.
             */
            void readString(char *buffer, const size_type n);

            /**
             * Reads the exact number of bits into the buffer.
             *
             * The function reads in chunks of 8 bit until n is smaller than that
             * and appends the left over bits
             */
            void readBits(char *buffer, const size_type n);
        private:
            /** Data to read from */
            std::vector<uint32_t> data;
            /** Current position in the vector in bits */
            size_type pos;
            /** Overall size of the data in bits */
            size_type size;
    };
}

#endif /* _UTIL_BITSTREAM_HPP_ */