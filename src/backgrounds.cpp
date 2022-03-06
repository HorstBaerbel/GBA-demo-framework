#include "backgrounds.h"

#include "sys/video.h"

namespace Backgrounds
{

    uint16_t control(Tiles::TileBase tileBase, Tiles::ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority)
    {
        return uint16_t(tileBase) | uint16_t(screenBase) | uint16_t(screenSize) | (uint16_t(depth) << 7) | uint16_t(priority);
    }

    void setControl(Background background, Tiles::TileBase tileBase, Tiles::ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority)
    {
        *(BGCTRL + uint32_t(background)) = control(tileBase, screenBase, screenSize, depth, priority);
    }

} // namespace Backgrounds
