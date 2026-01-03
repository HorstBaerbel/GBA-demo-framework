#pragma once

#include "base.h"

#include <cstdint>

namespace MemCtrl
{
    /// @brief Register for Game Pak SRAM and ROM wait states
    inline auto &RegWaitCnt{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0204)};

    /// @brief Minimum wait states possible for Game Pak SRAM and ROM
    /// WS0/ROM 2,1 clocks, SRAM 8 clocks, WS2/EEPROM 8,8 clocks, prefetch enabled
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint16_t WaitCntFast = 0x46DA;

    /// @brief Regular wait states for Game Pak SRAM and ROM
    /// WS0/ROM 3,1 clocks, SRAM 8 clocks, WS2/EEPROM 8,8 clocks, prefetch enabled
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint16_t WaitCntNormal = 0x4317;

    /// @brief Slow, default wait states possible for Game Pak SRAM and ROM for slow flash cards
    /// WS0/ROM 4,2 clocks, SRAM 8 clocks, WS2/EEPROM 8,8 clocks, prefetch enabled
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint16_t WaitCntDefault = 0x4303;

    /// @brief Check if the wait state value for ROM can be safely used.
    /// If so, sets that value, else falls back to WaitCntDefault.
    /// @param value Wait state value
    /// @return Returns true if the value is usable and was set, false otherwise.
    auto setWaitCnt(uint16_t value) -> bool;

    /// @brief Register for internal memory control
    inline auto &RegIntMemCnt{*reinterpret_cast<volatile uint32_t *>(REG_BASE + 0x0800)};

    /// @brief Wait states for EWRAM that crash the GBA (1/1/2)
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint32_t WaitEwramLudicrous = 0x0F000020;

    /// @brief Minimum wait states possible for EWRAM (2/2/4) on GBA and SP (not Micro or DS)
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint32_t WaitEwramFast = 0x0E000020;

    /// @brief Regular wait states for EWRAM (3/3/6)
    /// See: http://problemkaputt.de/gbatek.htm#gbasystemcontrol
    constexpr uint32_t WaitEwramNormal = 0x0D000020;
}
