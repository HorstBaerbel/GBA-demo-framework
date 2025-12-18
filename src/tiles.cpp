#include "tiles.h"
#include "math/random.h"

namespace Tiles
{
    const uint8_t TileCountForSizeCode[12] = {1, 4, 16, 64, 2, 4, 8, 32, 2, 4, 8, 32};
    const uint8_t HorizontalTilesForSizeCode[12] = {1, 2, 4, 8, 2, 4, 4, 8, 1, 1, 2, 4};
    const uint8_t VerticalTilesForSizeCode[12] = {1, 2, 4, 8, 1, 1, 2, 4, 2, 4, 4, 8};
} // namespace Tiles