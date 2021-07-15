#include "memctrl.h"

namespace MemCtrl
{

    constexpr uint32_t EWRAM_TEST_SIZE = 8;
    uint32_t EWRAM_TEST_BLOCK[EWRAM_TEST_SIZE] EWRAM_DATA;
    static const uint32_t WaitStateSettings[] = {WaitEwramFast, WaitEwramLudicrous};

    uint32_t BestEWRAMWaitstates()
    {
        uint32_t lastWorkingSetting = WaitEwramNormal;
        for (int_fast32_t wsi = 0; wsi < 2; wsi++)
        {
            RegWaitEwram = WaitStateSettings[wsi];
            for (uint_fast32_t i = 0; i < EWRAM_TEST_SIZE; i++)
            {
                EWRAM_TEST_BLOCK[i] = reinterpret_cast<uint32_t>(&EWRAM_TEST_BLOCK[i]);
            }
            for (uint_fast32_t i = 0; i < EWRAM_TEST_SIZE; i++)
            {
                EWRAM_TEST_BLOCK[i] = ~reinterpret_cast<uint32_t>(&EWRAM_TEST_BLOCK[i]);
            }
            for (uint_fast32_t i = 0; i < EWRAM_TEST_SIZE; i++)
            {
                if (EWRAM_TEST_BLOCK[i] != ~reinterpret_cast<uint32_t>(&EWRAM_TEST_BLOCK[i]))
                {
                    RegWaitEwram = WaitEwramNormal;
                    return lastWorkingSetting;
                }
            }
            lastWorkingSetting = WaitStateSettings[wsi];
        }
        RegWaitEwram = WaitEwramNormal;
        return lastWorkingSetting;
    }

}
