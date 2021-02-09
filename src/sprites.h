#pragma once

#include "math/mat.h"
#include "palette.h"
#include "tiles.h"

/// @brief Functions for sprites / OBJs in all modes
namespace Sprites
{

/// @brief Start of sprite pixel data in VRAM
constexpr uint32_t SpriteMem = 0x06010000;

/// @brief Sprite memory interpreted as 16 color tiles
auto const SpriteMem16{reinterpret_cast<Tiles::Tile16 *>(SpriteMem)};

/// @brief Sprite memory interpreted as 16 color tiles
auto const SpriteMem256{reinterpret_cast<Tiles::Tile256 *>(SpriteMem)};

/// @brief Sprite transformation type
enum class Type : uint8_t
{
    Regular = 0, /// Sprite can be moved and blended, but not scaled or rotated
    Affine = 1 /// Sprite has 2x2 matrix for scaling / rotation / shearing
};

/// @brief Sprite blending type
enum class Mode : uint8_t
{
    Normal = 0, /// No blending, layering according to index / order in OAM
    Transparent = 1, /// Sprite used as alpha blending 1st target
    Window = 2 /// Sprite non-zero pixels contribute to object window
};

/// @brief Sprite color depth
enum class ColorDepth : uint8_t
{
    Depth16 = 0,
    Depth256 = 1
};

/// @brief Size / geometry code for sprites
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

/// @brief Number of tiles for a specific sprite size code
extern const uint8_t TileCountForSizeCode[12];

/// @brief Number of horizontal tiles for a specific sprite size code
extern const uint8_t HorizontalTilesForSizeCode[12];

/// @brief Number of vertical tiles for a specific sprite size code
extern const uint8_t VerticalTilesForSizeCode[12];

/// @brief Sprite definition
struct Sprite2D
{
    uint16_t index = 0; /// Index in OAM, defines layering
    uint16_t x = 240; /// Horizontal sprite position
    uint16_t y = 160; /// Vertical sprite position
    uint16_t tileIndex = 0; /// Where sprite memory the sprite tiles start
    SizeCode size = SizeCode::Size8x8; /// Sprite size code
    uint8_t paletteIndex = 0; /// Palette index
    ColorDepth depth = ColorDepth::Depth16; /// Color depth
    bool mosaic = false; /// If true mosaicing will be applied to sprite
    bool visible = false; /// If true the sprite is visible
    Type type = Type::Regular; /// Regular or affine sprite
    Mode mode = Mode::Normal; /// Blend mode
    uint8_t matrixIndex = 0; /// Matrix index to use for affine transformation
    Math::fp1616mat2x2_t matrix; /// Sprite affine transformation matrix
} __attribute__((aligned(4), packed));

/// @brief Fill a Sprite2D struct with data. Will create a regular sprite.
void create(Sprite2D &sprite, uint16_t index, uint16_t x, uint16_t y, uint16_t tileIndex = 0, SizeCode size = SizeCode::Size8x8, uint8_t paletteIndex = 0, ColorDepth depth = ColorDepth::Depth16, Mode mode = Mode::Normal, bool mosaic = false, bool visible = true);

/// @brief Copy tile data for sprite to VRAM.
void copyTileData(const Sprite2D &sprite, const Tiles::Tile16 *tileData);

/// @brief Copy tile data for multiple sprites from bitmap to VRAM.
void copyTileData(const Sprite2D *sprites, const Tiles::Tile16 *tileData, uint32_t nrOfSprites = 1);

/// @brief Copy tile data for sprite to VRAM.
void copyTileData(const Sprite2D &sprite, const Tiles::Tile256 *tileData);

/// @brief Copy tile data for multiple sprites from bitmap to VRAM.
/// Use your if your sprites, e.g. 64x64 + 32x64 come from one bitmap, e.g. 96x64.
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

/// @brief Clear all sprite data in OAM. Call only in vblank.
void clearSprites();

} // namespace Sprites
