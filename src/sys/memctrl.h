#pragma once

#include <gba_base.h>
#include <cstdint>

/*#ifndef REG_WAITCNT
#define REG_WAITCNT (*(volatile uint16_t *)(REG_BASE + 0x0204))
#define REG_WAITEWRAM (*(volatile uint32_t *)(REG_BASE + 0x0800))
#endif*/

/// @brief Register for Game Pak SRAM and ROM wait states
auto &RegWaitCnt{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0204)};

/// @brief Minimum wait states possible for Game Pak SRAM and ROM
/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
constexpr uint16_t WaitCntFast = 0x46DA;

/// @brief Regular wait states possible for Game Pak SRAM and ROM
/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
constexpr uint16_t WaitCntNormal = 0x4317;

/// @brief Register for EWRAM wait states
auto &RegWaitEwram{*reinterpret_cast<volatile uint32_t *>(REG_BASE + 0x0800)};

/// @brief Wait states for EWRAM that crash the GBA (1/1/3)
/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
constexpr uint32_t WaitEwramLudicrous = 0x0F000020;

/// @brief Minimum wait states possible for EWRAM (2/2/4)
/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
constexpr uint32_t WaitEwramFast = 0x0E000020;

/// @brief Regular wait states possible for EWRAM (3/3/6)
/// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
constexpr uint32_t WaitEwramNormal = 0x0D000020;
