#pragma once

#include "math/mat.h"
#include "palette.h"
#include "tiles.h"

// Functions for sprites / OBJs in all modes.
namespace Sprites
{

    /// @brief Start of sprite tile memory
    constexpr uint32_t SpriteMem = 0x06010000;

    /// @brief Sprite tile memory interpreted as 16 color tiles
    auto const TileMem16{reinterpret_cast<Tiles::Tile16 *>(SpriteMem)};

    /// @brief Sprite tile memory interpreted as 256 color tiles
    auto const TileMem256{reinterpret_cast<Tiles::Tile256 *>(SpriteMem)};

    /// @brief 16-color sprite tile index to memory address
    constexpr uint16_t *TILE_INDEX_TO_MEM16(uint16_t index)
    {
        // sprite tile INDEX is the same in 4 and 8 bit mode. yeah, wtf.
        return reinterpret_cast<uint16_t *>(SpriteMem + ((uint32_t(index)) * 8 * 4));
    }

    /// @brief 256-color sprite tile index to memory address
    constexpr uint16_t *TILE_INDEX_TO_MEM256(uint16_t index)
    {
        // sprite tile INDEX is the same in 4 and 8 bit mode. yeah, wtf.
        return TILE_INDEX_TO_MEM16(index);
    }

    /// @brief Sprite type
    enum class Type : uint8_t
    {
        Regular = 0, // Regular sprite
        Affine = 1   // Affine sprite which can be scaled and rotated
    };

    /// @brief Sprite display mode
    enum class Mode : uint8_t
    {
        Normal = 0,      // Normal display
        Transparent = 1, // Sprite used for alpha blending
        Window = 2       // Sprite used for window mask
    };

    /// @brief Sprite size code
    enum class SizeCode : uint8_t
    {
        Size8x8 = 0,
        Size16x16 = 1,
        Size32x32 = 2,
        Size64x64 = 3,
        Size16x8 = 4,
        Size32x8 = 5,
        Size32x16 = 6,
        Size64x32 = 7,
        Size8x16 = 8,
        Size8x32 = 9,
        Size16x32 = 10,
        Size32x64 = 11
    };

    /// @brief Sprite color depth
    enum class ColorDepth : uint8_t
    {
        Depth16 = 0, // 16 colors, 4bpp, 16 palettes
        Depth256 = 1 // 256 colors, 8bpp, 1 palette
    };

    /// @brief Sprite priorities relative to background (0 = highest, 3 = lowest)
    /// See: http://problemkaputt.de/gbatek.htm#lcdobjoamattributes
    enum class Priority : uint8_t
    {
        Prio0 = 0,
        Prio1 = 1,
        Prio2 = 2,
        Prio3 = 3
    };

    /// @brief 2D sprite backbuffer
    struct Sprite2D
    {
        uint16_t index = 0;
        int16_t x = 240;
        int16_t y = 160;
        uint16_t tileIndex = 0;
        SizeCode size = SizeCode::Size8x8;
        uint8_t paletteIndex = 0;
        ColorDepth depth = ColorDepth::Depth16;
        bool mosaic = false;
        bool visible = false;
        Type type = Type::Regular;
        Mode mode = Mode::Normal;
        bool mirrorH = false;
        bool mirrorV = false;
        Priority priority = Priority::Prio0;
        bool doubleSize = false;
        uint8_t matrixIndex = 0;
        Math::fp1616mat2x2_t matrix = Math::fp1616mat2x2_t::identity;
    } __attribute__((aligned(4), packed));

    /// @brief Fill a Sprite2D struct with data. Will create a regular sprite.
    void create(Sprite2D &sprite, uint16_t index, int16_t x, int16_t y, uint16_t tileIndex = 0, SizeCode size = SizeCode::Size8x8, uint8_t paletteIndex = 0, ColorDepth depth = ColorDepth::Depth16, Mode mode = Mode::Normal, bool mosaic = false, bool visible = true);

    /// @brief Copy tile data for sprite to VRAM.
    void copyTileData(const Sprite2D &sprite, const Tiles::Tile16 *tileData);
    /// @brief Copy tile data for multiple sprites from bitmap to VRAM.
    void copyTileData(const Sprite2D *sprites, const Tiles::Tile16 *tileData, uint32_t nrOfSprites = 1);
    /// @brief Copy tile data for sprite to VRAM.
    void copyTileData(const Sprite2D &sprite, const Tiles::Tile256 *tileData);
    /// @brief Copy tile data for multiple sprites from bitmap to VRAM.
    /// Use your if your sprites ,e.g. 64x64 + 32x64 come from one bitmap, e.g. 96x64.
    void copyTileData(const Sprite2D *sprites, uint32_t nrOfSprites, const Tiles::Tile256 *bitmapData, uint32_t bitmapWidth, uint32_t xStep = 0, uint32_t yStep = 0);

    /// @brief Copy tile palette for sprite to palette RAM.
    void copyPalette(const Sprite2D &sprite, const Palette::Palette16 *paletteData, uint32_t nrOfPalettes = 1, uint32_t nrOfEntries = 16);
    /// @brief Copy tile palette for sprite to palette RAM.
    void copyPalette(const Sprite2D &sprite, const Palette::Palette256 *paletteData, uint32_t nrOfEntries = 256);

    /// @brief Set only the visible portion of the sprite to OAM. Call only in vblank.
    void setVisible(const Sprite2D &sprite);
    /// @brief Set only the position portion of the sprite to OAM. Call only in vblank.
    void setPosition(const Sprite2D &sprite);
    /// @brief Set only the matrix portion of the sprite to OAM. Call only in vblank.
    void setMatrix(const Sprite2D &sprite);
    /// @brief Copy sprite data to OAM. Call only in vblank.
    void setSprite(const Sprite2D &sprite);
    /// @brief Copy sprite data to OAM. Call only in vblank.
    void setSprites(const Sprite2D *sprites, uint32_t start = 0, uint32_t count = 1);

    /// @brief Clear all sprite data in OAM and disable all sprites. Call only in vblank.
    void clear();

} // namespace Sprites
