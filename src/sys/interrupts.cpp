#include "interrupts.h"

#include "sys/base.h"
#include "sys/video.h"

/// @brief Interrupt dispatch function in .s file.
extern "C" void DispatchInterrupt();

#define INT_VECTOR *(IntFunc *)(0x03007ffc)

struct IntTable
{
    IntFunc handler;
    uint16_t mask;
    uint16_t dummy;
} __attribute__((aligned(4), packed));

#define MAX_ENTRIES 15
IWRAM_DATA struct IntTable interruptTable[MAX_ENTRIES];
IWRAM_FUNC void dummy(void) {};

IWRAM_FUNC void irqInit()
{
    // clear all interrupt functions
    for (uint32_t i = 0; i < MAX_ENTRIES; i++)
    {
        interruptTable[i].handler = dummy;
        interruptTable[i].mask = 0;
    }
    INT_VECTOR = DispatchInterrupt;
}

IWRAM_FUNC IntFunc irqSet(IRQMask mask, IntFunc function)
{
    uint32_t i = 0;
    for (; i < MAX_ENTRIES; i++)
    {
        if (!interruptTable[i].mask || interruptTable[i].mask == static_cast<uint16_t>(mask))
        {
            break;
        }
    }
    if (i >= MAX_ENTRIES)
    {
        return nullptr;
    }
    interruptTable[i].handler = function;
    interruptTable[i].mask = static_cast<uint16_t>(mask);
    return interruptTable[i].handler;
}

inline bool contains(IRQMask lhs, IRQMask rhs)
{
    return (static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs)) != 0;
}

IWRAM_FUNC void irqEnable(IRQMask mask)
{
    REG_IME = uint16_t(0);
    if (contains(mask, IRQMask::IRQ_VBLANK))
    {
        REG_DISPSTAT |= LCDC_VBL;
    }
    if (contains(mask, IRQMask::IRQ_HBLANK))
    {
        REG_DISPSTAT |= LCDC_HBL;
    }
    if (contains(mask, IRQMask::IRQ_VCOUNT))
    {
        REG_DISPSTAT |= LCDC_VCNT;
    }
    REG_IE |= static_cast<uint16_t>(mask);
    REG_IME = uint16_t(1);
}

IWRAM_FUNC void irqDisable(IRQMask mask)
{
    REG_IME = uint16_t(0);
    if (contains(mask, IRQMask::IRQ_VBLANK))
    {
        REG_DISPSTAT &= ~LCDC_VBL;
    }
    if (contains(mask, IRQMask::IRQ_HBLANK))
    {
        REG_DISPSTAT &= ~LCDC_HBL;
    }
    if (contains(mask, IRQMask::IRQ_VCOUNT))
    {
        REG_DISPSTAT &= ~LCDC_VCNT;
    }
    REG_IE &= ~static_cast<uint16_t>(mask);
    REG_IME = uint16_t(1);
}
