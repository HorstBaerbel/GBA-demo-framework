#pragma once

#include "color.h"
#include "gradient.h"
#include "sys/video.h"

namespace Palette
{

    using Palette16 = color16[16];        // 1 palette of 4bpp
    using Palette256 = color16[256];      // 1 palette of 8bpp
    using PaletteBlock16 = Palette16[16]; // 16 palettes of 4bpp

    /// @brief Start of sprite palette memory
    auto const Sprite{reinterpret_cast<uint16_t *>(0x05000200)};
    /// @brief Start of sprite palette memory as 16 color palettes
    auto const Sprite16{reinterpret_cast<Palette::Palette16 *>(0x05000200)};
    /// @brief Start of sprite palette memory as a 256 color palette
    auto const Sprite256{reinterpret_cast<Palette::Palette256 *>(0x05000200)};

    /// @brief Start of background palette memory
    auto const Background{reinterpret_cast<uint16_t *>(0x05000000)};
    /// @brief Start of background palette memory as 16 color palettes
    auto const Background16{reinterpret_cast<Palette::Palette16 *>(0x05000000)};
    /// @brief Start of background palette memory as a 256 color palette
    auto const Background256{reinterpret_cast<Palette::Palette256 *>(0x05000000)};

    /// @brief Fill palette with a color gradient.
    /// @param palette Palette to fill with gradient.
    /// @param gradient Single gradient definition.
    /// @param startIndex start index in color palette.
    /// @param indexCount indexCount Number of indices to fill.
    void createGradientPalette(uint16_t *palette, const Gradient16 &gradient, int32_t startIndex = 0, int32_t indexCount = 256);

    /// @brief Fill palette with multiple color gradients.
    /// @param palette Palette to fill with gradients.
    /// @param parts Multiple gradient parts.
    /// @param partCount Multiple gradient parts.
    /// @param startIndex start index in color palette.
    /// @param indexCount indexCount Number of indices to fill.
    void createGradientPalette(uint16_t *palette, const Gradient16 *parts, uint8_t partCount, int32_t startIndex = 0, int32_t indexCount = 256);

    /// @brief Rotate the palette entries upwards.
    /// @param count Number of step to rotate.
    /// @param start Index to start at.
    /// @param nrOfEntries Number of entries to rotate.
    void rotatePalette(uint16_t *palette, uint32_t count, uint32_t start = 0, uint32_t nrOfEntries = 256);

    /// @brief Fade the palette entries towards black.
    /// @param t Fade value 0 = Black, 1 = input palette.
    /// @param start Index to start at.
    /// @param nrOfEntries Number of entries to fade.
    void fadeToBlack(uint16_t *palette, Math::fp1616_t t, uint32_t start = 0, uint32_t nrOfEntries = 256);

    /// @brief Cross-fade between two palettes.
    /// @param dst Output palette.
    /// @param paletteA Input palette A.
    /// @param paletteB Input palette B.
    /// @param t Fade value 0 = palette A, 1 = palette B.
    /// @param start Index to start at.
    /// @param nrOfEntries Number of entries to fade.
    void crossFade(uint16_t *dst, const uint16_t *paletteA, const uint16_t *paletteB, Math::fp1616_t t, uint32_t start = 0, uint32_t nrOfEntries = 256);

} //namespace Palette
