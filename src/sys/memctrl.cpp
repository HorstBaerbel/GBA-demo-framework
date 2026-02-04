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

    constexpr uint32_t IntMemCntDataMagic = 3510229297;
    constexpr uint32_t IntMemCntDataSize = 16;
    EWRAM_DATA uint32_t IntMemCntData[IntMemCntDataSize] = {
        IntMemCntDataMagic + 0,
        IntMemCntDataMagic + 1,
        IntMemCntDataMagic + 2,
        IntMemCntDataMagic + 3,
        IntMemCntDataMagic + 4,
        IntMemCntDataMagic + 5,
        IntMemCntDataMagic + 6,
        IntMemCntDataMagic + 7,
        IntMemCntDataMagic + 8,
        IntMemCntDataMagic + 9,
        IntMemCntDataMagic + 10,
        IntMemCntDataMagic + 11,
        IntMemCntDataMagic + 12,
        IntMemCntDataMagic + 13,
        IntMemCntDataMagic + 14,
        IntMemCntDataMagic + 15,
    };

    IWRAM_FUNC auto setIntMemCnt(uint16_t value) -> bool
    {
        RegIntMemCnt = value;
        // check sequential reads
        for (uint32_t i = 0; i < IntMemCntDataSize; i++)
        {
            if (*(volatile uint32_t *)&IntMemCntData[i] != (IntMemCntDataMagic + i))
            {
                RegIntMemCnt = WaitEwramNormal;
                return false;
            }
        }
        // check non-sequential reads
        for (uint32_t i = 0, j = IntMemCntDataSize - 1; i < IntMemCntDataSize; i++, j--)
        {
            const bool left = *(volatile uint32_t *)&IntMemCntData[i] == (IntMemCntDataMagic + i);
            const bool right = *(volatile uint32_t *)&IntMemCntData[j] == (IntMemCntDataMagic + j);
            if (!left || !right)
            {
                RegIntMemCnt = WaitEwramNormal;
                return false;
            }
        }
        return true;
    }
}
