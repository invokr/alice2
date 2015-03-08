/**
 * @file ztime.hpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 * @version 1.1
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

#ifndef _UTIL_ZTIME_HPP_
#define _UTIL_ZTIME_HPP_

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

#include <cstdio>
#include <cstdint>

#define TIME_USEC_PER_SEC 1000000

namespace alice {
    /** Type representing the format for a usec */
    typedef uint64_t ztime_t;

    /**
     * Returns high resolution time for performance counters.
     *
     * The actual values returned by this function may differ depending
     * on the operating system.
     */
    inline ztime_t getZTime() {
        #ifdef WIN32
            ztime_t ztimep;
            QueryPerformanceCounter ((LARGE_INTEGER *) &ztimep);
            return ztimep;
        #else
            struct timeval tv;
            gettimeofday (&tv, NULL);
            return ((ztime_t) tv.tv_sec * TIME_USEC_PER_SEC) + tv.tv_usec;
        #endif
    }
}

#endif /* _UTIL_ZTIME_HPP_ */
