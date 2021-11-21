#pragma once

#include "effect.h"
#include "fptime.h"

#include <cstdint>

/// @brief Module / effect player functions
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
///
/// The player will send events when a song is started, has ended or was paused.
/// You can also add messages to the module by adding SFx (or mod/xm EFx) effects. These values will be forwarded as song events too.
/// To receive song events, register a handler (up to 6) using:
///
/// Sound::callAtSongEvent(yourHandlerFunction);
///
/// When you don't need the handler anymore, deregister it using:
///
/// Sound::removeAtSongEvent(yourHandlerFunction);
///
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
    void setLooping(bool loop = true);

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

    /// @brief Skip to next song / module if any
    void skipNext();

    /// @brief Pause song / module playback. Sound effects will continue playing
    void pause();

    /// @brief Resume song / module playback
    void resume();

    /// @brief Stop song / module playback. Sound effects will continue playing
    void stop();

    //--- song events -----------------------------------------------------------------------------

    /// @brief Song event structure will be sent in song event handler
    struct SongEvent
    {
        enum class Type : uint16_t
        {
            SongStarted, // Song has started playing (either through playSong(), by looping, skipping or resuming). Stores song # in parameter
            SongPaused,  // Song was paused. Stores song # in parameter
            SongStopped, // Song has stopped playing (either through stop(), by looping, skipping or by finishing the last song). Stores song # in parameter
            SongMessage, // EFx or SFx message from song module. Will store value x in parameter
            BadType
        };
        Type type;
        int16_t parameter;
    } __attribute__((aligned(4), packed));

    /// @brief Song event handler function
    using SongEventHandler = void (*)(const SongEvent &event);

    /// @brief Set a function to call on song events, e.g. a song has started or finished or a message was received (see above)
    void callAtSongEvent(SongEventHandler handler);

    /// @brief Remove a function to be called on song events, e.g. a song has started or finished or a message was received (see above)
    void removeAtSongEvent(SongEventHandler handler);

} //namespace Sound
