#pragma once

#include "color.h"
#include "math/fp32.h"

/// A linear gradient from a to b.

/// @brief 8-bit indexed gradient
/// To generate an index gradient, call:
/// Gradient8 gradient = {23, 88, 0};
/// color8 gradientData[100];
/// fillWithGradient(*gradientData, gradient);
/// To build more complex gradients, combine these so the length of all parts adds up to 1.0.
/// If combined length < 1.0, indices will not be filled, if > 1.0 gradients will be truncated.
/// Gradient8 gradient[2] = {{23, 88, 0.3f}, {88, 145, 0.7f}};
/// fillWithGradient(*gradientData, gradient, 2);
struct Gradient8
{
    color8 from; /// Start color.
    color8 to; /// End color.
    Math::fp1616_t length; /// Length of part in whole gradient. Ignored if single gradient.
} __attribute__((packed));

/// @brief 15-bit RGB555 gradient
/// To generate a palette from a gradient, call:
/// Gradient16 gradient = {COLOR16_GREEN, COLOR16_YELLOW, 0};
/// Palette::Palette256 greenYellow;
/// fillWithGradient(*greenYellow, gradient);
/// To build more complex gradients, combine these so the length of all parts adds up to 1.0.
/// If combined length < 1.0, indices will not be filled, if > 1.0 gradients will be truncated.
/// Gradient16 gradient[2] = {{COLOR16_GREEN, COLOR16_YELLOW, 0.3f}, {COLOR16_YELLOW, COLOR16_CYAN, 0.7f}};
/// Then you can e.g. fill a palette with it:
/// Palette::Palette256 greenYellowCyan;
/// fillWithGradient(*greenYellowCyan, gradient, 2);
struct Gradient16
{
    color16 from; /// Start color.
    color16 to; /// End color.
    Math::fp1616_t length; /// Length of part in whole gradient. Ignored if single gradient.
} __attribute__((aligned(4), packed));

/// Fill array, e.g. palette with 8bit color gradient (lerp).
void fillWithGradient(color8 *colors, const Gradient8 &gradient, int32_t indexCount = 256);

/// Fill array, e.g. palette with 8bit color gradient (lerp).
void fillWithGradient(color8 *colors, const Gradient8 *parts, uint8_t partCount, int32_t indexCount = 256);

/// Fill array, e.g. palette with 15bit color gradient (lerp).
void fillWithGradient(color16 *colors, const Gradient16 &gradient, int32_t indexCount = 256);

/// Fill array, e.g. palette with 15bit color gradient (lerp).
void fillWithGradient(color16 *colors, const Gradient16 *parts, uint8_t partCount, int32_t indexCount = 256);
