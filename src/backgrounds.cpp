#include "backgrounds.h"

#include <gba_video.h>

namespace Backgrounds
{

    void setBackground(Background background, Tiles::TileBase tileBase, Tiles::ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority)
    {
        uint16_t settings = uint16_t(tileBase) | uint16_t(screenBase) | uint16_t(screenSize) | (uint16_t(depth) << 7) | uint16_t(priority);
        *(BGCTRL + uint32_t(background)) = settings;
    }

} // namespace Backgrounds
