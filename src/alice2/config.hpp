/**
 * @file config.hpp
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

#ifndef _ALICE_CONFIG_HPP_
#define _ALICE_CONFIG_HPP_

/// Header for demo version 1
#define ALICE_S1_HEADER "PBUFDEM"_chash
/// Header for demo version 2
#define ALICE_S2_HEADER "PBDEMS2"_chash

/// Number of bytes to allocate for decompression
#define ALICE_SNAPPY_BUFFER_SIZE 102400

#endif /* _ALICE_CONFIG_HPP_ */