#pragma once

#include <cstdint>

#include "base.h"

/// @brief Key Input register
#define REG_KEYINPUT *(volatile uint16_t *)(REG_BASE + 0x130)

/// @brief Keypad interrupt control register
#define REG_KEYCNT *(volatile uint16_t *)(REG_BASE + 0x132)

/// @brief Bit values for keypad buttons
typedef enum KEYPAD_BITS
{
    KEY_A = (1 << 0),                                 // keypad A button
    KEY_B = (1 << 1),                                 // keypad B button
    KEY_SELECT = (1 << 2),                            // keypad SELECT button
    KEY_START = (1 << 3),                             // keypad START button
    KEY_RIGHT = (1 << 4),                             // dpad RIGHT
    KEY_LEFT = (1 << 5),                              // dpad LEFT
    KEY_UP = (1 << 6),                                // dpad UP
    KEY_DOWN = (1 << 7),                              // dpad DOWN
    KEY_R = (1 << 8),                                 // Right shoulder button
    KEY_L = (1 << 9),                                 // Left shoulder button
    KEYIRQ_ENABLE = (1 << 14),                        // Enable keypad interrupt
    KEYIRQ_OR = (0 << 15),                            // interrupt logical OR mode
    KEYIRQ_AND = (1 << 15),                           // interrupt logical AND mode
    DPAD = (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT) // mask all dpad buttons
} KEYPAD_BITS;

/// @brief Call this function once per main loop in order to use the keypad functions
void scanKeys(void);

/// @brief Obtain the current keypad pressed state.
/// Returns the keys which have been pressed since the last call to KeysDown(), the keys are reset on this call.
/// Keys which are pressed will not be reported again until they are released.
uint16_t keysDown();
