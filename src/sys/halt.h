#pragma once

namespace Halt
{
    /// @brief Halt CPU until an interrupt occurs using SWI 02h (GBA) or SWI 06h (NDS/DSi)
    /// See: https://problemkaputt.de/gbatek.htm#bioshaltfunctions
    /// @note You will need to correctly set up interrutps for this!
    auto Halt() -> void;

    /// @brief Halt CPU until a VBlank interrupt occurs using SWI 05h
    /// See: https://problemkaputt.de/gbatek.htm#bioshaltfunctions
    /// @note You will need to correctly set up interrutps for this!
    auto VBlankIntrWait() -> void;
}
