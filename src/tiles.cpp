#include "tiles.h"
#include "math/random.h"

namespace Tiles
{

/*void copyTileData(Tile16 *tileMem, SizeCode blockSize, uint32_t nrOfBlocks, const Tiles::Tile16 *bitmap, uint32_t bitmapWidth, uint32_t xStep, uint32_t yStep)
{
    // we copy 8 4bit pixels (one tile line) at once
    const uint32_t bitmapDataStride = bitmapWidth >> 3;
    const uint32_t *bitmapData = reinterpret_cast<const uint32_t *>(bitmap);
    uint32_t tileIndex = 0;
    for (uint32_t i = 0; i < nrOfBlocks; ++i)
    {
        const uint32_t copiesPerLine = HorizontalTilesForSizeCode[(uint32_t)blockSize];
        const uint32_t nrOfTileLines = VerticalTilesForSizeCode[(uint32_t)blockSize];
        uint32_t *dstData = reinterpret_cast<uint32_t *>(&TileMem16[tileIndex]);
        const uint32_t *srcData = bitmapData + (i * (xStep >> 3)) + (i * yStep * bitmapDataStride);
        for (uint32_t tl = 0; tl < nrOfTileLines; ++tl)
        {
            // copy a single line of 8x8 tiles
            uint32_t *blockTile = dstData;
            for (uint32_t y = 0; y < 8; ++y)
            {
                // copy a line of pixels to our tile line
                uint32_t *tileLine = blockTile;
                for (uint32_t x = 0; x < copiesPerLine; ++x)
                {
                    // copy tile line
                    *tileLine = srcData[x];
                    // move to next tile to the right
                    tileLine += 8;
                }
                // move to next tile line
                blockTile++;
                // move to next line in source data
                srcData += bitmapDataStride;
            }
            // move to next tile set to fill
            dstData += 8 * copiesPerLine;
        }
        tileIndex += TileCountForSizeCode[(uint32_t)blockSize];
    }
}*/

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

} //namespace Tiles