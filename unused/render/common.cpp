#include "common.h"

#include <cstdint>
#ifdef TARGET_PC
#include <string>
#endif

namespace Render
{

    constexpr uint32_t BytesPerComponent[] = {0, sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t), 4, 2};

    uint32_t GetBytesPerComponent(ComponentType type)
    {
        return BytesPerComponent[static_cast<uint8_t>(type)];
    }

} // namespace Render
