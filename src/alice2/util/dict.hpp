/**
 * @file dict.hpp
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

#ifndef _UTIL_DICT_HPP_
#define _UTIL_DICT_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <cstddef>

namespace alice {
    /** Bi-directional map aka dictonary */
    template <typename Value>
    class dict {
    public:
        /** Structure for a single entry */
        struct entry_t {
            /** Numeric index of entry */
            std::size_t index;
            /** Key of entry */
            std::string key;
            /** Value */
            Value value;
        };

        /** Reserves memory for a certain number of entries */
        void reserve(uint32_t size) {
            entries.reserve(size);
        }

        /** Inserts a new entry */
        void insert(std::size_t index, std::string key, Value v) {
            if (entries.size() <= index)
                entries.resize(index + 1);

            entries[index] = entry_t{index, key, v};
            keyMap[key] = index;
        }

        /** Checks if specified key exists */
        bool has_key(const std::string& key) {
            return (keyMap.find(key) != keyMap.end());
        }

        /** Checks if index is valid */
        bool has_index(std::size_t idx) {
            return entries.size() > idx;
        }

        /** Returns entry by key */
        entry_t& by_key(const std::string& key) {
            return entries[keyMap[key]];
        }

        /** Returns entry by index */
        entry_t& by_index(std::size_t idx) {
            return entries[idx];
        }
    private:
        /** Key -> Index Mapping */
        std::unordered_map<std::string, std::size_t> keyMap;
        /** Index -> Entry Mapping */
        std::vector<entry_t> entries;
    };
}

#endif /* _UTIL_DICT_HPP_ */