/**
 * @file constexpr_hash.hpp
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

#ifndef _UTIL_CONSTEXPR_HASH_HPP_
#define _UTIL_CONSTEXPR_HASH_HPP_

#include <cstring>

namespace alice {
    namespace detail {
        // FNV-1a constants
        static constexpr unsigned long long constexpr_hash_basis = 14695981039346656037ULL;
        static constexpr unsigned long long constexpr_hash_prime = 1099511628211ULL;

        /** Hashes a single character at compile time */
        constexpr unsigned long long hash_single(char c, const char* remain, unsigned long long value) {
            return c == 0
                ? value
                : hash_single(remain[0], remain + 1, (value ^ c) * constexpr_hash_prime);
        }
    }

    /** Computes compile-time hash of string. str has to be NULL-terminated. */
    constexpr unsigned long long constexpr_hash( const char* str ) {
        return detail::hash_single(str[0], str+1, detail::constexpr_hash_basis);
    }

    /** Computes the same hash as constexpr_hash at runtime */
    unsigned long long constexpr_hash_rt( const char* str ) {
        unsigned long long hash = detail::constexpr_hash_basis;

        while (*str != 0) {
            hash ^= str[0];
            hash *= detail::constexpr_hash_prime;
            ++str;
        }

        return hash;
    }

    /** User defined literal to allow "str"_chash-style invocation */
    constexpr long long operator"" _chash( const char* str, size_t n ) {
        return constexpr_hash( str );
    }
}

#endif /* _UTIL_CONSTEXPR_HASH_HPP_ */