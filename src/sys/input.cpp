#include "input.h"

#include "sys/halt.h"
#include "sys/interrupts.h"

namespace Input
{
    struct KeyInput
    {
        uint16_t up;
        uint16_t down;
        uint16_t held;
        uint16_t last;
        uint16_t downRepeat;
    } __attribute__((aligned(4), packed));

    static KeyInput Keys = {0, 0, 0, 0, 0};
    static constexpr uint8_t DelayCount = 60;
    static constexpr uint8_t RepeatCount = 30;
    static uint8_t heldCount = 60;
    static constexpr uint16_t KeyBitsMask = 0x03FF; // Mask lower 10 bits of key input

    auto scanKeys() -> void
    {
        Keys.last = Keys.held;
        Keys.held = (RegKeyInput & KeyBitsMask) ^ KeyBitsMask;
        const uint16_t pressed = Keys.held & (Keys.last ^ KeyBitsMask);
        Keys.downRepeat |= pressed;
        Keys.down |= pressed;
        const uint16_t released = ((Keys.held ^ KeyBitsMask) & Keys.last);
        Keys.up |= released;
        Keys.down &= ~released;
        Keys.downRepeat &= ~released;
        Keys.up &= ~pressed;
        if (Keys.last != Keys.held)
        {
            heldCount = DelayCount;
        }
        if (DelayCount != 0)
        {
            heldCount--;
            if (heldCount == 0)
            {
                heldCount = RepeatCount;
                Keys.downRepeat |= Keys.held;
            }
        }
    }

    auto keysDown() -> KeyBits
    {
        auto tmp = Keys.down;
        Keys.down = 0;
        return static_cast<KeyBits>(tmp);
    }

    auto waitForKeysDown(KeyBits keys, bool halt) -> void
    {
        do
        {
            scanKeys();
            if (keysDown() & keys)
            {
                break;
            }
            if (halt)
            {
                // check if VBlank IRQ was set up and store old handler
                const auto wasEnabled = (Irq::getMask() & Irq::Mask::VBlank) != 0;
                const auto oldHandler = Irq::getHandler(Irq::Mask::VBlank);
                // set dummy handler and enable VBlank IRQ
                Irq::setHandler(Irq::Mask::VBlank, Irq::DummyHandler);
                Irq::enable(Irq::Mask::VBlank);
                Halt::Halt();
                // restore old VBlank handler and enable flag
                Irq::setHandler(Irq::Mask::VBlank, oldHandler);
                if (!wasEnabled)
                {
                    Irq::disable(Irq::Mask::VBlank);
                }
            }
        } while (true);
    }
}