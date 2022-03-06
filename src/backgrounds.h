#pragma once

#include "tiles.h"

#include <cstdint>

namespace Backgrounds
{

    /// @brief Backgrounds available on GBA
    /// See: http://problemkaputt.de/gbatek.htm#gbalcdvideocontroller
    enum class Background : uint16_t
    {
        BG0 = 0,
        BG1 = 1,
        BG2 = 2,
        BG3 = 3
    };

    /// @brief Background priorities (0 = highest, 3 = lowest)
    /// See: http://problemkaputt.de/gbatek.htm#lcdiobgcontrol
    enum class Priority : uint16_t
    {
        Prio0 = 0,
        Prio1 = 1,
        Prio2 = 2,
        Prio3 = 3
    };

    /// @brief Background screen sizes for modes 0/1/2
    enum class ScreenSize : uint16_t
    {
        Size0 = (0 << 14), // Text mode 256x256, Rotation mode 128x128
        Size1 = (1 << 14), // Text mode 512x256, Rotation mode 256x256
        Size2 = (2 << 14), // Text mode 256x512, Rotation mode 512x512
        Size3 = (3 << 14)  // Text mode 512x512, Rotation mode 1024x1024
    };

    /// @brief Background color depth for modes 0/1/2
    enum class ColorDepth : uint16_t
    {
        Depth16 = 0,
        Depth256 = 1
    };

    /// @brief Build background control
    /// See: http://problemkaputt.de/gbatek.htm#lcdiobgcontrol
    uint16_t control(Tiles::TileBase tileBase, Tiles::ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority = Priority::Prio0);

    /// @brief Set up background
    /// See: http://problemkaputt.de/gbatek.htm#lcdiobgcontrol
    void setControl(Background background, Tiles::TileBase tileBase, Tiles::ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority = Priority::Prio0);

} // namespace Backgrounds
