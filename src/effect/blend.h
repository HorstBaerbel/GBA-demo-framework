#pragma once

#include "time.h"
#include "math/fp32.h"

namespace Effect_Blend
{

    //-----BG/OBJ/BD fade and alpha blend effect------------------------------------------------------
    // See: http://problemkaputt.de/gbatek.htm#lcdiocolorspecialeffects
    // Note that only the top-most OBJ pixels are used for alpha-blending.

    enum class Target : uint16_t
    {
        TARGET1_BG0 = (1 << 0),   /// Target 1: Background 0.
        TARGET1_BG1 = (1 << 1),   /// Target 1: Background 1.
        TARGET1_BG2 = (1 << 2),   /// Target 1: Background 2.
        TARGET1_BG3 = (1 << 3),   /// Target 1: Background 3.
        TARGET1_OBJ = (1 << 4),   /// Target 1: OBJ layer.
        TARGET1_BACK = (1 << 5),  /// Target 1: Backdrop.
        TARGET2_BG0 = (1 << 8),   /// Target 2: Background 0.
        TARGET2_BG1 = (1 << 9),   /// Target 2: Background 1.
        TARGET2_BG2 = (1 << 10),  /// Target 2: Background 2.
        TARGET2_BG3 = (1 << 11),  /// Target 2: Background 3.
        TARGET2_OBJ = (1 << 11),  /// Target 2: OBJ layer.
        TARGET2_BACK = (1 << 13), /// Target 2: Backdrop.
    };

    /// @brief Combines two Target flags
    inline Target operator|(Target a, Target b) { return static_cast<Target>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b)); }

    enum class Mode : uint16_t
    {
        MODE_OFF = (0 << 6),   /// Fade effect or alpha blending off.
        MODE_ALPHA = (1 << 6), /// Alpha blending.
        MODE_WHITE = (2 << 6), /// Fade to white.
        MODE_BLACK = (3 << 6)  /// Fade to black.
    };

    enum class Direction : uint16_t
    {
        FADE_TO,           /// Fade from current state to "to" state.
        FADE_TO_AND_BACK,  /// Fade from current state to "to" state and back.
        FADE_FROM,         /// Fade from "from" state to "to" state.
        FADE_FROM_AND_BACK /// Fade from "from" state to "to" state and back.
    };

    struct EffectData
    {
        Target targets;      /// Fade or alpha blend targets. Combine with | operator.
        Mode mode;           /// Fading or alpha blending mode.
        Direction direction; /// Fade or alpha blend direction used.
        uint16_t dummy;
        uint8_t fromValue1 = 0;   /// Fade or alpha start value [0,16] for target 1.
        uint8_t toValue1 = 16;    /// Fade or alpha end value [0,16] for target 1.
        uint8_t fromValue2 = 0;   /// Alpha start value [0,16] for target 2. Not used for fades.
        uint8_t toValue2 = 16;    /// Alpha end value [0,16] for target 2. Not used for fades.
        Math::fp1616_t fadeTime;  /// The time the operation should take.
        Math::fp1616_t startTime; /// Time the effect was started.
    } __attribute__((aligned(4), packed));

    /// @brief Create background fade effect.
    void createFade(EffectData &data, Target targets, Mode mode, Direction direction, Math::fp1616_t duration, uint8_t from = 0, uint8_t to = 31);

    /// @brief Set background fade effect.
    /// @param data Effect data.
    void startFade(EffectData &data);

    /// @brief Enable/disable fade effect manually.
    /// @param targets Targets to apply effect to.
    /// @param mode Effect to apply.
    /// @param t Fade factor [0,1]. 1 for full effect.
    void setFade(Target targets, Mode mode, Math::fp1616_t t = 0);

    /// @brief Enable/disable alpha blend effect manually.
    /// @param targets1 Targets in tier 1 to apply effect to.
    /// @param targets2 Targets in tier 2 to apply effect to.
    /// @param t1 Fade factor [0,1] for targets1. 1 for full alpha.
    /// @param t2 Fade factor [0,1] for targets2. 1 for full alpha.
    void setBlend(Target targets1, Target targets2, Math::fp1616_t t1, Math::fp1616_t t2);

    /// @brief Clear all registers to their default values, turning off all effects.
    void clear();

} // namespace Effect_Blend