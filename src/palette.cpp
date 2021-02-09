#include "palette.h"
#include "memory/memory.h"

namespace Palette
{

void createGradientPalette(Palette16 &palette, const Gradient16 &gradient, int32_t startIndex, int32_t indexCount)
{
    fillWithGradient(&palette[startIndex], gradient, indexCount);
}

void createGradientPalette(Palette16 &palette, const Gradient16 *parts, uint8_t partCount, int32_t startIndex, int32_t indexCount)
{
    fillWithGradient(&palette[startIndex], parts, partCount, indexCount);
}

void createGradientPalette(Palette256 &palette, const Gradient16 &gradient, int32_t startIndex, int32_t indexCount)
{
    fillWithGradient(&palette[startIndex], gradient, indexCount);
}

void createGradientPalette(Palette256 &palette, const Gradient16 *parts, uint8_t partCount, int32_t startIndex, int32_t indexCount)
{
    fillWithGradient(&palette[startIndex], parts, partCount, indexCount);
}

void rotatePalette(Palette256 &palette, uint32_t count, uint32_t start, uint32_t nrOfEntries)
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

} //namespace Palette
