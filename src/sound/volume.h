#pragma once

#include "player.h"

#include <cstdint>

/// @brief Volume calculation functions to use inconjunction with player
/// You can add commands in the module by adding it SFx (or mod/xm EFx) commands.
/// Values above 0xFC will be processed in the following manner:
/// * 0xFD No action
/// * 0xFE Volume will be increased by VolumeMax / 2 and clamped to VolumeMax, bass set to 4
/// * 0xFF Volume will be increased by VolumeMax and clamped to VolumeMax, bass is set to 8
/// Volume values and bass will decrease over time. This lets you indicate beats in the audio without any audio analysis, or switch scenes / effects etc.
/// To make this work, initialize and register the song event handler somewhere:
///
/// Volume::init();
/// Sound::callAtSongEvent(Volume::handleSongEvent);
///
/// somewhere.
namespace Volume
{

    /// @brief Initialize volume calculation. Call this before using!
    void init();

    /// @brief Maximum possible volume value
    constexpr int32_t VolumeMax = 65535;

    /// @brief Get a slowly decreasing volume value
    /// @note Only usable if you add effect commands to your module, see above
    int32_t getVolume1s();

    /// @brief Get a faster decreasing volume value
    /// @note Only usable if you add effect commands to your module, see above
    int32_t getVolume025s();

    /// @brief Check if a bass was detected
    /// @note Only usable if you add effect commands to your module, see above
    bool isBass();

    /// @brief Process song volume messages from player
    void handleSongEvent(const Sound::SongEvent &event);

} //namespace Sound
