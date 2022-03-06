#pragma once

#include <cstdint>

using color8 = uint8_t;
using color16 = uint16_t;

#define RGB5(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
#define RGB8(r, g, b) ((((b) >> 3) << 10) | (((g) >> 3) << 5) | ((r) >> 3))

#define COLOR8_BLACK ((color8)0)
#define COLOR8_WHITE ((color8)255)

#define COLOR16_BLACK ((color16)0)
#define COLOR16_RED ((color16)0x001F)
#define COLOR16_GREEN ((color16)0x03E0)
#define COLOR16_BLUE ((color16)0x7C00)
#define COLOR16_CYAN ((color16)0x7FE0)
#define COLOR16_MAGENTA ((color16)0x7C1F)
#define COLOR16_YELLOW ((color16)0x03FF)
#define COLOR16_DARK_GRAY ((color16)0x2108)
#define COLOR16_GRAY ((color16)0x4631)
#define COLOR16_LIGHT_GRAY ((color16)0x5EF7)
#define COLOR16_WHITE ((color16)0xFFFF)

// Convert colors: http://www.budmelvin.com/dev/15bitconverter.html
// or use hex2gba from https://github.com/HorstBaerbel/GBA-image-tools
