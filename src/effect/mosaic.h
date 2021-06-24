#pragma once

#include <cstdint>

namespace Effect_Mosaic
{

    enum class Target : uint32_t
    {
        TARGET1_BG0 = 0, //Background 0.
        TARGET1_BG1 = 2, //Background 1.
        TARGET1_BG2 = 4, //Background 2.
        TARGET1_BG3 = 6, //Background 3.
    };

    /// @brief Toggle mosaicing on / off for specific target.
    void toggleMosaic(Target target, bool enable);

    /// @brief set mosaic values for Backgrounds. Values are in range [0-15]. 0 == disabled.
    void setMosaicBG(uint16_t bgH, uint16_t bgV);

    /// @brief set mosaic values for Sprites. Values are in range [0-15]. 0 == disabled.
    void setMosaicOBJ(uint16_t objH, uint16_t objV);

    void clear();

}
