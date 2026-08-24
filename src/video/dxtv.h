#pragma once

#include "sys/base.h"

#include <cstdint>

namespace Video
{
    /// @brief Decompress image from DXTV format. Written in ARMv4 assembler
    /// @param data Compressed image data in DXTV format
    /// @param dst Destination buffer. Must be able to hold a full decompressed image
    /// @param prevSrc Previous image to copy motion-compensated blocks from
    /// @param prevSrcLineStride Line stride in bytes for previous source image (e.g. 480 for VRAM mode 3)
    /// @param width Image width
    /// @param height Image height
    /// @note This is implemented in a C++ and ASM version. Include the correct file!
    extern "C" auto DXTVUnCompWrite16bit(const uint32_t *data, uint32_t *dst, const uint32_t *prevSrc, const uint32_t prevSrcLineStride, uint32_t width, uint32_t height) -> void;

    /// @brief Get stored uncompressed size of image data after decoding. Written in ARMv4 assembler
    /// @param data Pointer to DXTV data
    /// @note This is implemented in a C++ and ASM version. Include the correct file!
    extern "C" auto DXTVUnCompGetSize(const uint32_t *data) -> uint32_t;
}
