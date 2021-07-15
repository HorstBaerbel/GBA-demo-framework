#pragma once

#include <gba_base.h>
#include <cstdint>

namespace MemCtrl
{

    /// @brief Register for Game Pak SRAM and ROM wait states
    inline auto &RegWaitCnt{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0204)};

    /// @brief Minimum wait states possible for Game Pak SRAM and ROM
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint16_t WaitCntFast = 0x46DA;

    /// @brief Regular wait states for Game Pak SRAM and ROM
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint16_t WaitCntNormal = 0x4317;

    /// @brief Register for EWRAM wait states
    inline auto &RegWaitEwram{*reinterpret_cast<volatile uint32_t *>(REG_BASE + 0x0800)};

    /// @brief Wait states for EWRAM that crash the GBA (1/1/2)
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint32_t WaitEwramLudicrous = 0x0F000020;

    /// @brief Minimum wait states possible for EWRAM (2/2/4) on GBA and SP (not Micro or DS)
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint32_t WaitEwramFast = 0x0E000020;

    /// @brief Regular wait states for EWRAM (3/3/6)
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint32_t WaitEwramNormal = 0x0D000020;

    /// @brief Find best usable EWRAM wait states without memory corruption
    /// At the start of your program, do RegWaitEwram = BestEWRAMWaitstates();
    /// @note Will reset wait states to WaitEwramNormal
    uint32_t BestEWRAMWaitstates();

}
