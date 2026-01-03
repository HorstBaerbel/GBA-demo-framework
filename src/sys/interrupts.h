#pragma once

#include "base.h"

#include <cstdint>

namespace Irq
{
    /// @brief An interrupt function pointer
    using IntFunc = void (*)(void);

    /// @brief Interrupt Master Enable Register
    /// When bit 0 is clear, all interrupts are masked. When it is 1, interrupts will occur if not masked out in RegIe
    inline auto &RegIme{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0208)};

    /// @brief Interrupt Enable Register
    /// This is the activation mask for the internal interrupts. Unless the corresponding bit is set, the IRQ will be masked out
    inline auto &RegIe{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0200)};

    /// @brief Interrupt Flag Register
    /// Since there is only one hardware interrupt vector, the IF register contains flags to indicate when a particular of interrupt has occured.
    /// To acknowledge processing interrupts, set IF to the value of the interrupt handled
    inline auto &RegIf{*reinterpret_cast<volatile uint16_t *>(REG_BASE + 0x0202)};

    /// @brief Dummy IRQ handler
    auto DummyHandler(void) -> void;

    /// @brief These masks are used in conjuction with REG_IE to enable specific interrupts
    /// and with REG_IF to acknowledge interrupts have been serviced
    enum Mask : uint16_t
    {
        VBlank = (1 << 0),  // VBlank interrupt mask
        HBlank = (1 << 1),  // HBlank interrupt mask
        VCount = (1 << 2),  // VCount match interrupt mask
        Timer0 = (1 << 3),  // Timer 0 interrupt mask
        Timer1 = (1 << 4),  // Timer 1 interrupt mask
        Timer2 = (1 << 5),  // Timer 2 interrupt mask
        Timer3 = (1 << 6),  // Timer 3 interrupt mask
        Serial = (1 << 7),  // Serial interrupt mask
        DMA0 = (1 << 8),    // DMA 0 interrupt mask
        DMA1 = (1 << 9),    // DMA 1 interrupt mask
        DMA2 = (1 << 10),   // DMA 2 interrupt mask
        DMA3 = (1 << 11),   // DMA 3 interrupt mask
        KeyPad = (1 << 12), // Keypad interrupt mask
        GamePak = (1 << 13) // GamePak interrupt mask
    };

    /// @brief Initialises the GBA interrupt code
    void init();

    /// @brief Get currently set interrupt handler for a particular interrupt
    /// @param mask Which interrupts to get the handler for
    /// @return Returns the handler if successful, nullptr if it failed
    IntFunc getHandler(Mask mask);

    /// @brief sets the interrupt handler for a particular interrupt
    /// @param mask Which interrupts to call the handler for
    /// @param function Handler function
    /// @return Returns the handler if successful, nullptr if it failed
    IntFunc setHandler(Mask mask, IntFunc function);

    /// @brief Get current interrupt mask settings
    /// @return Current interrupt mask settings
    auto getMask() -> Mask;

    /// @brief allows an interrupt to occur
    /// @param mask
    void enable(Mask mask);

    /// @brief prevents an interrupt occuring
    /// @param mask
    void disable(Mask mask);
}
