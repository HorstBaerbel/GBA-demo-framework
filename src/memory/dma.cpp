#include "dma.h"

namespace DMA
{

    struct DMA_REC
    {
        const void *source;
        void *destination;
        uint16_t count;
        uint16_t mode;
    } __attribute__((aligned(4), packed));

#define REG_DMA ((volatile DMA_REC *)0x040000B0)
#define DMA_DST_INC (0 << 5)
#define DMA_DST_DEC (1 << 5)
#define DMA_DST_FIXED (2 << 5)
#define DMA_DST_RELOAD (3 << 5)
#define DMA_SRC_INC (0 << 7)
#define DMA_SRC_DEC (1 << 7)
#define DMA_SRC_FIXED (2 << 7)
#define DMA_REPEAT (1 << 9)
#define DMA16 (0 << 10)
#define DMA32 (1 << 10)
#define GAMEPAK_DRQ (1 << 11)
#define DMA_IMMEDIATE (0 << 12)
#define DMA_VBLANK (1 << 12)
#define DMA_HBLANK (2 << 12)
#define DMA_SPECIAL (3 << 12)
#define DMA_IRQ (1 << 14)
#define DMA_ENABLE (1 << 15)

    uint32_t DMAFillTempValue;

    /*//! General DMA transfer macro
#define DMA_TRANSFER(_destination, _source, count, channel, mode) \
    do                                                            \
    {                                                             \
        REG_DMA[channel].mode = 0;                               \
        REG_DMA[channel].source = (const void *)(_source);        \
        REG_DMA[channel].destination = (void *)(_destination);    \
        REG_DMA[channel].count = (count);                         \
        REG_DMA[channel].mode = (mode);                           \
    } while (0)
    */

    void dma_fill16(void *destination, uint16_t value, uint16_t nrOfHwords, uint16_t channel, uint16_t mode)
    {
        //while (REG_DMA[channel].count & DMA_ENABLE) {} // wait for previous transfer
        DMAFillTempValue = value;
        REG_DMA[channel].source = reinterpret_cast<const void *>(&DMAFillTempValue);
        REG_DMA[channel].destination = reinterpret_cast<void *>(destination);
        REG_DMA[channel].count = nrOfHwords;
        REG_DMA[channel].mode = mode | DMA16 | DMA_SRC_FIXED | DMA_ENABLE;
    }

    void dma_fill32(void *destination, uint32_t value, uint16_t nrOfWords, uint16_t channel, uint16_t mode)
    {
        //while (REG_DMA[channel].count & DMA_ENABLE) {} // wait for previous transfer
        DMAFillTempValue = value;
        REG_DMA[channel].source = reinterpret_cast<const void *>(&DMAFillTempValue);
        REG_DMA[channel].destination = reinterpret_cast<void *>(destination);
        REG_DMA[channel].count = nrOfWords;
        REG_DMA[channel].mode = mode | DMA32 | DMA_SRC_FIXED | DMA_ENABLE;
    }

    void fast_fill(void *destination, uint32_t value, uint32_t nrOfBytes)
    {
        const uint32_t dwords = nrOfBytes >> 2;
        const uint32_t restBytes = nrOfBytes - (dwords << 2);
        dma_fill32(destination, value, dwords, 3);
        if (restBytes > 0)
        {
            uint8_t *dest8 = static_cast<uint8_t *>(destination) + nrOfBytes - restBytes;
            const uint8_t *src8 = reinterpret_cast<const uint8_t *>(&DMAFillTempValue) + sizeof(uint32_t) - restBytes;
            for (uint32_t i = 0; i < restBytes; ++i)
            {
                dest8[i] = src8[i];
            }
        }
    }

    void vram_fill(uint16_t *destination, uint32_t value, uint32_t nrOfBytes)
    {
        // we need to pre-copy if destination is odd
        if (reinterpret_cast<uint32_t>(destination) & 1)
        {
            // we need to pre-copy 1 word. replace second byte in destination
            uint16_t *dTemp = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(destination) - 1);
            *dTemp = (*dTemp & 0x00FF) | static_cast<uint16_t>(value >> 24);
            // decrease copy by one byte and advance in destination and source
            destination = dTemp++;
            nrOfBytes--;
            value = (value << 8) | (value >> 24); // aka rotate left 8 bit
        }
        const uint32_t dwords = nrOfBytes >> 2;
        dma_fill32(destination, value, dwords, 3);
        // check if there are words or bytes left
        uint32_t restBytes = nrOfBytes - (dwords << 2);
        if (restBytes > 0)
        {
            uint16_t *dEnd = destination + (dwords << 1);
            if (restBytes >= 2)
            {
                *dEnd = static_cast<uint16_t>(value >> 16);
                dEnd++;
                restBytes -= 2;
                value <<= 16;
            }
            if (restBytes > 0)
            {
                // we need to post-copy 1 word. replace first byte in destination
                *dEnd = (*dEnd & 0xFF00) | static_cast<uint16_t>(value >> 16);
            }
        }
    }

    void dma_copy16(void *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel, uint16_t mode)
    {
        REG_DMA[channel].source = reinterpret_cast<const void *>(source);
        REG_DMA[channel].destination = reinterpret_cast<void *>(destination);
        REG_DMA[channel].count = nrOfHwords;
        REG_DMA[channel].mode = mode | DMA16 | DMA_SRC_INC | DMA_ENABLE;
    }

    void dma_copy32(void *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel, uint16_t mode)
    {
        REG_DMA[channel].source = reinterpret_cast<const void *>(source);
        REG_DMA[channel].destination = reinterpret_cast<void *>(destination);
        REG_DMA[channel].count = nrOfWords;
        REG_DMA[channel].mode = mode | DMA32 | DMA_SRC_INC | DMA_ENABLE;
    }

    void fast_copy(void *destination, const uint8_t *source, uint32_t nrOfBytes)
    {
        const uint32_t dwords = nrOfBytes >> 2;
        const uint32_t restBytes = nrOfBytes - (dwords << 2);
        const uint32_t *src32 = reinterpret_cast<const uint32_t *>(source);
        dma_copy32(destination, src32, dwords, 3);
        if (restBytes > 0)
        {
            uint8_t *dest8 = static_cast<uint8_t *>(destination) + nrOfBytes - restBytes;
            const uint8_t *src8 = source + nrOfBytes - restBytes;
            for (uint32_t i = 0; i < restBytes; ++i)
            {
                dest8[i] = src8[i];
            }
        }
    }

    void vram_copy(uint16_t *destination, const uint8_t *source, uint32_t nrOfBytes)
    {
        // we need to pre-copy if destination is odd
        if (reinterpret_cast<uint32_t>(destination) & 1)
        {
            // we need to pre-copy 1 half-word. replace second byte in destination
            uint16_t *dTemp = reinterpret_cast<uint16_t *>(reinterpret_cast<uint8_t *>(destination) - 1);
            *dTemp = (*dTemp & 0x00FF) | static_cast<uint16_t>(source[0]);
            // decrease copy by one byte and advance in destination and source
            destination = dTemp++;
            source++;
            nrOfBytes--;
        }
        // do 32bit DMA copy
        const uint32_t dwords = nrOfBytes >> 2;
        dma_copy32(destination, reinterpret_cast<const uint32_t *>(source), dwords, 3);
        // check if there are words or bytes left
        uint32_t restBytes = nrOfBytes - (dwords << 2);
        if (restBytes > 0)
        {
            uint16_t *dEnd = destination + (dwords << 1);
            const uint16_t *sEnd = reinterpret_cast<const uint16_t *>(source) + (dwords << 1);
            if (restBytes >= 2)
            {
                *dEnd = *sEnd;
                dEnd++;
                sEnd++;
                restBytes -= 2;
            }
            if (restBytes > 0)
            {
                // we need to post-copy 1 word. replace first byte in destination
                *dEnd = (*dEnd & 0xFF00) | *sEnd;
            }
        }
    }

    void dma_hdma(uint16_t *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel)
    {
        REG_DMA[channel].mode = 0;
        REG_DMA[channel].source = reinterpret_cast<const void *>(source);
        REG_DMA[channel].destination = reinterpret_cast<void *>(destination);
        REG_DMA[channel].count = nrOfHwords;
        REG_DMA[channel].mode = DMA_HBLANK | DMA_REPEAT | DMA16 | DMA_DST_FIXED | DMA_SRC_INC | DMA_ENABLE;
    }

    void dma_hdma(uint32_t *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel)
    {
        REG_DMA[channel].mode = 0;
        REG_DMA[channel].source = reinterpret_cast<const void *>(source);
        REG_DMA[channel].destination = reinterpret_cast<void *>(destination);
        REG_DMA[channel].count = nrOfWords;
        REG_DMA[channel].mode = DMA_HBLANK | DMA_REPEAT | DMA32 | DMA_DST_FIXED | DMA_SRC_INC | DMA_ENABLE;
    }

} // namespace DMA