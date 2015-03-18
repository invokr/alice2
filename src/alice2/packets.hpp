/**
 * @file packets.hpp
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

#ifndef _ALICE_PACKETS_HPP_
#define _ALICE_PACKETS_HPP_

#include <array>
#include <vector>
#include <functional>
#include <iostream>

// Source 1 files containing packets
#include "proto/source1/netmessages.pb.h"
#include "proto/source1/dota_usermessages.pb.h"
#include "proto/source1/demo.pb.h"
#include "proto/source1/netmessages.pb.h"

// Source 2 files containing packets
#include "proto/source2/netmessages.pb.h"
#include "proto/source2/dota_usermessages.pb.h"
#include "proto/source2/demo.pb.h"
#include "proto/source2/netmessages.pb.h"

namespace alice {
    /** Different possible packet types */
    enum packet_i {
        PACKET_DEM = 0,
        PACKET_NET = 1,
        PACKET_USR = 2
    };

    /** Same as packet_i, just typesafe / for interfaces */
    enum class packet {
        dem = PACKET_DEM,
        net = PACKET_NET,
        usr = PACKET_USR
    };

    /** A list of all possible packet types, used in conjuction with protobuf and is especially written for it */
    class packet_list {
    public:
        /** Resizes a packet slot to given number */
        void resize(unsigned type, size_t size) {
            auto &v = list[type];
            if (v.size() < size)
                v.resize(size);
        }

        /** Registers a new packet type */
        template <typename Obj>
        void add(unsigned type, unsigned subtype) {
            auto &v = list[type];
            if (v.size() <= subtype)
                v.resize(subtype+1);

            // We use non-capturing lambdas instead of util/delegate as those are optimized
            // for small functions, relying soley on the stack for this function
            v[subtype] = [](const char* data, size_t size) {
                Obj* msg = new Obj;
                msg->ParseFromArray(data, size);
                return (void*)msg;
           };
        }

        /** Creates packet from given type */
        template <typename Obj>
        Obj* get(unsigned type, unsigned subtype, const char* data, size_t size) {
            try {
                return reinterpret_cast<Obj*>(list[type][subtype](data, size));
            } catch (...) { // if subtype() is undefined, will throw
                return nullptr;
            }
        }

        /** Returns packet list */
        static packet_list* instance() {
            static auto instance = new packet_list;
            return instance;
        }
    private:
        // array of T vectors of factory functions
        std::array<
            std::vector<
                std::function<void* (const char* data, size_t size)>
            >, 3 // default to 3 packet slots
        > list;
    };

    /** Registers all source 1 packets */
    void packet_register_s1() {
        // There are still some linking issues
        // packet_list* p = packet_list::instance();
        // #include "packets.s1.hpp.inline"
    }

    /** Registers all source 2 packets */
    void packet_register_s2() {
        packet_list* p = packet_list::instance();
        #include "packets.s2.hpp.inline"
    }
}

#endif /* _ALICE_PACKETS_HPP_ */