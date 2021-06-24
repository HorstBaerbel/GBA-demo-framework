#pragma once

#include <cstdint>

namespace Sound
{

    /// @brief Define a sound effect to be played using Sound::playEffect()
    struct Effect
    {
        using Handle = uint16_t;

        uint32_t id = 0;         // Sound effect identifier from soundbank.h
        uint16_t rate = 1 << 10; // playback rate as 6.10 fixed-point factor
        Handle handle = 0;       // internal. leave at 0
        uint8_t volume = 255;    // volume 0->255
        uint8_t panning = 127;   // left to right, 127 is center
    };

}