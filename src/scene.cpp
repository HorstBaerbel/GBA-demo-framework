#include "scene.h"
#include "fptime.h"
#include "sound/player.h"

//#define DEBUG_SCENE
#ifdef DEBUG_SCENE
#include "debug_print.h"
#endif

// Define this to control scenes with the keypad (skip etc.)
//#define SCENE_CONTROL
#if defined(KEYSDOWN_IN_DATA) || defined(SCENE_CONTROL)
#include <gba_input.h>
#endif

namespace Scene
{

    void play(const Entry *entries, uint32_t nrOfEntries, uint32_t startIndex)
    {
        const Sound::Effect *currentEffect = nullptr;
        int16_t currentSongNr = -1;
        int32_t currentSongPosition = -1;
        for (uint32_t i = startIndex; i < nrOfEntries; ++i)
        {
            const auto &entry = entries[i];
#ifdef DEBUG_SCENE
            printf("Playing scene %s", entry.name);
#endif
            // set up player
            if (currentSongNr != entry.songNr && entry.songNr >= 0)
            {
                //Sound::stopEffect();
                Sound::playSong(entry.songNr);
                currentSongNr = entry.songNr;
            }
            if (currentSongPosition != entry.songPosition && entry.songPosition >= 0)
            {
                Sound::setSongPosition(entry.songPosition);
                currentSongPosition = entry.songPosition;
            }
            if (currentEffect != entry.effect && entry.effect != nullptr)
            {
                //Sound::stopSong();
                Sound::playEffect(entry.effect);
                currentEffect = entry.effect;
            }
            // set up start / end time
            const Math::fp1616_t startTime = Time::now();
            const Math::fp1616_t endTime = startTime + entry.duration;
            // set up data
            Data sceneData;
            sceneData.startTime = startTime;
            sceneData.duration = entry.duration;
            sceneData.t = 0;
            // set up scene
            if (entry.setup != nullptr)
            {
#ifdef DEBUG_SCENE
                printf("Running setup");
#endif
                entry.setup(sceneData);
            }
            // run scene loop
            if (entry.loop != nullptr)
            {
#ifdef DEBUG_SCENE
                printf("Running loop");
#endif
                while (Time::now() < endTime || entry.duration <= 0)
                {
#if defined(KEYSDOWN_IN_DATA) || defined(SCENE_CONTROL)
                    scanKeys();
#ifdef KEYSDOWN_IN_DATA
                    sceneData.keysDown = keysDown() | keysHeld();
#endif
#endif
                    sceneData.t = Math::fp1616_t::ONE - (endTime - Time::now()) / sceneData.duration;
                    entry.loop(sceneData);
#ifdef SCENE_CONTROL
                    if (sceneData.keysDown & KEY_START && i > 0)
                    {
                        // skip to start
                        i = 0;
                        Time::setTime(sceneData.startTime);
#ifdef DEBUG_SCENE
                        printf("Skipping to first scene");
#endif
                        break;
                    }
                    else if (sceneData.keysDown & KEY_LEFT && i > 0)
                    {
                        // skip backwards
                        i--;
                        Time::setTime(sceneData.startTime);
#ifdef DEBUG_SCENE
                        printf("Skipping to scene %d", i);
#endif
                        break;
                    }
                    else if (sceneData.keysDown & KEY_RIGHT && i < nrOfEntries - 1)
                    {
                        // skip forward
                        i++;
                        Time::setTime(sceneData.startTime);
#ifdef DEBUG_SCENE
                        printf("Skipping to scene %d", i);
#endif
                        break;
                    }
#endif
                }
            }
            // set up scene
            if (entry.cleanup != nullptr)
            {
#ifdef DEBUG_SCENE
                printf("Running cleanup");
#endif
                entry.cleanup(sceneData);
            }
        }
    }

} // namespace Scene