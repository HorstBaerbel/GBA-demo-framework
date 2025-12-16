#pragma once

#include <cstdint>

struct DMA_STRUCT
{
    volatile uint32_t source;
    volatile uint32_t destination;
    volatile uint16_t count;
    volatile uint16_t control;
} __attribute__((aligned(4), packed));

#define REG_DMA ((volatile DMA_STRUCT *)0x040000B0)

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

namespace DMA
{
    // PLEASE NOTE: We're on ARM, so word means 32-bits, half-word means 16-bit!

    /// @brief General DMA fill routine
    void dma_fill16(void *destination, uint16_t value, uint16_t nrOfHwords, uint16_t channel = 3, uint16_t mode = 0);
    /// @brief General DMA fill routine
    void dma_fill32(void *destination, uint32_t value, uint16_t nrOfWords, uint16_t channel = 3, uint16_t mode = 0);

    /// @brief General DMA copier
    void dma_copy16(void *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel = 3, uint16_t mode = 0);
    /// @brief General DMA copier
    void dma_copy32(void *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel = 3, uint16_t mode = 0);

    /// @brief Start 16-bit H-blank DMA
    void dma_hdma(uint16_t *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel = 3);
    /// @brief Start 32-bit H-blank DMA
    void dma_hdma(uint32_t *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel = 3);

} // namespace DMA