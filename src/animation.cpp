#include "animation.h"
#include "memory/memory.h"

//#define DEBUG_ANIMATION
#ifdef DEBUG_ANIMATION
#include "debug/print.h"
#endif

#include <gba_base.h>

namespace Animation
{

#define MAX_SEQUENCES 16
    Sequence sequences[MAX_SEQUENCES] EWRAM_DATA;
    uint16_t nrOfSequences EWRAM_DATA = 0;

    Sequence &addSequence(Math::fp1616_t start, void (*updateFunc)(Math::fp1616_t, const void *, const void *), uint8_t nrOfKeyframes, RepeatMode repeatMode, uint8_t repeatCount)
    {
        if (nrOfSequences < (MAX_SEQUENCES - 1))
        {
            auto &sequence = sequences[nrOfSequences];
            sequence.start = start;
            sequence.updateFunc = updateFunc;
            sequence.keyframes = Memory::malloc_EWRAM<Keyframe>(nrOfKeyframes);
            sequence.nrOfKeyframes = 0;
            sequence.maxKeyframes = nrOfKeyframes;
            sequence.direction = DirectionMode::Forward;
            sequence.repeatMode = repeatMode;
            sequence.repeatCount = repeatCount;
            sequence.active = true;
            nrOfSequences++;
        }
        return sequences[nrOfSequences - 1];
    }

    void addKeyframe(Sequence &sequence, const Keyframe &kf)
    {
        if (sequence.nrOfKeyframes < sequence.maxKeyframes)
        {
            auto &keyframe = sequence.keyframes[sequence.nrOfKeyframes];
            keyframe = kf;
            keyframe.offset = sequence.nrOfKeyframes == 0 ? 0 : kf.offset;
            sequence.nrOfKeyframes++;
        }
    }

    void addKeyframe(Sequence &sequence, const void *data, Math::fp1616_t offset, EaseMode easeMode)
    {
        if (sequence.nrOfKeyframes < sequence.maxKeyframes)
        {
            auto &keyframe = sequence.keyframes[sequence.nrOfKeyframes];
            keyframe.data = data;
            keyframe.offset = sequence.nrOfKeyframes == 0 ? 0 : offset;
            keyframe.easeMode = easeMode;
            keyframe.isEmpty = false;
            sequence.nrOfKeyframes++;
        }
    }

    void addEmptyKeyframe(Sequence &sequence, Math::fp1616_t offset)
    {
        if (sequence.nrOfKeyframes < sequence.maxKeyframes)
        {
            auto &keyframe = sequence.keyframes[sequence.nrOfKeyframes];
            keyframe.data = nullptr;
            keyframe.offset = sequence.nrOfKeyframes == 0 ? 0 : offset;
            keyframe.easeMode = EaseMode::Linear;
            keyframe.isEmpty = true;
            sequence.nrOfKeyframes++;
        }
    }

    void clear()
    {
        if (nrOfSequences > 0)
        {
#ifdef DEBUG_ANIMATION
            printf("Sequences: %d", nrOfSequences);
#endif
            for (int i = 0; i < nrOfSequences; ++i)
            {
#ifdef DEBUG_ANIMATION
                printf("Sequence %d - Keyframes %x", uint32_t(i), sequences[i].keyframes);
#endif
                // free memory
                Memory::free(sequences[i].keyframes);
            }
            Memory::memset32(sequences, 0, nrOfSequences * sizeof(Sequence) / 4);
            nrOfSequences = 0;
        }
    }

    void update()
    {
        const auto currentTime = Time::now();
        // loop through all sequences currently active
        for (uint32_t si = 0; si < nrOfSequences; ++si)
        {
            auto &sequence = sequences[si];
            if (sequence.active)
            {
                const auto duration = sequence.keyframes[sequence.nrOfKeyframes - 1].offset;
                auto durationPassed = currentTime - sequence.start;
#ifdef DEBUG_ANIMATION
                printf("Sequence %d, dur = %d, passed = %d", si, duration, durationPassed);
#endif
                // check if before first keyframe
                if (durationPassed < 0)
                {
#ifdef DEBUG_ANIMATION
                    printf("Sequence %d not started", si);
#endif
                    continue;
                }
                // check if after last keyframe
                else if (durationPassed >= duration)
                {
                    // check if we want to repeat and have repeats left
                    if (sequence.repeatMode == RepeatMode::None || sequence.repeatCount <= 0)
                    {
#ifdef DEBUG_ANIMATION
                        printf("Sequence %d finished", si);
#endif
                        // call function with final value
                        if (sequence.direction == DirectionMode::Forward)
                        {
                            sequence.updateFunc(Math::fp1616_t::ONE, sequence.keyframes[sequence.nrOfKeyframes - 2].data, sequence.keyframes[sequence.nrOfKeyframes - 1].data);
                        }
                        else
                        {
                            sequence.updateFunc(Math::fp1616_t::ZERO, sequence.keyframes[0].data, sequence.keyframes[1].data);
                        }
                        sequence.active = false;
                        continue;
                    }
                    else
                    {
#ifdef DEBUG_ANIMATION
                        printf("Sequence %d finished, repeating", si);
#endif
                        // set the start time to the current time so we'll repeat or reverse the sequence
                        auto rest = durationPassed - duration;
                        sequence.start = currentTime - rest;
                        // recalculate the duration passed
                        durationPassed = currentTime - sequence.start;
                        if (sequence.repeatMode == RepeatMode::PingPong || sequence.repeatMode == RepeatMode::PingPongForever)
                        {
                            sequence.direction = sequence.direction == DirectionMode::Forward ? DirectionMode::Backward : DirectionMode::Forward;
                        }
                        if (sequence.repeatMode != RepeatMode::RepeatForever && sequence.repeatMode != RepeatMode::PingPongForever)
                        {
                            sequence.repeatCount--;
                        }
                    }
                }
                // if the direction is backwards, adjust current time accordingly
                const auto offset = sequence.direction == DirectionMode::Forward ? durationPassed : duration - durationPassed;
#ifdef DEBUG_ANIMATION
                printf("Sequence %d offset = %d", si, offset);
#endif
                // loop through all keyframes in sequence to find the two keyframes involved. we start at second - there must be two keyframes
                for (uint32_t ki = 1; ki < sequence.nrOfKeyframes; ++ki)
                {
                    const auto &keyframe1 = sequence.keyframes[ki];
                    // is the keyframe active?
                    if (offset < keyframe1.offset)
                    {
                        // only do something if keyframe not empty
                        const auto &keyframe0 = sequence.keyframes[ki - 1];
                        if (!keyframe0.isEmpty)
                        {
                            // calculate delta and clamp to [0,1]
                            auto t = (offset - keyframe0.offset) / (keyframe1.offset - keyframe0.offset);
                            t = clamp(t, Math::fp1616_t::ZERO, Math::fp1616_t::ONE);
#ifdef DEBUG_ANIMATION
                            printf("Keyframe %d, t = %d", ki, t);
#endif
                            // apply ease function
                            switch (keyframe0.easeMode)
                            {
                            case EaseMode::EaseIn:
                                t = ease_in(t);
                                break;
                            case EaseMode::EaseOut:
                                t = ease_out(t);
                                break;
                            case EaseMode::SmoothStep:
                                t = smoothstep(t);
                                break;
                            case EaseMode::InPauseOut:
                                t = in_pause_out(t);
                                break;
                            default:
                                // linear interpolation already handled
                                break;
                            }
                            // call function with t and data
                            sequence.updateFunc(t, keyframe0.data, keyframe1.data);
                        }
                        // skip to next sequence
                        break;
                    }
                }
            }
        }
    }

} // namespace Animation
