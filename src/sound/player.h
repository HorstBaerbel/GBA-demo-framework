#pragma once

#include "effect.h"
#include "fptime.h"

#include <cstdint>

/// @brief Module / effect player functions
/// You can add commands in the module by adding it SFx (or mod/xm EFx) commands.
/// Values above 0xFC will be processed in the following manner:
/// * 0xFD No action, but will be passed on to an external function you can specify in setSongEventHandler
/// * 0xFE Volume will be increased by VolumeMax / 2 and clamped to VolumeMax, bass set to 4
/// * 0xFF Volume will be increased by VolumeMax and clamped to VolumeMax, bass is set to 8
/// Volume values and bass will decrease over time
/// This lets you indicate beats in the audio without any audio analysis, or switch scenes / effects etc.
///
/// To generate a soundbank for the init() function, add your .wav, .mod, .xm files to your
/// CMakeLists.txt, e.g. like so:
///
/// # List all the MaxMod-compatible music files in the ./music directory
/// file(GLOB_RECURSE MUSIC_FILES
///	    ./music/*
/// )
/// target_maxmod_file(${PROJECT_NAME}.elf ${MUSIC_FILES}) # Build soundbank file from music files
///
/// This will generate and auto-include the generated files into your project. Then:
///
/// #include "soundbank.h"
/// #include "soundbank_bin.h"
///
/// and call:
///
/// Sound::init((const void *)&soundbank_bin, MSL_NSONGS);
///
/// somewhere. You should be able to Sound::playSong()s now.
namespace Sound
{

    /// @brief Initialize player. Call this before using the player!
    /// @param soundbank MaxMod binary soundbank data from soundbank_bin.h
    /// @param nrOfSongs Number of songs in soundbank. Use MSL_NSONGS from soundbank.h
    void init(const void *soundbank, uint32_t nrOfSongs);

    /// @brief Get numer of files / modules available to player (from soundbank.bin)
    uint32_t getSongCount();

    /// @brief Get file / module currently beint played
    int32_t getCurrentSongNr();

    /// @brief If looping, the current playing song / module will repeat after it ended
    bool isLooping();

    /// @brief If true is passed the current playing song / module will repeat after it ended
    void setLooping(bool loop = false);

    /// @brief Get a slowly decreasing volume value
    /// @note Only usable if you add effect commands to your module, see above
    int32_t getVolume1s();

    /// @brief Get a faster decreasing volume value
    /// @note Only usable if you add effect commands to your module, see above
    int32_t getVolume025s();

    /// @brief Maximum possible volume value
    constexpr int32_t VolumeMax = 65535;

    /// @brief Check if a bass was detected
    /// @note Only usable if you add effect commands to your module, see above
    bool isBass();

    /// @brief Length of sound buffer returned in waveBuffer()
    uint32_t waveBufferLength();

    /// @brief Wave buffer data from player
    const uint8_t *waveBuffer();

    /// @brief Play sound effect from soundbank.bin
    Effect::Handle playEffect(const Effect *effect);

    /// @brief Stop sound effect started via playEffect()
    void stopEffect(Effect::Handle handle);

    /// @brief Start playing song / module from soundbank.bin
    void playSong(int32_t fileNr);

    /// @brief Search to the song position indicated by position
    void setSongPosition(uint32_t position);

    /// @brief Skip to previous song / module if any
    void skipPrevious();

    /// @brief Skip to previous song / module if any
    void skipNext();

    /// @brief Pause song / module playback. Sound effects will continue playing
    void pause();

    /// @brief Resume song / module playback
    void resume();

    /// @brief Stop song / module playback. Sound effects will continue playing
    void stop();

    /// @brief Set a function to call on song events, e.g. a song has finished or a message was received (see above)
    void setSongEventHandler(unsigned int (*handler)(const unsigned int message, unsigned int parameter));

} //namespace Sound
