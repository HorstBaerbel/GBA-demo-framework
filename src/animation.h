#pragma once

#include "fptime.h"
#include "math/fp32.h"

/// @brief Simple animation functions
namespace Animation
{

/// @brief Interpolation mode to use between keyframes
enum class EaseMode : uint8_t
{
    Linear,
    EaseIn,
    EaseOut,
    SmoothStep,
    InPauseOut
};

/// @brief Sequence repeat mode
enum class RepeatMode : uint8_t
{
    None,
    Repeat,
    RepeatForever,
    PingPong,
    PingPongForever
};

/// @brief Sequence play direction
enum class DirectionMode : uint8_t
{
    Forward,
    Backward
};

/// @brief One key frame for an animation
struct Keyframe
{
    const void *data; /// Pointer to keyframe data to be passed to the update() function. Can be nullptr.
    Math::fp1616_t offset; /// Keyframe time offset from sequence start time.
    EaseMode easeMode; /// Interpolation mode to next keyframe.
    bool isEmpty; /// True if empty. Usefull to pause an animation
} __attribute__((packed));

/// @brief Animation sequence consisting of multiple keyframes
struct Sequence
{
    Math::fp1616_t start; /// Absolute start time of sequence.
    void (*updateFunc)(Math::fp1616_t, const void *, const void *); /// Function to call every update() call. Will pass time [0,1] and the previous and the current data pointers
    Keyframe *keyframes; /// Pointer to keyframes
    uint8_t nrOfKeyframes; /// Current number of keyframes in sequence
    uint8_t maxKeyframes; /// Maximum number of keyframes the sequence can have
    DirectionMode direction; /// Sequence play direction
    RepeatMode repeatMode; /// Sequence repeat mode
    uint8_t repeatCount; /// How often to repeat the sequence
    bool active; /// True if the sequence is active
} __attribute__((packed));

/// @brief Add an animation sequence.
/// @param startTime Global start time for sequence.
/// @param updateFunc Function to be called every update().
/// This will pass the time from the start of this keyframe to the current position inside the keyframe [0,1].
/// Additionally the current and the next "data" pointers are passed.
/// @param nrOfKeyframe Maximum number of all keyframes in sequence.
/// @param repeatMode Repeat mode of whole sequence.
/// @note You'll need at least 2 keyframes for anything to happen.
Sequence &addSequence(Math::fp1616_t start, void (*updateFunc)(Math::fp1616_t, const void *, const void *), uint8_t nrOfKeyframes, RepeatMode repeatMode = RepeatMode::None, uint8_t repeatCount = 1);

/// @brief Add a keyframe to a sequence.
/// @param sequence Sequence to add keyframe to.
/// @param data Keyframe data to pass to sequence updateFunc.
/// @param offset Keyframes relative time offset from sequence start. Make sure this is in increasing order!
/// The offset for the first keyframe will be automatically set to 0, regardless of what is passed here...
/// @param easeMode Ease mode between this and the next keyframe.
/// @note You'll need at least 2 keyframes for anything to happen.
void addKeyframe(Sequence &sequence, const void *data, Math::fp1616_t offset, EaseMode easeMode = EaseMode::Linear);

/// @brief Add a keyframe to a sequence.
/// @param sequence Sequence to add keyframe to.
/// @param keyframe Keyframe to add.
void addKeyframe(Sequence &sequence, const Keyframe &keyframe);

/// @brief Add an empty keyframe to a sequence. Can be e.g. used to pause a sequence.
void addEmptyKeyframe(Sequence &sequence, Math::fp1616_t offset);

/// @brief Clear all sequences and their keyframes.
void clear();

/// @brief This needs to be called regularly (every frame) to update the animation system.
void update();

} //namespace Animation
