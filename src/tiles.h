#pragma once

#include "palette.h"

// Functions for tile-base backgrounds in modes 0/1/2.
namespace Tiles
{

    struct Tile16
    {
        uint32_t data[8];
    }; //!<s-tile 8x8@4bpp: 32bytes; 8 uints

    struct Tile256
    {
        uint32_t data[16];
    }; //!<d-tile: 8x8@8bpp: 64bytes; 16 uints

    /// @brief Start of tile memory (== start of VRAM)
    constexpr uint32_t TileMem = 0x06000000;

    /// @brief Tile memory interpreted as 16 color tiles
    auto const TileMem16{reinterpret_cast<Tile16 *>(TileMem)};

    /// @brief Tile memory interpreted as 256 color tiles
    auto const TileMem256{reinterpret_cast<Tile256 *>(TileMem)};

    enum class SizeCode : uint8_t
    {
        Size8x8 = 0,
        Size16x16 = 1,
        Size32x32 = 2,
        Size64x64 = 3, //square, OBJ_SHAPE(0)
        Size16x8 = 4,
        Size32x8 = 5,
        Size32x16 = 6,
        Size64x32 = 7, //horizontal, OBJ_SHAPE(1)
        Size8x16 = 8,
        Size8x32 = 9,
        Size16x32 = 10,
        Size32x64 = 11 //vertical, OBJ_SHAPE(2)
    };

    enum class Background : uint8_t
    {
        BG0 = 0,
        BG1 = 1,
        BG2 = 2,
        BG3 = 3
    };

    enum class Priority : uint8_t
    {
        PRIO_0 = 0,
        PRIO_1 = 1,
        PRIO_2 = 2,
        PRIO_3 = 3
    };

    /// @brief This is where the tiles aka the bitmap data for the tiles starts.
    enum class TileBase : uint16_t
    {
        BASE_0000 = (0 << 2),
        BASE_4000 = (1 << 2),
        BASE_8000 = (2 << 2),
        BASE_C000 = (3 << 2)
    };

    constexpr uint16_t *TILE_BASE_TO_MEM(TileBase b)
    {
        return reinterpret_cast<uint16_t *>(TileMem + ((uint32_t(b)) << 12));
    }

    /// @brief This is where the screen aka the map data for the tiles starts.
    enum class ScreenBase : uint16_t
    {
        BASE_0000 = (0 << 8),
        BASE_0800 = (1 << 8),
        BASE_1000 = (2 << 8),
        BASE_1800 = (3 << 8),
        BASE_2000 = (4 << 8),
        BASE_2800 = (5 << 8),
        BASE_3000 = (6 << 8),
        BASE_3800 = (7 << 8),
        BASE_4000 = (8 << 8),
        BASE_4800 = (9 << 8),
        BASE_5000 = (10 << 8),
        BASE_5800 = (11 << 8),
        BASE_6000 = (12 << 8),
        BASE_6800 = (13 << 8),
        BASE_7000 = (14 << 8),
        BASE_7800 = (15 << 8),
        BASE_8000 = (16 << 8),
        BASE_8800 = (17 << 8),
        BASE_9000 = (18 << 8),
        BASE_9800 = (19 << 8),
        BASE_A000 = (20 << 8),
        BASE_A800 = (21 << 8),
        BASE_B000 = (22 << 8),
        BASE_B800 = (23 << 8),
        BASE_C000 = (24 << 8),
        BASE_C800 = (25 << 8),
        BASE_D000 = (26 << 8),
        BASE_D800 = (27 << 8),
        BASE_E000 = (28 << 8),
        BASE_E800 = (29 << 8),
        BASE_F000 = (30 << 8),
        BASE_F800 = (31 << 8)
    };

    constexpr uint16_t *SCREEN_BASE_TO_MEM(ScreenBase b)
    {
        return reinterpret_cast<uint16_t *>(TileMem + ((uint32_t(b)) << 3));
    }

    enum class ScreenSize : uint16_t
    {
        SIZE_0 = (0 << 14),
        SIZE_1 = (1 << 14),
        SIZE_2 = (2 << 14),
        SIZE_3 = (3 << 14)
    };

    enum class ColorDepth : uint8_t
    {
        Depth16 = 0,
        Depth256 = 1
    };

    extern const uint8_t TileCountForSizeCode[12];
    extern const uint8_t HorizontalTilesForSizeCode[12];
    extern const uint8_t VerticalTilesForSizeCode[12];

    /// @brief Set up background.
    /// See: http://problemkaputt.de/gbatek.htm#lcdiobgcontrol
    void setBackground(Background background, TileBase tileBase, ScreenBase screenBase, ScreenSize screenSize, ColorDepth depth, Priority priority = Priority::PRIO_0);

    /// @brief Copy tile data for multiple tiles from bitmap to VRAM.
    /// Use your if your tiles come from one bitmap.
    void copyTileData(Tile16 *tileMem, SizeCode blockSize, uint32_t nrOfBlocks, const Tile16 *bitmap, uint32_t bitmapWidth, uint32_t xStep = 0, uint32_t yStep = 0);

    /// @brief Write random values to tile data.
    void randomTileData(Tile16 *tileMem, uint32_t nrOfTiles, bool noZeroColor = false);

    /// @brief Write random values to map data for text (NON rotate/scale backgrounds).
    /// @param mapMem Map meory to fill.
    /// @param nrOfTiles Number of map entries to fill. Must be divisible by 2.
    /// @param paletteIndex Palette index for 16-color modes.
    /// @param pow2ModValue Power-of-2 modulu value for random values. Must be in [1,2,4,8,16,32,64,128,256,512,1024]!
    void randomMapData(uint16_t *mapMem, uint32_t nrOfTiles, uint8_t paletteIndex = 0, uint32_t pow2ModValue = 1024);

}
