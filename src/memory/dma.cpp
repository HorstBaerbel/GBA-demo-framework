#include "dma.h"

#include "sys/base.h"

namespace DMA
{
    IWRAM_DATA uint32_t DMAFillTempValue;

    void dma_fill16(void *destination, uint16_t value, uint16_t nrOfHwords, uint16_t channel, uint16_t mode)
    {
        // wait for previous transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
        DMAFillTempValue = value;
        REG_DMA[channel].source = (uint32_t)&DMAFillTempValue;
        REG_DMA[channel].destination = (uint32_t)destination;
        REG_DMA[channel].count = nrOfHwords;
        REG_DMA[channel].control = mode | DMA16 | DMA_SRC_FIXED | DMA_ENABLE;
        // wait for transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
    }

    void dma_fill32(void *destination, uint32_t value, uint16_t nrOfWords, uint16_t channel, uint16_t mode)
    {
        // wait for previous transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
        DMAFillTempValue = value;
        REG_DMA[channel].source = (uint32_t)&DMAFillTempValue;
        REG_DMA[channel].destination = (uint32_t)destination;
        REG_DMA[channel].count = nrOfWords;
        REG_DMA[channel].control = mode | DMA32 | DMA_SRC_FIXED | DMA_ENABLE;
        // wait for transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
    }

    void dma_copy16(void *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel, uint16_t mode)
    {
        // wait for previous transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
        REG_DMA[channel].source = (uint32_t)source;
        REG_DMA[channel].destination = (uint32_t)destination;
        REG_DMA[channel].count = nrOfHwords;
        REG_DMA[channel].control = mode | DMA16 | DMA_SRC_INC | DMA_ENABLE;
        // wait for transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
    }

    void dma_copy32(void *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel, uint16_t mode)
    {
        // wait for previous transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
        REG_DMA[channel].source = (uint32_t)source;
        REG_DMA[channel].destination = (uint32_t)destination;
        REG_DMA[channel].count = nrOfWords;
        REG_DMA[channel].control = mode | DMA32 | DMA_SRC_INC | DMA_ENABLE;
        // wait for transfer to finish
        while (REG_DMA[channel].control & DMA_ENABLE)
        {
        }
    }

    void dma_hdma(uint16_t *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel)
    {
        REG_DMA[channel].control = 0;
        REG_DMA[channel].source = (uint32_t)source;
        REG_DMA[channel].destination = (uint32_t)destination;
        REG_DMA[channel].count = nrOfHwords;
        REG_DMA[channel].control = DMA_HBLANK | DMA_REPEAT | DMA16 | DMA_DST_FIXED | DMA_SRC_INC | DMA_ENABLE;
    }

    void dma_hdma(uint32_t *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel)
    {
        REG_DMA[channel].control = 0;
        REG_DMA[channel].source = (uint32_t)source;
        REG_DMA[channel].destination = (uint32_t)destination;
        REG_DMA[channel].count = nrOfWords;
        REG_DMA[channel].control = DMA_HBLANK | DMA_REPEAT | DMA32 | DMA_DST_FIXED | DMA_SRC_INC | DMA_ENABLE;
    }
} // namespace DMA