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
}
