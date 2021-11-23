#include "sprites.h"
#include "memory/memory.h"
#include <gba_affine.h>
#include <gba_sprites.h>

namespace Sprites
{

    const uint8_t TileCountForSizeCode[12] = {1, 4, 16, 64, 2, 4, 8, 32, 2, 4, 8, 32};
    const uint8_t HorizontalTilesForSizeCode[12] = {1, 2, 4, 8, 2, 4, 4, 8, 1, 1, 2, 4};
    const uint8_t VerticalTilesForSizeCode[12] = {1, 2, 4, 8, 1, 1, 2, 4, 2, 4, 4, 8};

    void clear()
    {
        if (REG_DISPCNT & MODE_3 || REG_DISPCNT & MODE_4)
        {
            Memory::memset32(&OAM[512], 0, sizeof(OBJATTR) * 512 / 4);
        }
        else
        {
            Memory::memset32(OAM, 0, sizeof(OBJATTR) * 1024 / 4);
        }
    }

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

    void create(Sprite2D &sprite, uint16_t index, uint16_t x, uint16_t y, uint16_t tileIndex, SizeCode size, uint8_t paletteIndex, ColorDepth depth, Mode mode, bool mosaic, bool visible)
    {
        sprite.index = index;
        sprite.x = x;
        sprite.y = y;
        sprite.tileIndex = tileIndex;
        sprite.size = size;
        sprite.paletteIndex = paletteIndex;
        sprite.depth = depth;
        sprite.mosaic = mosaic;
        sprite.visible = visible;
        sprite.type = Type::Regular;
        sprite.mode = mode;
        sprite.matrixIndex = 0;
        sprite.matrix = Math::fp1616mat2x2_t::identity;
    }

    void setSprite(const Sprite2D &sprite)
    {
        OBJATTR *attribute = &OAM[sprite.index];
        attribute->attr0 = OBJ_Y(sprite.y) | getSpriteScale(sprite.size);
        attribute->attr0 |= (sprite.visible ? 0 : OBJ_DISABLE) | (sprite.mosaic ? OBJ_MOSAIC : 0) | (sprite.depth == ColorDepth::Depth256 ? ATTR0_COLOR_256 : ATTR0_COLOR_16);
        attribute->attr0 |= (sprite.mode == Mode::Transparent ? OBJ_TRANSLUCENT : 0) | (sprite.mode == Mode::Window ? OBJ_OBJWINDOW : 0);
        attribute->attr1 = OBJ_X(sprite.x) | getSpriteSize(sprite.size);
        if (sprite.type == Type::Affine)
        {
            attribute->attr0 |= (sprite.visible ? OBJ_ROT_SCALE_ON | OBJ_DOUBLE : 0);
            attribute->attr1 |= ATTR1_ROTDATA(sprite.matrixIndex & 0x1F);
            setMatrix(sprite);
        }
        attribute->attr2 = (sprite.tileIndex & 1023) | ATTR2_PALETTE(sprite.depth == ColorDepth::Depth256 ? 0 : (sprite.paletteIndex & 15));
    }

    void setSprites(const Sprite2D *sprites, uint32_t start, uint32_t count)
    {
        for (uint32_t i = start; i < start + count; ++i)
        {
            setSprite(sprites[i]);
        }
    }

    void setVisible(const Sprite2D &sprite)
    {
        OBJATTR *attribute = &OAM[sprite.index];
        attribute->attr0 = (attribute->attr0 & 0b1111110011111111) | (sprite.visible ? 0 : OBJ_DISABLE) | ((sprite.type == Type::Affine && sprite.visible) ? OBJ_ROT_SCALE_ON | OBJ_DOUBLE : 0);
    }

    void setPosition(const Sprite2D &sprite)
    {
        OBJATTR *attribute = &OAM[sprite.index];
        attribute->attr0 = (attribute->attr0 & 0b1111111100000000) | OBJ_Y(sprite.y);
        attribute->attr1 = (attribute->attr1 & 0b1111111000000000) | OBJ_X(sprite.x);
    }

    void setMatrix(const Sprite2D &sprite)
    {
        OBJAFFINE *objAffine = reinterpret_cast<OBJAFFINE *>(&OAM[sprite.matrixIndex & 0x1F]);
        objAffine->pa = (int16_t)(sprite.matrix[0][0].raw() >> 8);
        objAffine->pb = (int16_t)(sprite.matrix[0][1].raw() >> 8);
        objAffine->pc = (int16_t)(sprite.matrix[1][0].raw() >> 8);
        objAffine->pd = (int16_t)(sprite.matrix[1][1].raw() >> 8);
    }

    void copyTileData(const Sprite2D &sprite, const Tiles::Tile16 *tileData)
    {
        copyTileData(sprite, tileData);
    }

    void copyTileData(const Sprite2D *sprites, const Tiles::Tile16 *tileData, uint32_t nrOfSprites)
    {
        uint32_t offsetInSrcData = 0;
        auto srcData = reinterpret_cast<const uint8_t *>(tileData);
        for (uint32_t i = 0; i < nrOfSprites; ++i)
        {
            const auto &sprite = sprites[i];
            const uint32_t bytesPerSprite = (uint32_t(TileCountForSizeCode[(uint32_t)sprite.size]) * 64) >> 1;
            Memory::memcpy32(&SpriteMem16[sprite.tileIndex], srcData + offsetInSrcData, bytesPerSprite >> 2);
            offsetInSrcData += bytesPerSprite;
        }
    }

    void copyTileData(const Sprite2D &sprite, const Tiles::Tile256 *tileData)
    {
        uint32_t tileCount = TileCountForSizeCode[(uint32_t)sprite.size];
        Memory::memcpy32(&SpriteMem256[sprite.tileIndex], tileData, sizeof(Tiles::Tile256) / 4 * tileCount);
    }

    void copyTileData(const Sprite2D *sprites, uint32_t nrOfSprites, const Tiles::Tile256 *bitmapData, uint32_t bitmapWidth, uint32_t xStep, uint32_t yStep)
    {
        // we copy 4 8bit pixels (1/2 tile line) at once
        const uint32_t tileDataStride = bitmapWidth >> 2;
        auto tileData = reinterpret_cast<const uint32_t *>(bitmapData);
        for (uint32_t i = 0; i < nrOfSprites; ++i)
        {
            const auto &sprite = sprites[i];
            const uint32_t copiesPerLine = HorizontalTilesForSizeCode[(uint32_t)sprite.size] << 1;
            const uint32_t nrOfLines = VerticalTilesForSizeCode[(uint32_t)sprite.size] << 3;
            auto spriteData = reinterpret_cast<uint32_t *>(&SpriteMem256[sprite.tileIndex]);
            const uint32_t *srcData = tileData + (i * (xStep >> 2)) + (i * (yStep >> 2) * tileDataStride);
            for (uint32_t y = 0; y < nrOfLines; ++y)
            {
                // copy one line of tile pixels, 4 pixels at once
                for (uint32_t x = 0; x < copiesPerLine; ++x)
                {
                    *spriteData++ = srcData[x];
                }
                // next line in source data
                srcData += tileDataStride;
            }
            // step to the right to the start of the next tile (+2 == one tile)
            tileData += copiesPerLine;
        }
    }

    void copyPalette(const Sprite2D &sprite, const Palette::Palette16 *paletteData, uint32_t nrOfPalettes, uint32_t nrOfEntries)
    {
        Memory::memcpy16(Palette::SpritePaletteMem16[0][sprite.paletteIndex], paletteData, nrOfEntries * nrOfPalettes);
    }

    void copyPalette(const Sprite2D &sprite, const Palette::Palette256 *paletteData, uint32_t nrOfEntries)
    {
        Memory::memcpy16(Palette::SpritePaletteMem256, paletteData, nrOfEntries);
    }

} //namespace Sprites