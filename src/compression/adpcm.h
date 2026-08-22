#pragma once

#include "sys/base.h"

#include <cstdint>

namespace Audio
{
    /// @brief Decode 4-bit ADPCM sample data and truncate/dither to 8bit. Written in ARMv4 assembler
    /// @param data Pointer to 4-bit ADPCM data
    /// @param dst Pointers to output sample buffer(s). Will be written to as bytes
    extern "C" auto ADPCMUnCompWrite8bit_8bit(const void *data, uint32_t dataSize, uint8_t *dst[2]) -> uint32_t;

    /// @brief Get stored uncompressed size of sample data after decoding and truncating to 8-bit. Written in ARMv4 assembler
    /// @param data Pointer to ADPCM data
    /// @note Only sample depths of (8, 16, 24, 32) are supported!
    extern "C" auto ADPCMUnCompGetSize_8bit(const void *data) -> uint32_t;

    /// @brief Initialize ADPCM decompressor with linear upsampling
    /// @param srcRateHz Input sample rate in Hz. Must be < dstRateHz
    /// @param dstRateHz Output sample rate in Hz. Must be > srcRateHz
    auto ADPCMUpsampleInit(uint32_t srcRateHz, uint32_t dstRateHz) -> void;

    /// @brief Decode 4-bit ADPCM sample data and upsample. Initialize first with UpsampleInit()
    /// @param data Pointer to 4-bit ADPCM data
    /// @param dst Pointers to output sample buffer(s). Will be written to as bytes. Make sure dst is big enough (more samples than ADPCM sample rate)
    /// @return Size of decoded data written to dst in bytes
    auto ADPCMUnCompWrite8bit_8bit_upsample(const void *data, uint32_t dataSize, uint8_t *dst[2]) -> uint32_t;
}
