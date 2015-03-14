/**
 * @file dem.hpp
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

#ifndef _DEM_MESSAGE_HPP_
#define _DEM_MESSAGE_HPP_

#include <fstream>
#include <cstring>
#include <cstdint>

#include "util/varint.hpp"

namespace alice {
    /** Engine the replay was played in */
    enum class engine {
        sone = 1,
        stwo
    };

    /** DEM file header, used for verification purposes */
    struct dem_header {
        /** Used for verification purposes, needs to equal DOTA_HEADERID */
        char headerid[ 8 ];
        /** Points to the location of the game summary */
        int32_t offset;
    };

    /** A single dem message */
    struct dem_message {
        /** Tick this messages was emitted at */
        uint32_t tick;
        /** Message type */
        uint32_t type;
        /** Size of data */
        std::size_t size;
        /** Points to start of message */
        const char* data;
    };
}

#endif /* _DEM_MESSAGE_HPP_ */