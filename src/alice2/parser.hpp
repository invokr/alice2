/**
 * @file parser.hpp
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

#ifndef _ALICE_PARSER_HPP_
#define _ALICE_PARSER_HPP_

#include <string>
#include <cstddef>

#include "util/noncopyable.hpp"
#include "dem.hpp"

namespace alice {
    /** */
    class parser : private noncopyable {
    public:
        /** Default move constructor */
        parser(parser&&) = default;
        /** Default move assignment operator */
        parser& operator=(parser&&) = default;

        /** Loads specified file into memory to be parsed */
        parser(const char* path);

        /** Read from the provided buffer */
        parser(char* data, std::size_t size);

        /** Destructor */
        ~parser();
    private:
        /** Data buffer */
        char* data;
        /** Data buffer size */
        std::size_t dataSize;
        /** Current position */
        std::size_t dataPos;
        /** Buffer for uncompressed snappy data */
        char* dataSnappy;

        /** Engine this replay was created from */
        engine source_version;
        /** Offset of summary packet */
        uint32_t offset;

        /** Verifies the file signature and detects the correct engine */
        void parse_header();
    };
}

#endif /* _ALICE_PARSER_HPP_ */