#include "gradient.h"


void fillWithGradient(color8 * colors, const Gradient8 & gradient, int32_t indexCount)
{
    fillWithGradient(colors, &gradient, 1, indexCount);
}

void fillWithGradient(color8 * colors, const Gradient8 * parts, uint8_t partCount, int32_t indexCount)
{
    Math::fp1616_t accumulatedRest = 0;
    int32_t index = 0;
    for (int32_t pi = 0; pi < partCount; ++pi)
    {
        // get new gradient part from list
        const auto & gradient = parts[pi];
        auto partIndexCountf = gradient.length * indexCount;
        int32_t partIndexCount = (int32_t)(partIndexCountf);
        // accumulate rest of truncated count and check if we need an extra index
        accumulatedRest += frac(partIndexCountf);
        if (accumulatedRest >= Math::fp1616_t::HALF)
        {
            partIndexCount += 1;
            accumulatedRest -= 1;
        }
        // clamp part count to maximum index count
        if (index + partIndexCount >= indexCount)
        {
            partIndexCount = indexCount - index;
        }
        // check if gradient part is longer than 0 indices
        if (partIndexCount > 0)
        {
            // yes. get gradient information
            const int32_t gfrom = gradient.from;
            const int32_t gto = gradient.to;
            const int32_t gStep = ((gto - gfrom) << 16) / partIndexCount;
            // fill array with gradient
            int32_t color = gfrom;
            color8 * partColors = &colors[index];
            for (int32_t i = 0; i < partIndexCount; ++i)
            {
                // should we round here?
                partColors[i] = (color + (1 << 15)) >> 16;
                color += gStep;
            }
            index += partIndexCount;
            // stop if maximum index count reached
            if (index >= indexCount)
            {
                break;
            }
        }
    }
}

void fillWithGradient(color16 * colors, const Gradient16 & gradient, int32_t indexCount)
{
    fillWithGradient(colors, &gradient, 1, indexCount);
}

void fillWithGradient(color16 * colors, const Gradient16 * parts, uint8_t partCount, int32_t indexCount)
{
    Math::fp1616_t accumulatedRest = 0;
    int32_t index = 0;
    for (int32_t pi = 0; pi < partCount; ++pi)
    {
        // get new gradient part from list
        const auto & gradient = parts[pi];
        auto partIndexCountf = gradient.length * indexCount;
        int32_t partIndexCount = (int32_t)(partIndexCountf);
        // accumulate rest of truncated count and check if we need an extra index
        accumulatedRest += frac(partIndexCountf);
        if (accumulatedRest >= Math::fp1616_t::HALF)
        {
            partIndexCount += 1;
            accumulatedRest -= 1;
        }
        // clamp part count to maximum index count
        if (index + partIndexCount >= indexCount)
        {
            partIndexCount = indexCount - index;
        }
        // check if gradient part is longer than 0 indices
        if (partIndexCount > 0)
        {
            // yes. get gradient information
            const int32_t fromR = gradient.from & 0x1F;
            const int32_t toR = gradient.to & 0x1F;
            const int32_t gradientStepR = ((toR - fromR) << 16) / partIndexCount;
            const int32_t fromG = (gradient.from >> 5) & 0x1F;
            const int32_t toG = (gradient.to >> 5) & 0x1F;
            const int32_t gradientStepG = ((toG - fromG) << 16) / partIndexCount;
            const int32_t fromB = (gradient.from >> 10) & 0x1F;
            const int32_t toB = (gradient.to >> 10) & 0x1F;
            const int32_t gradientStepB = ((toB - fromB) << 16) / partIndexCount;
            int32_t colorR = fromR << 16;
            int32_t colorG = fromG << 16;
            int32_t colorB = fromB << 16;
            color16 * partColors = &colors[index];
            for (int32_t i = 0; i < partIndexCount; ++i)
            {
                // should we round here?
                partColors[i] = ((colorR + (1 << 15)) >> 16) | (((colorG + (1 << 15)) >> 16) << 5) | (((colorB + (1 << 15)) >> 16) << 10);
                colorR += gradientStepR;
                colorG += gradientStepG;
                colorB += gradientStepB;
            }
            index += partIndexCount;
            // stop if maximum index count reached
            if (index >= indexCount)
            {
                break;
            }
        }
    }
}
