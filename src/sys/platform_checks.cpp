#include "platform_checks.h"

#include "memctrl.h"
#include "memory/dma.h"

namespace PlatformChecks
{

    // From lifning at gbadev Discord
    // Check the open bus value which should not return 0
    bool isHardwareVBA()
    {
        uint32_t dest[2] = {0xFFFFFFFF, 0xFFFFFFFF};
        DMA::dma_copy32(&dest[0], reinterpret_cast<const uint32_t *>(0x08000000), 1); // read value from ROM
        DMA::dma_copy32(&dest[1], nullptr, 1);                                        // read value from BIOS
        return dest[1] == 0;
    }

    constexpr uint32_t EWRAM_TEST_SIZE = 8;
    EWRAM_DATA uint32_t EWRAM_TEST_BLOCK[EWRAM_TEST_SIZE];
    IWRAM_DATA const uint32_t EWRAMWaitStateSettings[] = {MemCtrl::WaitEwramFast, MemCtrl::WaitEwramLudicrous};

    IWRAM_FUNC uint32_t BestEWRAMWaitstates()
    {
        uint32_t lastWorkingSetting = MemCtrl::WaitEwramNormal;
        for (uint32_t wsi = 0; wsi < 2; wsi++)
        {
            MemCtrl::RegWaitEwram = EWRAMWaitStateSettings[wsi];
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
                    MemCtrl::RegWaitEwram = MemCtrl::WaitEwramNormal;
                    return lastWorkingSetting;
                }
            }
            lastWorkingSetting = EWRAMWaitStateSettings[wsi];
        }
        MemCtrl::RegWaitEwram = MemCtrl::WaitEwramNormal;
        return lastWorkingSetting;
    }

    constexpr uint32_t ROM_MAGIC = 14021968;
    constexpr int32_t ROM_CHECK_SIZE = 16;
    const uint32_t ROMCheckData[ROM_CHECK_SIZE] = {
        ROM_MAGIC + 0,
        ROM_MAGIC + 1,
        ROM_MAGIC + 2};
    EWRAM_DATA const uint32_t ROMWaitStateSettings[] = {MemCtrl::WaitCntNormal, MemCtrl::WaitCntFast};
}
