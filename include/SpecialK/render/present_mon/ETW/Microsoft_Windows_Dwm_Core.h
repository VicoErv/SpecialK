﻿// Copyright (C) 2020-2021 Intel Corporation
// SPDX-License-Identifier: MIT
//
// This file originally generated by etw_list
//     version:    development branch e5985e637875db6cb6f90e8c92d0857b6fb95324
//     parameters: --no_event_structs --event=MILEVENT_MEDIA_UCE_PROCESSPRESENTHISTORY_GetPresentHistory::Info --event=SCHEDULE_PRESENT::Start --event=SCHEDULE_SURFACEUPDATE::Info --provider=Microsoft-Windows-Dwm-Core
#pragma once

namespace Microsoft_Windows_Dwm_Core {

struct __declspec(uuid("{9E9BBA3C-2E38-40CB-99F4-9E8281425164}")) GUID_STRUCT;
static const auto GUID = __uuidof(GUID_STRUCT);

// Win7 GUID added manually:
namespace Win7 {
struct __declspec(uuid("{8c9dd1ad-e6e5-4b07-b455-684a9d879900}")) GUID_STRUCT;
static const auto GUID = __uuidof(GUID_STRUCT);
}

enum class Keyword : uint64_t {
    DwmCore                               = 0x1,
    DetailedFrameInformation              = 0x2,
    FrameDrawInconsistencyInformation     = 0x4,
    DwmFrameRate                          = 0x8,
    AnimationScreenOff                    = 0x10,
    ProcessAttribution                    = 0x20,
    ComputeScribble                       = 0x40,
    FrameVisualization                    = 0x10000,
    LowOverheadSmoothnessTracking         = 0x20000,
    FrameVisualizationExtra               = 0x40000,
    DwmHolographic                        = 0x80000,
    WorkPerFrame                          = 0x100000,
    DwmInput                              = 0x200000,
    DwmDiagTrack                          = 0x400000,
    WorkPerFrameVerbose                   = 0x800000,
    win_ResponseTime                      = 0x1000000000000,
    Microsoft_Windows_Dwm_Core_Diagnostic = 0x8000000000000000,
};

enum class Level : uint8_t {
    win_Error         = 0x2,
    win_Informational = 0x4,
    win_Verbose       = 0x5,
};

enum class Channel : uint8_t {
    Microsoft_Windows_Dwm_Core_Diagnostic = 0x10,
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

EVENT_DESCRIPTOR_DECL(MILEVENT_MEDIA_UCE_PROCESSPRESENTHISTORY_GetPresentHistory_Info, 0x0040, 0x00, 0x10, 0x05, 0x00, 0x003f, 0x8000000000000001);
EVENT_DESCRIPTOR_DECL(SCHEDULE_PRESENT_Start                                         , 0x000f, 0x00, 0x10, 0x04, 0x01, 0x000d, 0x8000000000000001);
EVENT_DESCRIPTOR_DECL(SCHEDULE_SURFACEUPDATE_Info                                    , 0x00c4, 0x00, 0x10, 0x04, 0x00, 0x007f, 0x8000000000000001);

// These events added manually:
EVENT_DESCRIPTOR_DECL(FlipChain_Complete                                             , 0x0046, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x0000000000000000);
EVENT_DESCRIPTOR_DECL(FlipChain_Dirty                                                , 0x0065, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x0000000000000000);
EVENT_DESCRIPTOR_DECL(FlipChain_Pending                                              , 0x0045, 0x00, 0x00, 0x00, 0x00, 0x0000, 0x0000000000000000);

#undef EVENT_DESCRIPTOR_DECL

}