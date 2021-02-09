#pragma once

#include "palette.h"

/// @brief Functions for tile-base backgrounds in modes 0/1/2
namespace Tiles
{

/// @brief s-tile 8x8@4bpp: 32 bytes, 8 words
struct Tile16
{
    uint32_t data[8];
};

/// @brief d-tile: 8x8@8bpp: 64 bytes, 16 words
struct Tile256
{
    uint32_t data[16];
};

/// @brief Start of tile memory (== start VRAM)
constexpr uint32_t TileMem = 0x06000000;

/// @brief Tile memory interpreted as 16 color tiles
auto const TileMem16{reinterpret_cast<Tile16 *>(TileMem)};

/// @brief Tile memory interpreted as 256 color tiles
auto const TileMem256{reinterpret_cast<Tile256 *>(TileMem)};

/// @brief This is where the tiles aka the bitmap / pixel data for the tiles starts
enum class TileBase : uint16_t
{
    Base_0000 = (0 << 2),
    Base_4000 = (1 << 2),
    Base_8000 = (2 << 2),
    Base_C000 = (3 << 2)
};

/// @brief Convert a tile base value to a VRAM address
constexpr uint16_t *TILE_BASE_TO_MEM(TileBase b)
{
    return reinterpret_cast<uint16_t *>(TileMem + ((uint32_t(b)) << 12));
}

/// @brief This is where the screen / tile layout aka the map data for the tiles starts
enum class ScreenBase : uint16_t
{
    Base_0000 = (0 << 8),
    Base_0800 = (1 << 8),
    Base_1000 = (2 << 8),
    Base_1800 = (3 << 8),
    Base_2000 = (4 << 8),
    Base_2800 = (5 << 8),
    Base_3000 = (6 << 8),
    Base_3800 = (7 << 8),
    Base_4000 = (8 << 8),
    Base_4800 = (9 << 8),
    Base_5000 = (10 << 8),
    Base_5800 = (11 << 8),
    Base_6000 = (12 << 8),
    Base_6800 = (13 << 8),
    Base_7000 = (14 << 8),
    Base_7800 = (15 << 8),
    Base_8000 = (16 << 8),
    Base_8800 = (17 << 8),
    Base_9000 = (18 << 8),
    Base_9800 = (19 << 8),
    Base_A000 = (20 << 8),
    Base_A800 = (21 << 8),
    Base_B000 = (22 << 8),
    Base_B800 = (23 << 8),
    Base_C000 = (24 << 8),
    Base_C800 = (25 << 8),
    Base_D000 = (26 << 8),
    Base_D800 = (27 << 8),
    Base_E000 = (28 << 8),
    Base_E800 = (29 << 8),
    Base_F000 = (30 << 8),
    Base_F800 = (31 << 8)
};

/// @brief Convert a screen base value to a VRAM address
constexpr uint16_t *SCREEN_BASE_TO_MEM(ScreenBase b)
{
    return reinterpret_cast<uint16_t *>(TileMem + ((uint32_t(b)) << 3));
}

/// @brief Copy tile data for multiple tiles from single bitmap to VRAM.
/// Use your if your sprites / tiles come from one bitmap.
//void copyTileData(Tile16 *tileMem, SizeCode blockSize, uint32_t nrOfBlocks, const Tile16 *bitmap, uint32_t bitmapWidth, uint32_t xStep = 0, uint32_t yStep = 0);

/// @brief Write random values to tile data.
void randomTileData(Tile16 *tileMem, uint32_t nrOfTiles, bool noZeroColor = false);

/// @brief Write random values to map data for text (NON rotate/scale backgrounds).
/// @param mapMem Map memory to fill.
/// @param nrOfTiles Number of map entries to fill. Must be divisible by 2!
/// @param paletteIndex Palette index for 16-color modes.
/// @param pow2ModValue Power-of-2 modulu value for random values. Must be in [1,2,4,8,16,32,64,128,256,512,1024]!
void randomMapData(uint16_t *mapMem, uint32_t nrOfTiles, uint8_t paletteIndex = 0, uint32_t pow2ModValue = 1024);

} // namespace Tiles
