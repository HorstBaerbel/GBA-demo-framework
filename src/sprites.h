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

    constexpr uint16_t *TILE_INDEX_TO_MEM16(uint16_t index)
    {
        // sprite tile INDEX is the same in 4 and 8 bit mode. yeah, wtf.
        return reinterpret_cast<uint16_t *>(SpriteMem + ((uint32_t(index)) * 8 * 4));
    }

    constexpr uint16_t *TILE_INDEX_TO_MEM256(uint16_t index)
    {
        // sprite tile INDEX is the same in 4 and 8 bit mode. yeah, wtf.
        return TILE_INDEX_TO_MEM16(index);
    }

    enum class Type : uint8_t
    {
        Regular = 0,
        Affine = 1
    };

    enum class Mode : uint8_t
    {
        Normal = 0,
        Transparent = 1,
        Window = 2
    };

    struct Sprite2D
    {
        uint16_t index = 0;
        int16_t x = 240;
        int16_t y = 160;
        uint16_t tileIndex = 0;
        Tiles::SizeCode size = Tiles::SizeCode::Size8x8;
        uint8_t paletteIndex = 0;
        Tiles::ColorDepth depth = Tiles::ColorDepth::Depth16;
        bool mosaic = false;
        bool visible = false;
        Type type = Type::Regular;
        Mode mode = Mode::Normal;
        bool mirrorH = false;
        bool mirrorV = false;
        Tiles::Priority priority = Tiles::Priority::PRIO_0;
        bool doubleSize = false;
        uint8_t matrixIndex = 0;
        Math::fp1616mat2x2_t matrix = Math::fp1616mat2x2_t::identity;
    } __attribute__((aligned(4), packed));

    /// @brief Fill a Sprite2D struct with data. Will create a regular sprite.
    void create(Sprite2D &sprite, uint16_t index, int16_t x, int16_t y, uint16_t tileIndex = 0, Tiles::SizeCode size = Tiles::SizeCode::Size8x8, uint8_t paletteIndex = 0, Tiles::ColorDepth depth = Tiles::ColorDepth::Depth16, Mode mode = Mode::Normal, bool mosaic = false, bool visible = true);

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
