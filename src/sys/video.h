#pragma once

#include "base.h"

#include <cstdint>

/// @brief This register controls all aspects of the GBA display.
#define REG_DISPCNT *((volatile uint16_t *)(REG_BASE + 0x00))

/// @brief These bits are used in conjuction with REG_DISPCNT to control the GBA display hardware.
typedef enum LCDC_BITS
{
    MODE_0 = 0,                                       // BG Mode 0
    MODE_1 = 1,                                       // BG Mode 1
    MODE_2 = 2,                                       // BG Mode 2
    MODE_3 = 3,                                       // BG Mode 3
    MODE_4 = 4,                                       // BG Mode 4
    MODE_5 = 5,                                       // BG Mode 5
    BACKBUFFER = BIT(4),                              // buffer display select
    OBJ_1D_MAP = BIT(6),                              // sprite 1 dimensional mapping
    LCDC_OFF = BIT(7),                                // LCDC OFF
    BG0_ON = BIT(8),                                  // enable background 0
    BG1_ON = BIT(9),                                  // enable background 1
    BG2_ON = BIT(10),                                 // enable background 2
    BG3_ON = BIT(11),                                 // enable background 3
    OBJ_ON = BIT(12),                                 // enable sprites
    WIN0_ON = BIT(13),                                // enable window 0
    WIN1_ON = BIT(14),                                // enable window 1
    OBJ_WIN_ON = BIT(15),                             // enable obj window
    BG0_ENABLE = BG0_ON,                              // enable background 0
    BG1_ENABLE = BG1_ON,                              // enable background 1
    BG2_ENABLE = BG2_ON,                              // enable background 2
    BG3_ENABLE = BG3_ON,                              // enable background 3
    OBJ_ENABLE = OBJ_ON,                              // enable sprites
    WIN0_ENABLE = WIN0_ON,                            // enable window 0
    WIN1_ENABLE = WIN1_ON,                            // enable window 1
    OBJ_WIN_ENABLE = OBJ_WIN_ON,                      // enable obj window
    BG_ALL_ON = BG0_ON | BG1_ON | BG2_ON | BG3_ON,    // All Backgrounds on.
    BG_ALL_ENABLE = BG0_ON | BG1_ON | BG2_ON | BG3_ON // All Backgrounds enabled.

} LCDC_BITS;

/// @brief This register controls the LCD interrupts.
#define REG_DISPSTAT *((volatile uint16_t *)(REG_BASE + 0x04))

/// @brief LCDC Interrupt bits
enum LCDC_IRQ
{
    LCDC_VBL_FLAG = (1 << 0),
    LCDC_HBL_FLAG = (1 << 1),
    LCDC_VCNT_FLAG = (1 << 2),
    LCDC_VBL = (1 << 3),
    LCDC_HBL = (1 << 4),
    LCDC_VCNT = (1 << 5)
};

static inline uint32_t VCOUNT(int m) { return m << 8; }

/// @brief Vertical retrace line counter register
#define REG_VCOUNT *((volatile uint16_t *)(REG_BASE + 0x06))

/// @brief  Array definition for background control registers.
///	BGCTRL[0] references background 0 control register
///	BGCTRL[1] references background 1 control register
///	BGCTRL[2] references background 2 control register
///	BGCTRL[3] references background 3 control register
#define BGCTRL ((volatile uint16_t *)(REG_BASE + 0x08))

/// @brief  Background 0 control register.
#define REG_BG0CNT *((volatile uint16_t *)(REG_BASE + 0x08))

/// @brief  Background 1 control register.
#define REG_BG1CNT *((volatile uint16_t *)(REG_BASE + 0x0a))

/// @brief  Background 2 control register.
#define REG_BG2CNT *((volatile uint16_t *)(REG_BASE + 0x0c))

/// @brief  Background 3 control register.
#define REG_BG3CNT *((volatile uint16_t *)(REG_BASE + 0x0e))

typedef struct
{
    volatile uint16_t x;
    volatile uint16_t y;
} BG_SCROLL;

#define BG_OFFSET ((BG_SCROLL *)(REG_BASE + 0x10))

#define REG_BG0HOFS *((uint16_t *)(REG_BASE + 0x10)) // BG 0 H Offset
#define REG_BG0VOFS *((uint16_t *)(REG_BASE + 0x12)) // BG 0 V Offset
#define REG_BG1HOFS *((uint16_t *)(REG_BASE + 0x14)) // BG 1 H Offset
#define REG_BG1VOFS *((uint16_t *)(REG_BASE + 0x16)) // BG 1 V Offset
#define REG_BG2HOFS *((uint16_t *)(REG_BASE + 0x18)) // BG 2 H Offset
#define REG_BG2VOFS *((uint16_t *)(REG_BASE + 0x1a)) // BG 2 V Offset
#define REG_BG3HOFS *((uint16_t *)(REG_BASE + 0x1c)) // BG 3 H Offset
#define REG_BG3VOFS *((uint16_t *)(REG_BASE + 0x1e)) // BG 3 V Offset

#define REG_BG2PA *((int16_t *)(REG_BASE + 0x20))
#define REG_BG2PB *((int16_t *)(REG_BASE + 0x22))
#define REG_BG2PC *((int16_t *)(REG_BASE + 0x24))
#define REG_BG2PD *((int16_t *)(REG_BASE + 0x26))
#define REG_BG2X *((int32_t *)(REG_BASE + 0x28))
#define REG_BG2Y *((int32_t *)(REG_BASE + 0x2c))
#define REG_BG3PA *((int16_t *)(REG_BASE + 0x30))
#define REG_BG3PB *((int16_t *)(REG_BASE + 0x32))
#define REG_BG3PC *((int16_t *)(REG_BASE + 0x34))
#define REG_BG3PD *((int16_t *)(REG_BASE + 0x36))
#define REG_BG3X *((int32_t *)(REG_BASE + 0x38))
#define REG_BG3Y *((int32_t *)(REG_BASE + 0x3c))

#define BG_SIZE(m) ((m << 14))

/// @brief Bit values for background control
enum BG_CTRL_BITS
{
    BG_MOSAIC = BIT(6),     /*!< enable background mosaic			*/
    BG_16_COLOR = (0 << 7), /*!< background uses 16 color tiles		*/
    BG_256_COLOR = BIT(7),  /*!< background uses 256 color tiles	*/
    BG_WRAP = BIT(13),      /*!< background wraps when scrolling	*/
    BG_SIZE_0 = BG_SIZE(0), /*!< Map Size 256x256	*/
    BG_SIZE_1 = BG_SIZE(1), /*!< Map Size 512x256	*/
    BG_SIZE_2 = BG_SIZE(2), /*!< Map Size 256x512	*/
    BG_SIZE_3 = BG_SIZE(3), /*!< Map Size 512x512	*/
};

#define CHAR_BASE(m) ((m) << 2)
#define BG_TILE_BASE(m) ((m) << 2)
#define CHAR_BASE_ADR(m) ((void *)(VRAM + ((m) << 14)))
#define CHAR_BASE_BLOCK(m) ((void *)(VRAM + ((m) << 14)))
#define MAP_BASE_ADR(m) ((void *)(VRAM + ((m) << 11)))
#define SCREEN_BASE_BLOCK(m) ((void *)(VRAM + ((m) << 11)))
#define SCREEN_BASE(m) ((m) << 8)
#define BG_MAP_BASE(m) ((m) << 8)

// Alternate names for char and screen base
#define TILE_BASE(m) ((m) << 2)
#define TILE_BASE_ADR(m) ((void *)(VRAM + ((m) << 14)))

#define MAP_BASE_ADR(m) ((void *)(VRAM + ((m) << 11)))
#define MAP_BASE(m) ((m) << 8)

#define BG_PRIORITY(m) ((m))
#define CHAR_PALETTE(m) ((m) << 12)

/*---------------------------------------------------------------------------------
	CHAR_BASE_ADR() is the direct equivalent to old PATRAM(),
	giving the base address of a chr bank.
	These macros pinpoint the base address of a single tile.
---------------------------------------------------------------------------------*/
#define PATRAM4(x, tn) ((uint32_t *)(VRAM | (((x) << 14) + ((tn) << 5))))
#define PATRAM8(x, tn) ((uint32_t *)(VRAM | (((x) << 14) + ((tn) << 6))))
#define SPR_VRAM(tn) ((uint32_t *)(VRAM | 0x10000 | ((tn) << 5)))

/*---------------------------------------------------------------------------------
	MAP_BASE_ADR() only gives the beginning of a map.
	Each cell of a text map can be accessed using 3D array notation:

	MAP[page][y][x]
---------------------------------------------------------------------------------*/
typedef uint16_t NAMETABLE[32][32];
#define MAP ((NAMETABLE *)0x06000000)

/*---------------------------------------------------------------------------------
	width and height of a GBA text map can (and probably should)
	be controlled separately.
---------------------------------------------------------------------------------*/
#define BG_WID_32 BG_SIZE_0
#define BG_WID_64 BG_SIZE_1
#define BG_HT_32 BG_SIZE_0
#define BG_HT_64 BG_SIZE_2
//---------------------------------------------------------------------------------
// Symbolic names for the rot/scale map sizes
//---------------------------------------------------------------------------------
#define ROTBG_SIZE_16 BG_SIZE_0
#define ROTBG_SIZE_32 BG_SIZE_1
#define ROTBG_SIZE_64 BG_SIZE_2
#define ROTBG_SIZE_128 BG_SIZE_3

#define TEXTBG_SIZE_256x256 BG_SIZE_0
#define TEXTBG_SIZE_512x256 BG_SIZE_1
#define TEXTBG_SIZE_256x512 BG_SIZE_2
#define TEXTBG_SIZE_512x512 BG_SIZE_3

#define ROTBG_SIZE_128x128 BG_SIZE_0
#define ROTBG_SIZE_256x256 BG_SIZE_1
#define ROTBG_SIZE_512x512 BG_SIZE_2
#define ROTBG_SIZE_1024x1024 BG_SIZE_3

//---------------------------------------------------------------------------------
// Framebuffers for mode 3 and 5
//---------------------------------------------------------------------------------
typedef uint16_t MODE3_LINE[240];
typedef uint16_t MODE5_LINE[160];

#define MODE3_FB ((MODE3_LINE *)0x06000000)
#define MODE5_FB ((MODE5_LINE *)0x06000000)
#define MODE5_BB ((MODE5_LINE *)0x0600A000)

#define REG_WIN0H *((volatile uint16_t *)(REG_BASE + 0x40))
#define REG_WIN1H *((volatile uint16_t *)(REG_BASE + 0x42))
#define REG_WIN0V *((volatile uint16_t *)(REG_BASE + 0x44))
#define REG_WIN1V *((volatile uint16_t *)(REG_BASE + 0x46))
#define REG_WININ *((volatile uint16_t *)(REG_BASE + 0x48))
#define REG_WINOUT *((volatile uint16_t *)(REG_BASE + 0x4A))

#define REG_MOSAIC *(volatile uint16_t *)(REG_BASE + 0x4c)

#define REG_BLDCNT *((volatile uint16_t *)(REG_BASE + 0x50))
#define REG_BLDALPHA *((volatile uint16_t *)(REG_BASE + 0x52))
#define REG_BLDY *((volatile uint16_t *)(REG_BASE + 0x54))

static inline void SetMode(int mode)
{
    REG_DISPCNT = mode;
}

// --- Sprites -----------------------------------------------------------------------

#define OAM ((OBJATTR *)0x07000000)
#define OBJ_BASE_ADR ((void *)(VRAM + 0x10000))
#define SPRITE_GFX ((u16 *)(VRAM + 0x10000))
#define BITMAP_OBJ_BASE_ADR ((void *)(VRAM + 0x14000))

#define OBJ_SHAPE(m) ((m) << 14)

// Sprite Attribute 0
#define OBJ_Y(m) ((m)&0x00ff)
#define OBJ_ROT_SCALE_ON (1 << 8)
#define OBJ_DISABLE (1 << 9)
#define OBJ_DOUBLE (1 << 9)
#define OBJ_MODE(m) ((m) << 10)
#define OBJ_MOSAIC (1 << 12)
#define OBJ_256_COLOR (1 << 13)
#define OBJ_16_COLOR (0 << 13)

#define ATTR0_MOSAIC (1 << 12)
#define ATTR0_COLOR_256 (1 << 13)
#define ATTR0_COLOR_16 (0 << 13)
#define ATTR0_TYPE_NORMAL (0 << 10)
#define ATTR0_TYPE_BLENDED (1 << 10)
#define ATTR0_TYPE_WINDOWED (2 << 10)
#define ATTR0_NORMAL (0 << 8)
#define ATTR0_ROTSCALE (1 << 8)
#define ATTR0_DISABLED (2 << 8)
#define ATTR0_ROTSCALE_DOUBLE (3 << 8)

#define ATTR0_SQUARE OBJ_SHAPE(SQUARE)
#define ATTR0_WIDE OBJ_SHAPE(WIDE)
#define ATTR0_TALL OBJ_SHAPE(TALL)

// Sprite Attribute 1
#define OBJ_X(m) ((m)&0x01ff)
#define OBJ_ROT_SCALE(m) ((m) << 9)
#define OBJ_HFLIP (1 << 12)
#define OBJ_VFLIP (1 << 13)

#define ATTR1_ROTDATA(n) ((n) << 9) // note: overlaps with flip flags
#define ATTR1_FLIP_X (1 << 12)
#define ATTR1_FLIP_Y (1 << 13)
#define ATTR1_SIZE_8 (0 << 14)
#define ATTR1_SIZE_16 (1 << 14)
#define ATTR1_SIZE_32 (2 << 14)
#define ATTR1_SIZE_64 (3 << 14)

#define OBJ_SIZE(m) ((m) << 14)

// Sprite Attribute 2
#define OBJ_CHAR(m) ((m)&0x03ff)
#define OBJ_PRIORITY(m) ((m) << 10)
#define OBJ_PALETTE(m) ((m) << 12)

#define ATTR2_PRIORITY(n) ((n) << 10)
#define ATTR2_PALETTE(n) ((n) << 12)

#define OBJ_TRANSLUCENT OBJ_MODE(1)
#define OBJ_OBJWINDOW OBJ_MODE(2)
#define OBJ_SQUARE OBJ_SHAPE(0)
#define OBJ_WIDE OBJ_SHAPE(1)
#define OBJ_TALL OBJ_SHAPE(2)
