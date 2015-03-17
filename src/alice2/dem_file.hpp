/**
 * @file dem_file.hpp
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

#ifndef _ALICE_DEM_FILE_HPP_
#define _ALICE_DEM_FILE_HPP_

#include <string>
#include <cstddef>

#include "util/exception.hpp"
#include "util/noncopyable.hpp"
#include "dem.hpp"

namespace alice {
    /// Thrown when the parser fails to open the specified file
    ALICE_CREATE_EXCEPTION(DemFileIO, "Unable to open file");
    /// Thrown when the file being parsed is to small
    ALICE_CREATE_EXCEPTION(DemFileSize, "File to small");
    /// Invalid file format
    ALICE_CREATE_EXCEPTION(DemInvalid, "Invalid file format or header corrupt");

    /** Class representation of a single demo file */
    class dem_file : private noncopyable {
    public:
        /** Default move constructor */
        dem_file(dem_file&&) = default;
        /** Default move assignment operator */
        dem_file& operator=(dem_file&&) = default;

        /** Loads specified file into memory to be parsed */
        dem_file(const char* path);
        /** Read from the provided buffer */
        dem_file(char* data, std::size_t size);
 
        /** Destructor */
        ~dem_file();

        /** Returns a single dem packet */
        dem_packet get();
        /** Whether there is still data left to read */
        bool good();
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

        /** Whether we own the underlying buffer */
        bool ownsBuffer;

        /** Verifies the file signature and detects the correct engine */
        void parse_header();
        /** Reads a single integer from the buffer */
        uint32_t read_varint();
    };
}

#endif /* _ALICE_DEM_FILE_HPP_ */