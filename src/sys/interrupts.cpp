#include "interrupts.h"

#include "sys/base.h"
#include "sys/video.h"

/// @brief Interrupt dispatch function in intdispatcher.s
extern "C" void DispatchInterrupt();

struct IntTable
{
    Irq::IntFunc handler;
    uint16_t mask;
    uint16_t dummy;
} __attribute__((aligned(4), packed));

#define MAX_ENTRIES 15
IWRAM_DATA struct IntTable interruptTable[MAX_ENTRIES];

namespace Irq
{
#define INT_VECTOR *(IntFunc *)(0x03007ffc)
    IWRAM_FUNC void DummyHandler(void) {};

    IWRAM_FUNC void init()
    {
        // clear all interrupt functions
        for (uint32_t i = 0; i < MAX_ENTRIES; i++)
        {
            interruptTable[i].handler = DummyHandler;
            interruptTable[i].mask = 0;
        }
        INT_VECTOR = DispatchInterrupt;
    }

    IWRAM_FUNC IntFunc getHandler(Mask mask)
    {
        for (uint32_t i = 0; i < MAX_ENTRIES; i++)
        {
            if (interruptTable[i].mask == static_cast<uint16_t>(mask))
            {
                return interruptTable[i].handler;
            }
        }
        return nullptr;
    }

    IWRAM_FUNC IntFunc setHandler(Mask mask, IntFunc function)
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

    IWRAM_FUNC auto getMask() -> Mask
    {
        return static_cast<Mask>(RegIe);
    }

    inline bool contains(Mask lhs, Mask rhs)
    {
        return (static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs)) != 0;
    }

    IWRAM_FUNC void enable(Mask mask)
    {
        RegIme = uint16_t(0);
        if (contains(mask, Mask::VBlank))
        {
            REG_DISPSTAT |= LCDC_VBL;
        }
        if (contains(mask, Mask::HBlank))
        {
            REG_DISPSTAT |= LCDC_HBL;
        }
        if (contains(mask, Mask::VCount))
        {
            REG_DISPSTAT |= LCDC_VCNT;
        }
        RegIe |= static_cast<uint16_t>(mask);
        RegIme = uint16_t(1);
    }

    IWRAM_FUNC void disable(Mask mask)
    {
        RegIme = uint16_t(0);
        if (contains(mask, Mask::VBlank))
        {
            REG_DISPSTAT &= ~LCDC_VBL;
        }
        if (contains(mask, Mask::HBlank))
        {
            REG_DISPSTAT &= ~LCDC_HBL;
        }
        if (contains(mask, Mask::VCount))
        {
            REG_DISPSTAT &= ~LCDC_VCNT;
        }
        RegIe &= ~static_cast<uint16_t>(mask);
        RegIme = uint16_t(1);
    }
}