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
        char varIntBuffer<const char*&>::get() {
            assert(mPos != mSize); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }
        
        template<>
        char varIntBuffer<const unsigned char*&>::get() {
            assert(mPos != mSize); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }

        template<>
        char varIntBuffer<std::istream&>::get() {
            assert(mBuffer.good()); // Trying to read past buffer boundaries
            return mBuffer.get();
        }

        template<>
        char varIntBuffer<std::string&>::get() {
            assert(mPos != mBuffer.size()); // Trying to read past buffer boundaries
            return mBuffer[mPos++];
        }
    }

    /** Reads an uint32_t, encoded by protobuf */
    template <typename T>
    uint32_t readVarUInt32(T&& t, uint32_t& read, uint32_t size = 0, uint32_t position = 0) {
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
}

#endif /* _UTIL_VARINT_HPP_ */