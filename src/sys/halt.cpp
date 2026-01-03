#include "halt.h"

#include "syscall.h"

namespace Halt
{
    auto Halt() -> void
    {
#ifdef __NDS__
        SYSCALL(0x06);
#else
        SYSCALL(0x02);
#endif
    }

    auto VBlankIntrWait() -> void
    {
        SYSCALL(0x05);
    }
}
