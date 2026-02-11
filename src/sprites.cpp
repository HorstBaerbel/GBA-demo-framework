#include "sprites.h"

#include "memory/memory.h"
#include "sys/video.h"
#include "math/fp32.h"

namespace Sprites
{

    typedef struct
    {
        uint16_t attr0;
        uint16_t attr1;
        uint16_t attr2;
        uint16_t dummy;
    } ALIGN(4) OBJATTR;

    typedef struct
    {
        uint16_t attribute[3];
        uint16_t dummy;
    } ALIGN(4) SpriteEntry;

    typedef struct
    {
        uint16_t dummy0[3];
        int16_t pa;
        uint16_t dummy1[3];
        int16_t pb;
        uint16_t dummy2[3];
        int16_t pc;
        uint16_t dummy3[3];
        int16_t pd;
    } ALIGN(4) OBJAFFINE;

    uint16_t getSpriteScale(SizeCode size)
    {
        switch (size)
        {
        case SizeCode::Size16x8:
        case SizeCode::Size32x8:
        case SizeCode::Size32x16:
        case SizeCode::Size64x32:
            return OBJ_SIZE(0b01);
        case SizeCode::Size8x16:
        case SizeCode::Size8x32:
        case SizeCode::Size16x32:
        case SizeCode::Size32x64:
            return OBJ_SIZE(0b10);
        default:
            return 0;
        }
    }

    uint16_t getSpriteSize(SizeCode size)
    {
        switch (size)
        {
        case SizeCode::Size16x16:
        case SizeCode::Size32x8:
        case SizeCode::Size8x32:
            return OBJ_SIZE(0b01);
        case SizeCode::Size32x32:
        case SizeCode::Size32x16:
        case SizeCode::Size16x32:
            return OBJ_SIZE(0b10);
        case SizeCode::Size64x64:
        case SizeCode::Size64x32:
        case SizeCode::Size32x64:
            return OBJ_SIZE(0b11);
        default:
            return 0;
        }
    }

    auto create(uint32_t spriteIndex, int32_t x, int32_t y, uint32_t tileIndex, SizeCode size, ColorDepth depth, uint32_t paletteIndex, Mode mode, bool visible) -> Sprite2D
    {
        Sprite2D sprite;
        sprite.index = spriteIndex;
        sprite.x = x;
        sprite.y = y;
        sprite.tileIndex = tileIndex;
        sprite.size = size;
        sprite.paletteIndex = paletteIndex;
        sprite.depth = depth;
        sprite.mosaic = false;
        sprite.visible = visible;
        sprite.type = Type::Regular;
        sprite.mode = mode;
        sprite.mirrorH = false;
        sprite.mirrorV = false;
        sprite.priority = Priority::Prio0;
        sprite.doubleSize = false;
        sprite.matrixIndex = 0;
        sprite.matrix = {1, 0, 0, 1};
        return sprite;
    }

    auto create(Sprite2D *sprites, uint32_t nrOfSprites, uint32_t spriteIndex, uint32_t tileIndex, SizeCode size, ColorDepth depth, uint32_t paletteIndex) -> void
    {
        const int32_t nrOfTilesPerSprite = (depth == ColorDepth::Depth256 ? 2 : 1) * Tiles::TileCountForSizeCode[static_cast<uint8_t>(size)]; // 256-color sprites use twice the amount of tiles
        Sprite2D sprite;
        sprite.size = size;
        sprite.depth = depth;
        uint32_t si = 0;
        uint32_t ti = tileIndex;
        while (si < nrOfSprites)
        {
            sprite.index = spriteIndex + si;
            sprite.tileIndex = ti;
            sprite.paletteIndex = paletteIndex;
            sprites[si] = sprite;
            si++;
            ti += nrOfTilesPerSprite;
        }
    }

    void copyToOAM(const Sprite2D &sprite)
    {
        auto &obj = reinterpret_cast<OBJATTR *>(OAM)[sprite.index];
        obj.attr0 = OBJ_Y(sprite.y) | getSpriteScale(sprite.size);
        obj.attr0 |= (sprite.visible ? 0 : OBJ_DISABLE) | (sprite.mosaic ? OBJ_MOSAIC : 0) | (sprite.depth == ColorDepth::Depth256 ? ATTR0_COLOR_256 : ATTR0_COLOR_16);
        obj.attr0 |= (sprite.mode == Mode::Transparent ? OBJ_TRANSLUCENT : 0) | (sprite.mode == Mode::Window ? OBJ_OBJWINDOW : 0);
        obj.attr1 = OBJ_X(sprite.x) | getSpriteSize(sprite.size);
        if (sprite.type == Type::Affine)
        {
            obj.attr0 |= (sprite.visible ? OBJ_ROT_SCALE_ON : 0) | (sprite.doubleSize ? OBJ_DOUBLE : 0);
            obj.attr1 |= ATTR1_ROTDATA(static_cast<uint16_t>(sprite.matrixIndex) & 0x1F);
            setMatrixOAM(sprite.matrixIndex, sprite.matrix);
        }
        else
        {
            obj.attr1 |= (sprite.mirrorH ? OBJ_HFLIP : 0) | (sprite.mirrorV ? OBJ_VFLIP : 0);
        }
        obj.attr2 = (sprite.tileIndex & 1023);
        obj.attr2 |= ATTR2_PALETTE(sprite.depth == ColorDepth::Depth256 ? 0 : (sprite.paletteIndex & 15));
        obj.attr2 |= ATTR2_PRIORITY(static_cast<uint16_t>(sprite.priority));
    }

    void copyToOAM(const Sprite2D *sprites, uint32_t start, uint32_t count)
    {
        for (uint32_t i = start; i < start + count; ++i)
        {
            copyToOAM(sprites[i]);
        }
    }

    void clearOAM()
    {
        for (uint32_t i = 0; i < 128; i++)
        {
            auto &obj = reinterpret_cast<OBJATTR *>(OAM)[i];
            obj.attr0 = OBJ_DISABLE;
            obj.attr1 = 0;
            obj.attr2 = 0;
            obj.dummy = 0;
        }
    }

    void setVisibleOAM(uint16_t index, bool visible, bool affine)
    {
        auto &obj = reinterpret_cast<OBJATTR *>(OAM)[index];
        if (affine)
        {
            obj.attr0 = (obj.attr0 & 0b1111111011111111) | (visible ? OBJ_ROT_SCALE_ON : 0);
        }
        else
        {
            obj.attr0 = (obj.attr0 & 0b1111110111111111) | (visible ? 0 : OBJ_DISABLE);
        }
    }

    void setVisibleOAM(const Sprite2D &sprite)
    {
        setVisibleOAM(sprite.index, sprite.visible, sprite.type == Type::Affine);
    }

    void setPositionOAM(uint16_t index, int16_t x, int16_t y)
    {
        x = x > 511 ? 511 : x;
        x = x < -64 ? 240 : x;
        y = y > 255 ? 255 : y;
        y = y < -64 ? 160 : y;
        auto &obj = reinterpret_cast<OBJATTR *>(OAM)[index];
        obj.attr0 = (obj.attr0 & 0b1111111100000000) | OBJ_Y(y);
        obj.attr1 = (obj.attr1 & 0b1111111000000000) | OBJ_X(x);
    }

    void setPositionOAM(const Sprite2D &sprite)
    {
        setPositionOAM(sprite.index, sprite.x, sprite.y);
    }

    void setMatrixOAM(uint8_t matrixIndex, const AffineData &matrix)
    {
        auto &objAffine = reinterpret_cast<OBJAFFINE *>(OAM)[matrixIndex & 0x1F];
        objAffine.pa = matrix.dx;
        objAffine.pb = matrix.dmx;
        objAffine.pc = matrix.dy;
        objAffine.pd = matrix.dmy;
    }

    void setMatrixOAM(const Sprite2D &sprite)
    {
        setMatrixOAM(sprite.matrixIndex, sprite.matrix);
    }

    void copyTileData16(const Sprite2D *sprite, const uint32_t *tileData)
    {
        copyTileData16(sprite, tileData, 1);
    }

    void copyTileData16(const Sprite2D *sprites, uint32_t nrOfSprites, const uint32_t *tileData)
    {
        uint32_t offsetInSrcData = 0;
        const uint8_t *srcData = reinterpret_cast<const uint8_t *>(tileData);
        for (uint32_t i = 0; i < nrOfSprites; ++i)
        {
            const auto &sprite = sprites[i];
            const uint32_t bytesPerSprite = (uint32_t(Tiles::TileCountForSizeCode[(uint32_t)sprite.size]) * 64) >> 1;
            Memory::memcpy32(Sprites::TILE_INDEX_TO_MEM<uint32_t>(sprite.tileIndex), srcData + offsetInSrcData, bytesPerSprite >> 2);
            offsetInSrcData += bytesPerSprite;
        }
    }

    void copyTileData256(const Sprite2D *sprite, const uint32_t *tileData)
    {
        uint32_t tileCount = Tiles::TileCountForSizeCode[(uint32_t)sprite->size];
        Memory::memcpy32(Sprites::TILE_INDEX_TO_MEM256<uint32_t>(sprite->tileIndex), tileData, sizeof(Tiles::Tile256) / 4 * tileCount);
    }

    void copyTileData256(const Sprite2D *sprites, uint32_t nrOfSprites, const uint32_t *bitmapData, uint32_t bitmapWidth, uint32_t xStep, uint32_t yStep)
    {
        const uint32_t srcTileLineStride = bitmapWidth * 8; // 8 lines per tile
        for (uint32_t i = 0; i < nrOfSprites; ++i)
        {
            const auto &sprite = sprites[i];
            const uint32_t bytesPerTileLine = Tiles::HorizontalTilesForSizeCode[(uint32_t)sprite.size] * 8 * 8; // 8 bytes per tile line, 8 lines
            const uint32_t nrOfTileLines = Tiles::VerticalTilesForSizeCode[(uint32_t)sprite.size];
            auto dstData = Sprites::TILE_INDEX_TO_MEM256<uint32_t>(sprite.tileIndex);
            auto srcData = bitmapData + ((i * xStep * 8 + i * yStep * srcTileLineStride) >> 2);
            for (uint32_t y = 0; y < nrOfTileLines; ++y)
            {
                Memory::memcpy32(dstData, srcData, bytesPerTileLine >> 2);
                // next line in source and destination data
                srcData += srcTileLineStride >> 2;
                dstData += bytesPerTileLine >> 2;
            }
        }
    }

    void copyPalette(const Sprite2D *sprite, const Palette::Palette16 *paletteData, uint32_t nrOfPalettes, uint32_t nrOfEntries)
    {
        Memory::memcpy16(Palette::Sprite16[sprite->paletteIndex], paletteData, nrOfEntries * nrOfPalettes);
    }

    void copyPalette(const Palette::Palette256 *paletteData, uint32_t nrOfEntries)
    {
        Memory::memcpy16(Palette::Sprite, paletteData, nrOfEntries);
    }

    AffineData calculateAffineData(Math::fp1616_t angle, Math::fp1616_t sx, Math::fp1616_t sy)
    {
        AffineData result;
        Math::fp1616_t srad(0);
        Math::fp1616_t crad(1);
        if (angle != 0)
        {
            sincos(angle, srad, crad);
        }
        result.dx = (int16_t)((crad / sx).raw() >> 8);
        result.dmx = (int16_t)((-srad / sx).raw() >> 8);
        result.dy = (int16_t)((srad / sy).raw() >> 8);
        result.dmy = (int16_t)((crad / sy).raw() >> 8);
        return result;
    }

    bool isInside(const Sprite2D &sprite, uint32_t right, uint32_t bottom)
    {
        const auto width = 8 * Tiles::HorizontalTilesForSizeCode[static_cast<uint8_t>(sprite.size)];
        const auto height = 8 * Tiles::VerticalTilesForSizeCode[static_cast<uint8_t>(sprite.size)];
        return (sprite.x <= static_cast<int16_t>(right)) && (sprite.y <= static_cast<int16_t>(bottom)) && (sprite.x + width > 0) && (sprite.y + height > 0);
    }

} // namespace Sprites