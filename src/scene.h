#pragma once

#include "math/fp32.h"
#include "sound/effect.h"

// Define this to passs information about currently
// pressed or held keys via the scene data
//#define KEYSDOWN_IN_DATA

/// @brief Scene management
namespace Scene
{

    /// @brief Scene runtime data that will be passed to scene functions
    struct Data
    {
        Math::fp1616_t startTime = 0; /// Start time of the scene. Will be auto-filled.
        Math::fp1616_t duration = 0;  /// Duration of the scene. Will be auto-filled.
        Math::fp1616_t t = 0;         /// delta time running from 0->1 when loop() is called. Will be auto-filled.
#ifdef KEYSDOWN_IN_DATA
        uint16_t keysDown = 0; /// Currently pressed or held keys.
#endif
    } __attribute__((aligned(4), packed));

    /// @brief Defines a scene with functions to setup, loop and clean up
    struct Entry
    {
        const char *name;                      /// Name of the scene for debug purposes.
        Math::fp1616_t duration;               /// Duration of the scene in the demo.
        const Sound::Effect *effect = nullptr; /// Sound effect played at start of scene. Nullptr means no effect.
        int32_t songNr;                        /// Song that will be played when scene is running. -1 means nothing is done.
        int32_t songPosition;                  /// Song position that will be set when scene is running. -1 means nothing is done.
        void (*setup)(const Data &);           /// Initialize all your data / gfx etc. here when the scene starts.
        void (*loop)(const Data &);            /// Will called repeatedly until endTime has been reached.
        void (*cleanup)(const Data &);         /// Clean up all your data / gfx etc. here when the scene ends.
    } __attribute__((aligned(4), packed));

    /// @brief Starts play a sequence of scenes.
    void play(const Entry *entries, uint32_t nrOfEntries, uint32_t startIndex = 0);

} // namespace Scene
