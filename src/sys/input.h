#pragma once

#include <cstdint>

#include "base.h"

namespace Input
{
    /// @brief Register for key input
    inline auto &RegKeyInput{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x130)};

    /// @brief Keypad interrupt control register
    inline auto &RegKeyCnt{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x132)};

    /// @brief Bit values for keypad buttons
    enum KeyBits : uint16_t
    {
        KeyA = (1 << 0),        // Keypad A button
        KeyB = (1 << 1),        // Keypad B button
        KeySelect = (1 << 2),   // Keypad select button
        KeyStart = (1 << 3),    // Keypad start button
        KeyRight = (1 << 4),    // Dpad right
        KeyLeft = (1 << 5),     // Dpad left
        KeyUp = (1 << 6),       // Dpad up
        KeyDown = (1 << 7),     // Dpad down
        KeyR = (1 << 8),        // Right shoulder button
        KeyL = (1 << 9),        // Left shoulder button
        IrqEnable = (1 << 14),  // Enable keypad interrupt (only use with RegKeyCnt)
        IrqCondOr = (0 << 15),  // Interrupt when one of the selected buttons is pressed (only use with RegKeyCnt)
        IrqCondAnd = (1 << 15), // Interrupt when all of the selected buttons are pressed (only use with RegKeyCnt)
    };

    /// @brief Any Dpad button
    constexpr KeyBits KeysDpad = static_cast<KeyBits>(KeyBits::KeyUp | KeyBits::KeyDown | KeyBits::KeyLeft | KeyBits::KeyRight);

    /// @brief Call this function once per main loop in order to use the keypad functions
    auto scanKeys() -> void;

    /// @brief Obtain the current keypad pressed state
    /// @return Returns the keys which have been pressed since the last call to keysDown(), the keys are reset on this call.
    /// Keys which are pressed will not be reported again until they are released.
    auto keysDown() -> KeyBits;

    /// @brief Wait for specific keys to be pressed
    /// @param keys Keys to be pressed
    /// @param halt If true, halt CPU to save power. If false, busy-wait.
    auto waitForKeysDown(KeyBits keys, bool halt = false) -> void;
}