#include "palette.h"

#include "memory/memory.h"

namespace Palette
{

    void createGradientPalette(uint16_t *palette, const Gradient16 &gradient, int32_t startIndex, int32_t indexCount)
    {
        fillWithGradient(&palette[startIndex], gradient, indexCount);
    }

    void createGradientPalette(uint16_t *palette, const Gradient16 *parts, uint8_t partCount, int32_t startIndex, int32_t indexCount)
    {
        fillWithGradient(&palette[startIndex], parts, partCount, indexCount);
    }

    void rotatePalette(uint16_t *palette, uint32_t count, uint32_t start, uint32_t nrOfEntries)
    {
        // restrict count to maximum number of entries
        count = count % (nrOfEntries - 1);
        // back up palette
        Palette256 temp;
        Memory::memcpy16(temp, palette, 256);
        // copy entries
        Memory::memcpy16(&palette[start], &temp[(start + nrOfEntries - count) & 0xFF], count);
        Memory::memcpy16(&palette[start + count], &temp[start], nrOfEntries - count);
    }

    void fadeToBlack(uint16_t *palette, Math::fp1616_t t, uint32_t start, uint32_t nrOfEntries)
    {
        Math::clamp(t, Math::fp1616_t::ZERO, Math::fp1616_t::ONE);
        t = Math::fp1616_t::ONE - t;
        for (uint32_t i = start; i < (start + nrOfEntries); i++)
        {
            // get fade source color
            int16_t rF = palette[i] & 0x1F;
            int16_t gF = (palette[i] >> 5) & 0x1F;
            int16_t bF = (palette[i] >> 10) & 0x1F;
            rF = int16_t(t * rF);
            gF = int16_t(t * gF);
            bF = int16_t(t * bF);
            // write color to palette memory
            palette[i] = (bF << 10) | (gF << 5) | rF;
        }
    }

    void crossFade(uint16_t *dst, const uint16_t *paletteA, const uint16_t *paletteB, Math::fp1616_t t, uint32_t start, uint32_t nrOfEntries)
    {
        Math::clamp(t, Math::fp1616_t::ZERO, Math::fp1616_t::ONE);
        for (uint32_t i = start; i < (start + nrOfEntries); i++)
        {
            // get fade source color
            int16_t rA = paletteA[i] & 0x1F;
            int16_t gA = (paletteA[i] >> 5) & 0x1F;
            int16_t bA = (paletteA[i] >> 10) & 0x1F;
            int16_t rB = paletteB[i] & 0x1F;
            int16_t gB = (paletteB[i] >> 5) & 0x1F;
            int16_t bB = (paletteB[i] >> 10) & 0x1F;
            rA = int16_t(rA + t * (rB - rA));
            gA = int16_t(gA + t * (gB - gA));
            bA = int16_t(bA + t * (bB - bA));
            // write color to palette memory
            dst[i] = (bA << 10) | (gA << 5) | rA;
        }
    }

} //namespace Palette
