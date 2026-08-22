#pragma once

#include "sys/base.h"

#include <cstdint>

namespace Compression
{
    /// @brief Decompress LZ4 variant 40h, writing 8 bit at a time. Safe for VRAM. Written in ARMv4 assembler
    /// @param data Pointer to LZ4-compressed data
    /// @param dst Pointer to output buffer
    /// @note This is implemented in a C++ and ASM version. Include the correct file!
    extern "C" auto LZ4UnCompWrite8bit(const void *data, void *dst) -> void;

    /// @brief Get stored uncompressed size of data after decoding. Written in ARMv4 assembler
    /// @param data Pointer to LZ4-compressed data
    /// @note This is implemented in a C++ and ASM version. Include the correct file!
    extern "C" auto LZ4UnCompGetSize(const void *data) -> uint32_t;
}
