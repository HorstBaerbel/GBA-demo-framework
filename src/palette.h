#pragma once

#include "color.h"
#include "gradient.h"

#include <gba_video.h>

namespace Palette
{

using Palette16 = color16[16];
using Palette256 = color16[256];
using PaletteBlock16 = Palette16[16]; //!<16 palettes of 4bpp

// Adresses for tile / bitmap background color palettes
auto const BgPaletteMem16{reinterpret_cast<PaletteBlock16 *>(BG_PALETTE)};
auto const BgPaletteMem256{reinterpret_cast<Palette256 *>(BG_PALETTE)};

// Adresses for sprite / OBJ color palettes
auto const SpritePaletteMem16{reinterpret_cast<PaletteBlock16 *>(SPRITE_PALETTE)};
auto const SpritePaletteMem256{reinterpret_cast<Palette256 *>(SPRITE_PALETTE)};

/// @brief Fill 16 color palette with a color gradient.
/// @param palette Palette to fill with gradient.
/// @param gradient Single gradient definition.
/// @param startIndex start index in color palette.
/// @param indexCount indexCount Number of indices to fill.
void createGradientPalette(Palette16 &palette, const Gradient16 &gradient, int32_t startIndex = 0, int32_t indexCount = 16);

/// @brief Fill 16 color palette with multiple color gradients.
/// @param palette Palette to fill with gradients.
/// @param parts Multiple gradient parts.
/// @param partCount Multiple gradient parts.
/// @param startIndex start index in color palette.
/// @param indexCount indexCount Number of indices to fill.
void createGradientPalette(Palette16 &palette, const Gradient16 *parts, uint8_t partCount, int32_t startIndex = 0, int32_t indexCount = 16);

/// @brief Fill 256 color palette with a color gradient.
/// @param palette Palette to fill with gradient.
/// @param gradient Single gradient definition.
/// @param startIndex start index in color palette.
/// @param indexCount indexCount Number of indices to fill.
void createGradientPalette(Palette256 &palette, const Gradient16 &gradient, int32_t startIndex = 0, int32_t indexCount = 256);

/// @brief Fill 256 color palette with multiple color gradients.
/// @param palette Palette to fill with gradients.
/// @param parts Multiple gradient parts.
/// @param partCount Multiple gradient parts.
/// @param startIndex start index in color palette.
/// @param indexCount indexCount Number of indices to fill.
void createGradientPalette(Palette256 &palette, const Gradient16 *parts, uint8_t partCount, int32_t startIndex = 0, int32_t indexCount = 256);

/// @brief Rotate the palette entries upwards.
/// @param count Number of step to rotate.
/// @param start Index to start at.
/// @param nrOfEntries Number of entries to rotate.
void rotatePalette(Palette256 &palette, uint32_t count, uint32_t start = 0, uint32_t nrOfEntries = 256);

} //namespace Palette
