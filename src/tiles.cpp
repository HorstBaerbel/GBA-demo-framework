#include "tiles.h"
#include "math/random.h"

namespace Tiles
{
    const uint8_t TileCountForSizeCode[12] = {1, 4, 16, 64, 2, 4, 8, 32, 2, 4, 8, 32};
    const uint8_t HorizontalTilesForSizeCode[12] = {1, 2, 4, 8, 2, 4, 4, 8, 1, 1, 2, 4};
    const uint8_t VerticalTilesForSizeCode[12] = {1, 2, 4, 8, 1, 1, 2, 4, 2, 4, 4, 8};

    void randomTileData(Tile16 *tileMem, uint32_t nrOfTiles, bool noZeroColor)
    {
        const uint32_t orMask = noZeroColor ? 0x11111111 : 0;
        uint32_t *buffer32 = reinterpret_cast<uint32_t *>(tileMem);
        const uint32_t nrOfBytes = nrOfTiles * (8 * 8 / 2);
        const uint32_t count = nrOfBytes >> 2;
        for (uint32_t i = 0; i < count; i++)
        {
            buffer32[i] = random<uint32_t>() | orMask;
        }
    }

    void randomMapData(uint16_t *mapMem, uint32_t nrOfTiles, uint8_t paletteIndex, uint32_t pow2ModValue)
    {
        const uint32_t paletteValue = ((uint32_t)paletteIndex & 0x0F) << 12;
        const uint32_t count = nrOfTiles >> 1;
        const uint32_t andValue = pow2ModValue - 1;
        for (uint32_t i = 0; i < count; i++)
        {
            uint32_t value = random<uint32_t>();
            *mapMem++ = (value & andValue) | paletteValue;
            *mapMem++ = ((value >> 16) & andValue) | paletteValue;
        }
    }
} // namespace Tiles