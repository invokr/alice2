/**
 * @file expect.hpp
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

#ifndef _UTIL_EXPECT_HPP_
#define _UTIL_EXPECT_HPP_

// Using static branch prediction makes sense in some cases as it leads to better usage of the L1 cache

#ifdef __GNUC__
#define expect(__expr) __builtin_expect((__expr), !0)
#else
#define expect(__expr) __expr
#endif /* __GNUC */

#endif /* _UTIL_EXPECT_HPP_ */