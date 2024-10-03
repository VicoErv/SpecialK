﻿/*
Copyright 2020 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// This file originally generated by etw_list
//     version:    development branch 4676a0924b2a2d446f58e1104732a2553a4ca60d
//     parameters: --show=all --output=c++ --event=Present::Start --event=Present::Stop --provider=Microsoft-Windows-D3D9

#include <SpecialK/stdafx.h>

namespace Microsoft_Windows_D3D9 {

struct __declspec(uuid("{783ACA0A-790E-4D7F-8451-AA850511C6B9}")) GUID_STRUCT;
static const auto GUID = __uuidof(GUID_STRUCT);

enum class Keyword : uint64_t {
    Events                               = 0x2,
    Microsoft_Windows_Direct3D9_Analytic = 0x8000000000000000,
};

enum class Level : uint8_t {
    win_LogAlways = 0x0,
};

enum class Channel : uint8_t {
    Microsoft_Windows_Direct3D9_Analytic = 0x10,
};

// Event descriptors:
#define EVENT_DESCRIPTOR_DECL(name_, id_, version_, channel_, level_, opcode_, task_, keyword_)\
struct name_ {                                                                                 \
  static uint16_t const Id      =                       id_;                                   \
  static uint8_t  const Version =                  version_;                                   \
  static uint8_t  const Channel =                  channel_;                                   \
  static uint8_t  const Level   =                    level_;                                   \
  static uint8_t  const Opcode  =                   opcode_;                                   \
  static uint16_t const Task    =                     task_;                                   \
  static Keyword  const Keyword = { (enum Keyword)keyword_ };                                  \
};

EVENT_DESCRIPTOR_DECL(Present_Start, 0x0001, 0x00, 0x10, 0x00, 0x01, 0x0001, 0x8000000000000002);
EVENT_DESCRIPTOR_DECL(Present_Stop , 0x0002, 0x00, 0x10, 0x00, 0x02, 0x0001, 0x8000000000000002);

#undef EVENT_DESCRIPTOR_DECL

#pragma warning(push)
#pragma warning(disable: 4200) // nonstandard extension used: zero-sized array in struct

#pragma pack(push)
#pragma pack(1)

template<typename PointerT>
struct Present_Start_Struct {
    PointerT    pSwapchain;
    uint32_t    Flags;
};

struct Present_Stop_Struct {
    uint32_t    Result;
};

#pragma pack(pop)
#pragma warning(pop)

}
