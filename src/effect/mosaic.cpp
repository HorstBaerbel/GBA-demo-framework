#include "mosaic.h"

#include "sys/base.h"
#include "sys/video.h"

namespace Effect_Mosaic
{

    void toggleMosaic(Target target, bool enable)
    {
        volatile uint16_t *reg = reinterpret_cast<volatile uint16_t *>(REG_BASE + 8 + uint32_t(target));
        *reg = enable ? *reg | 0x40 : *reg & ~0x40;
    }

    void setMosaicBG(uint16_t bgH, uint16_t bgV)
    {
        REG_MOSAIC = (REG_MOSAIC & 0xFF00) | ((bgV & 0xF) << 4) | (bgV & 0xF);
    }

    void setMosaicOBJ(uint16_t objH, uint16_t objV)
    {
        REG_MOSAIC = ((objV & 0xF) << 12) | ((objH & 0xF) << 8) | (REG_MOSAIC & 0xFF);
    }

    void clear()
    {
        volatile uint16_t *reg = reinterpret_cast<volatile uint16_t *>(REG_BASE + 8);
        for (uint32_t i = 0; i < 4; ++i)
        {
            *reg &= ~0x40;
            reg++;
        }
        REG_MOSAIC = 0;
    }

}
