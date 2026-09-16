#pragma once

#include "math/random.h"

#include <cstdint>

namespace Effect_Random
{

    template <unsigned int BYTES_PER_SCANLINES, unsigned int HEIGHT>
    void fill(uint16_t *buffer, bool halfYRes, uint32_t orMask)
    {
        uint32_t *buffer32 = reinterpret_cast<uint32_t *>(buffer);
        if (halfYRes)
        {
            constexpr uint32_t dwordsPerLine = BYTES_PER_SCANLINES >> 2;
            constexpr uint32_t skipCount = BYTES_PER_SCANLINES >> 1;
            for (uint32_t y = 0; y < HEIGHT; y += 2)
            {
                for (uint32_t i = 0; i < dwordsPerLine; i++)
                {
                    uint32_t value = random<uint32_t>() | orMask;
                    buffer32[i] = value;
                    buffer32[i + dwordsPerLine] = value;
                }
                buffer32 += skipCount;
            }
        }
        else
        {
            constexpr uint32_t dwords = (BYTES_PER_SCANLINES * HEIGHT) >> 2;
            for (uint32_t i = 0; i < dwords; i++)
            {
                buffer32[i] = random<uint32_t>() | orMask;
            }
        }
    }

    /// @brief Fill 16-color buffer with random values.
    /// @param noZeroColor If true does not fill the buffer with the nibble value 0.
    /// @param halfYRes If true will double pixels vertically (might be a bit faster).
    template <unsigned int BYTES_PER_SCANLINES, unsigned int HEIGHT>
    void fill4(uint16_t *buffer, bool noZeroColor, bool halfYRes)
    {
        const uint32_t orMask = noZeroColor ? 0x11111111 : 0;
        fill<BYTES_PER_SCANLINES, HEIGHT>(buffer, halfYRes, orMask);
    }

    /// @brief Fill 256-color buffer with random values.
    /// @param noZeroColor If true does not fill the buffer with the byte value 0.
    /// @param halfYRes If true will double pixels vertically (might be a bit faster).
    template <unsigned int BYTES_PER_SCANLINES, unsigned int HEIGHT>
    void fill8(uint16_t *buffer, bool noZeroColor, bool halfYRes)
    {
        const uint32_t orMask = noZeroColor ? 0x01010101 : 0;
        fill<BYTES_PER_SCANLINES, HEIGHT>(buffer, halfYRes, orMask);
    }

}
