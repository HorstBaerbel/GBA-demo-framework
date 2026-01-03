#include "memctrl.h"

namespace MemCtrl
{
    constexpr uint32_t WaitCntRomMagic = 3510229297;
    constexpr uint32_t WaitCntRomSize = 16;
    const uint32_t WaitCntRomData[WaitCntRomSize] = {
        WaitCntRomMagic + 0,
        WaitCntRomMagic + 1,
        WaitCntRomMagic + 2,
        WaitCntRomMagic + 3,
        WaitCntRomMagic + 4,
        WaitCntRomMagic + 5,
        WaitCntRomMagic + 6,
        WaitCntRomMagic + 7,
        WaitCntRomMagic + 8,
        WaitCntRomMagic + 9,
        WaitCntRomMagic + 10,
        WaitCntRomMagic + 11,
        WaitCntRomMagic + 12,
        WaitCntRomMagic + 13,
        WaitCntRomMagic + 14,
        WaitCntRomMagic + 15,
    };

    EWRAM_FUNC auto setWaitCnt(uint16_t value) -> bool
    {
        RegWaitCnt = value;
        // check sequential reads
        for (uint32_t i = 0; i < WaitCntRomSize; i++)
        {
            if (*(volatile uint32_t *)&WaitCntRomData[i] != (WaitCntRomMagic + i))
            {
                RegWaitCnt = WaitCntDefault;
                return false;
            }
        }
        // check non-sequential reads
        for (uint32_t i = 0, j = WaitCntRomSize - 1; i < WaitCntRomSize; i++, j--)
        {
            const bool left = *(volatile uint32_t *)&WaitCntRomData[i] == (WaitCntRomMagic + i);
            const bool right = *(volatile uint32_t *)&WaitCntRomData[j] == (WaitCntRomMagic + j);
            if (!left || !right)
            {
                RegWaitCnt = WaitCntDefault;
                return false;
            }
        }
        return true;
    }
}
