#pragma once

#include "effect.h"
#include "math/fp32.h"
#include "spectrum.h"

#include <cstdint>

/// @brief Full-fledged module / effect player using Maxmod
///
/// ----- General info -----
/// Maxmod always plays as stereo. It uses Timer 0, DMA 1 + 2.
/// Its mixxing and wave buffers are put into IWRAM. Size depends on the mxing frequency and number of supported channels.
/// It also needs to be called on Vblank consistently, thus it is called on Vblank IRQ. All other Vblank handler must come after that.
///
/// ----- Generating sound files -----
/// To generate a soundbank for the init() function, add your .wav, .mod, .xm files to your
/// CMakeLists.txt, e.g. like so:
///
/// # List all the Maxmod-compatible music files in the ./music directory
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
/// ModulePlayer::init((const void *)&soundbank_bin, MSL_NSONGS);
///
/// somewhere. You should be able to ModulePlayer::playSong()s now.
///
/// ----- Song events for synchronization / volume information -----
/// The player will send events when a song is started, has ended or was paused.
/// You can also add messages to the module by adding SFx (or mod/xm EFx) effects. These values will be forwarded as song events too.
/// To receive song events, register a handler (up to 4) using:
///
/// ModulePlayer::callAtSongEvent(yourHandlerFunction);
///
/// When you don't need the handler anymore, deregister it using:
///
/// ModulePlayer::removeAtSongEvent(yourHandlerFunction);
namespace ModulePlayer
{
    /// @brief Song loop mode
    enum class LoopMode : uint8_t
    {
        None,    // Default. No looping
        LoopAll, // Loop all songs in soundbank
        LoopOne  // Loop single song
    };

    /// @brief Player mixing frequency
    enum class MixFrequency : uint8_t
    {
        Frequency_8kHz,  // = 8121 Hz
        Frequency_10kHz, // = 10512 Hz
        Frequency_13kHz, // = 13379 Hz
        Frequency_16kHz, // = 15768 Hz
        Frequency_18kHz, // = 18157 Hz
        Frequency_21kHz, // = 21024 Hz
        Frequency_27kHz, // = 26758 Hz
        Frequency_31kHz  // = 31536 Hz
    };

    /// @brief Initialize player. Call this before using the player!
    /// @param soundbank Maxmod binary soundbank data from soundbank_bin.h
    /// @param nrOfSongs Number of songs in soundbank. Use MSL_NSONGS from soundbank.h
    /// @param frequency Mixing frequency of player. Higher = Higher quality, but higher memory and CPU usage
    /// @param channels Overall combined module and effect channels. Maximum is 32.
    void init(const void *soundbank, uint32_t nrOfSongs, MixFrequency frequency = MixFrequency::Frequency_21kHz, uint32_t channels = 8);

    /// @brief Get numer of files / modules available to player (from soundbank.bin)
    uint32_t getSongCount();

    /// @brief Get file / module currently beint played
    int32_t getCurrentSongNr();

    /// @brief  Get the current song / soundbank loop mode
    LoopMode getLoopMode();

    /// @brief Set song / soundbank loop mode
    void setLoopMode(LoopMode mode);

    /// @brief Mixing frequency of player in Hz
    uint32_t getMixFrequencyHz();

    /// @brief Length of sound buffer returned in getWaveBuffer()
    uint32_t getWaveBufferLength();

    /// @brief Stereo mixing buffer data from player at mixing frequency
    const int8_t *getWaveBuffer();

    /// @brief Play sound effect from soundbank.bin
    Effect::Handle playEffect(const Effect *effect);

    /// @brief Stop sound effect started via playEffect()
    void stopEffect(Effect::Handle handle);

    /// @brief Start playing song / module from soundbank.bin
    void playSong(int32_t fileNr);

    /// @brief Search to the song position indicated by position
    void setSongPosition(uint32_t position);

    /// @brief Get the duration in s the current song has been playing
    Math::fp1616_t getPlayedSongDuration();

    /// @brief Skip to previous song / module if any
    void skipPrevious();

    /// @brief Skip to next song / module if any
    void skipNext();

    /// @brief Pause song / module playback. Sound effects will continue playing
    void pause();

    /// @brief Resume song / module playback
    void resume();

    /// @brief Stop song / module playback. Sound effects will continue playing
    void stop();

    //--- song events -----------------------------------------------------------------------------

    /// @brief Song event type that will be sent in song event handler
    enum class SongEvent : uint32_t
    {
        SongStarted, // Song has started playing (either through playSong(), by looping, skipping or resuming). Stores song # in parameter
        SongPaused,  // Song was paused. Stores song # in parameter
        SongResumed, // Song was resumed. Stores song # in parameter
        SongStopped, // Song has stopped playing (either through stop(), by looping, skipping or by finishing the last song). Stores song # in parameter
        SongMessage, // EFx or SFx message from song module. Will store value x in parameter
        BadType
    };

    /// @brief Song event handler function
    using SongEventHandler = void (*)(SongEvent event, int32_t parameter);

    /// @brief Set a function to call on song events, e.g. a song has started or finished or a message was received (see above)
    void callAtSongEvent(SongEventHandler handler);

    /// @brief Remove a function to be called on song events, e.g. a song has started or finished or a message was received (see above)
    void removeAtSongEvent(SongEventHandler handler);

    //--- spectrum -----------------------------------------------------------------------------

    /// @brief Calculate spectrum from Maxmod play buffer
    void updateSpectrum();

    /// @brief Get current spectrum. Use update() to calculate
    /// @param result Returned spectrum
    const Spectrum::Bands &getSpectrum();
}
