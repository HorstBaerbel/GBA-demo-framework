#pragma once

#include <cstdint>

namespace DMA
{

    // PLEASE NOTE: We're on ARM, so word means 32-bits, half-word means 16-bit!

    /// @brief General DMA fill routine.
    void dma_fill16(void *destination, uint16_t value, uint16_t nrOfHwords, uint16_t channel = 3, uint16_t mode = 0);
    /// @brief General DMA fill routine.
    void dma_fill32(void *destination, uint32_t value, uint16_t nrOfWords, uint16_t channel = 3, uint16_t mode = 0);

    /// @brief Fast dword fill using DMA 3.
    /// This will fill nrOfBytes in destination with value. The bulk of the
    /// Filling is done using DMA copy. The remaining bytes (nrOfBytes % 4)
    /// will be copied via byte transfers.
    /// @param destination Destination address.
    /// @param value Value to fill destination with.
    /// @param nrOfBytes Bytes to copy.
    void fast_fill(void *destination, uint32_t value, uint32_t nrOfBytes);

    /// @brief Fast VRAM dword fill using DMA 3.
    /// This will fill nrOfBytes in destination with value. The bulk of the
    /// filling is done using DMA copy. Data not starting or ending on even
    /// adresses will be copied by seperate word transfers, because VRAM is word-transfer-only.
    /// @param destination VRAM destination.
    /// @param value Value to fill destination with.
    /// @param nrOfBytes Bytes to copy.
    void vram_fill(uint16_t *destination, uint32_t value, uint32_t nrOfBytes);

    /// @brief General DMA copier.
    void dma_copy16(void *destination, const uint16_t *source, uint16_t nrOfHwords, uint16_t channel = 3, uint16_t mode = 0);
    /// @brief General DMA copier.
    void dma_copy32(void *destination, const uint32_t *source, uint16_t nrOfWords, uint16_t channel = 3, uint16_t mode = 0);

    /// @brief Fast dword copy using DMA 3.
    /// This will copy nrOfBytes from source to destination. The bulk of the
    /// Transfer is done using DMA copy. The remaining bytes (nrOfBytes % 4)
    /// will be copied via byte transfers.
    /// @param destination Destination address.
    /// @param source Source data.
    /// @param nrOfBytes Bytes to copy.
    void fast_copy(void *destination, const uint8_t *source, uint32_t nrOfBytes);

    /// @brief Fast VRAM dword copy using DMA 3.
    /// This will copy nrOfBytes from source to destination. The bulk of the
    /// transfer is done using DMA copy. Data not starting or ending on even
    /// adresses will be copied by seperate word transfers, because VRAM is word-transfer-only.
    /// @param destination VRAM destination.
    /// @param source Source data.
    /// @param nrOfBytes Bytes to copy.
    void vram_copy(uint16_t *destination, const uint8_t *source, uint32_t nrOfBytes);

} // namespace DMA