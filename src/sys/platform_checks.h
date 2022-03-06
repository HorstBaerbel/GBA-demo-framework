#pragma once

#include <cstdint>

namespace PlatformChecks
{

    /// @brief Check for VBA emulator
    bool isHardwareVBA();

    /// @brief Find best usable EWRAM wait states without memory corruption
    /// At the start of your program, do MemCtrl::RegWaitEwram = BestEWRAMWaitstates();
    /// @note Will reset wait states to MemCtrl::WaitEwramNormal
    uint32_t BestEWRAMWaitstates();

    /// @brief Find best usable ROM wait states without data corruption
    /// At the start of your program, do MemCtrl::RegWaitCnt = BestROMWaitstates();
    /// @note Will reset wait states to MemCtrl::WaitEwramNormal
    uint32_t BestROMWaitstates();

}
