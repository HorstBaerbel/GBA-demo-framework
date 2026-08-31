#pragma once

#include "resampler.h"
#include "sys/base.h"

#include <cstdint>

namespace Audio
{

    /// @brief Decode 4-bit ADPCM sample data, truncate/dither to 8bit and resample. Initialize first with ResamplerInit()
    /// @param data Pointer to 4-bit ADPCM data
    /// @param dst Pointers to output sample buffer(s). Will be written to as bytes. Make sure dst is big enough (more samples than ADPCM sample rate)
    /// @param resamplerData Pass resampler data initialized with ResamplerInit() to resample, or nullptr to output straight PCM data
    /// @return Size of decoded data written to dst in bytes
    extern "C" auto ADPCMUnCompWrite8bit_8bit(const void *data, uint8_t *dst[2], LinearResamplerData *resamplerData) -> uint32_t;

    /// @brief Get stored uncompressed size of sample data after decoding and truncating to 8-bit. Written in ARMv4 assembler
    /// @param data Pointer to ADPCM data
    /// @note Only sample depths of (8, 16, 24, 32) are supported!
    extern "C" auto ADPCMUnCompGetSize_8bit(const void *data) -> uint32_t;

}
