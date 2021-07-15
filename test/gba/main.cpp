#include "tests.h"

#include <fptime.h>
#include <sys/interrupts.h>
#include <sys/memctrl.h>
#include <video.h>

int main()
{
    // Lets set some cool waitstates
    MemCtrl::RegWaitCnt = MemCtrl::WaitCntFast;
    MemCtrl::RegWaitEwram = MemCtrl::BestEWRAMWaitstates();
    // Clear memory and initialize interrupts
    irqInit();
    // Set up video mode 4, no sprites
    Video::setMode(MODE_4 | BG2_ON);
    // Start internal timer
    Time::start();
    // Run tests on the GBA
    Test::memory();
    Test::copy();
    Test::math_fp32();
    return 0;
}
