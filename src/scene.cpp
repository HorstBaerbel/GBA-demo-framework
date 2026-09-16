#include "scene.h"
#include "sound/player.h"
#include "time.h"

// #define DEBUG_SCENE
#ifdef DEBUG_SCENE
#include "debug_print.h"
#endif

#if defined(KEYSDOWN_IN_DATA) || defined(SCENE_CONTROL)
#include "sys/input.h"
#endif

namespace Scene
{

    void play(const Entry *entries, uint32_t nrOfEntries, uint32_t startIndex, bool loopScene)
    {
        const Player::Effect *currentEffect = nullptr;
        int16_t currentSongNr = -1;
        int32_t currentSongPosition = -1;
        for (uint32_t sceneIndex = startIndex; sceneIndex < nrOfEntries; ++sceneIndex)
        {
            const auto &entry = entries[sceneIndex];
#ifdef DEBUG_SCENE
            printf("Playing scene %s", entry.name);
#endif
            // set up player
            if (currentSongNr != entry.songNr && entry.songNr >= 0)
            {
                // Player::stopEffect();
                Player::playSong(entry.songNr);
                currentSongNr = entry.songNr;
            }
            if (currentSongPosition != entry.songPosition && entry.songPosition >= 0)
            {
                Player::setSongPosition(entry.songPosition);
                currentSongPosition = entry.songPosition;
            }
            if (currentEffect != entry.effect && entry.effect != nullptr)
            {
                // Player::stopSong();
                Player::playEffect(entry.effect);
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
                printf("Running setup for scene %d", sceneIndex);
#endif
                entry.setup(sceneData);
            }
            // run scene loop
            if (entry.loop != nullptr)
            {
#ifdef DEBUG_SCENE
                printf("Running loop for scene %d", sceneIndex);
#endif
                while (Math::fp1616_t::fromRaw(Time::now()) < endTime || entry.duration <= 0 || loopScene)
                {
#if defined(KEYSDOWN_IN_DATA) || defined(SCENE_CONTROL)
                    scanKeys();
#ifdef KEYSDOWN_IN_DATA
                    sceneData.keysDown = keysDown() | keysHeld();
#endif
#endif
                    sceneData.t = Math::fp1616_t::ONE - (endTime - Math::fp1616_t::fromRaw(Time::now())) / sceneData.duration;
                    entry.loop(sceneData);
#ifdef SCENE_CONTROL
                    if (sceneData.keysDown & KEY_START && sceneIndex > 0)
                    {
                        // skip to start
                        sceneIndex = -1;
                        currentEffect = nullptr;
                        currentSongNr = -1;
                        currentSongPosition = -1;
#ifdef DEBUG_SCENE
                        printf("Skipping to first scene");
#endif
                        break;
                    }
                    else if (sceneData.keysDown & KEY_LEFT && sceneIndex > 0)
                    {
                        // skip backwards (sceneIndex will be increased in for statement)
                        sceneIndex -= 2;
#ifdef DEBUG_SCENE
                        printf("Skipping to scene %d", sceneIndex);
#endif
                        break;
                    }
                    else if (sceneData.keysDown & KEY_RIGHT && sceneIndex < nrOfEntries - 1)
                    {
                        // skip forward (sceneIndex will be increased in for statement)
#ifdef DEBUG_SCENE
                        printf("Skipping to scene %d", sceneIndex);
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