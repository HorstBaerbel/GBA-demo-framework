#pragma once

#include <stdint.h>
#ifdef TARGET_PC
#include <string>
#endif

namespace Render
{

    /// @brief Attribute component types supported for renderer.
    enum class ComponentType : uint8_t
    {
        BAD_TYPE = 0,
        U8 = 1,     // single uint8_t (paletted color)
        U16 = 2,    // single uint16_t (indices, RGB555 color)
        U32 = 3,    // single uint32_t (indices, RGB888 color)
        FP1616 = 4, // single fp1616_t (vertices)
        FP412 = 5   // single fp412_t (normals, texcoords)
    };

    /// @brief Map type enum to type size information.
    uint32_t GetBytesPerComponent(ComponentType type);

    /// @brief Action to apply when creating or setting data.
    enum class CopyPolicy : uint8_t
    {
        USE_POINTER = 0,           // Simply use the pointer passed, do not allocate any memory or copy any data
        ALLOCATE_DATA = 1,         // Allocate memory for the data (in EWRAM on GBA), but do not copy.
        ALLOCATE_AND_COPY_DATA = 2 // Allocate memory for the data (in EWRAM on GBA) and copy data from pointer to internal memory
    };

    /// @brief Power-of-two sizes in pixels, e.g. for textures.
    enum class Pow2Size : uint16_t
    {
        BAD_SIZE = 0,
        S16 = 16,
        S32 = 32,
        S64 = 64,
        S128 = 128,
        S256 = 256,
        S512 = 512,
        S1024 = 1024
    };

} // namespace Render
