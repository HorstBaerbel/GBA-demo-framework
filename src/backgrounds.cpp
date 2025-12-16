#include "backgrounds.h"

#include "sys/video.h"

namespace Backgrounds
{

    void setControl(Background background, Tiles::TileBase tileBase, Tiles::ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority)
    {
        *(BGCTRL + uint32_t(background)) = control(tileBase, screenBase, screenSize, depth, priority);
    }

} // namespace Backgrounds
