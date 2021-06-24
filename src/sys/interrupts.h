#pragma once

#include <cstdint>

// Interrupt system compatible with libgba

/// @brief An interrupt function pointer.
using IntFunc = void (*)(void);

/// @brief Interrupt Master Enable Register.
/// When bit 0 is clear, all interrupts are masked.  When it is 1, interrupts will occur if not masked out in REG_IE.
#define REG_IME *(volatile uint16_t *)(0x04000000 + 0x208)

/// @brief Interrupt Enable Register.
/// This is the activation mask for the internal interrupts.  Unless the corresponding bit is set, the IRQ will be masked out.
#define REG_IE *(volatile uint16_t *)(0x04000000 + 0x200)

/// @brief Interrupt Flag Register.
/// Since there is only one hardware interrupt vector, the IF register contains flags to indicate when a particular of interrupt has occured.
/// To acknowledge processing interrupts, set IF to the value of the interrupt handled.
#define REG_IF *(volatile uint16_t *)(0x04000000 + 0x202) // Interrupt Request

/// @brief These masks are used in conjuction with REG_IE to enable specific interrupts
/// and with REG_IF to acknowledge interrupts have been serviced.
enum class IRQMask : uint16_t
{
    IRQ_VBLANK = (1 << 0),  /*!< vertical blank interrupt mask */
    IRQ_HBLANK = (1 << 1),  /*!< horizontal blank interrupt mask */
    IRQ_VCOUNT = (1 << 2),  /*!< vcount match interrupt mask */
    IRQ_TIMER0 = (1 << 3),  /*!< timer 0 interrupt mask */
    IRQ_TIMER1 = (1 << 4),  /*!< timer 1 interrupt mask */
    IRQ_TIMER2 = (1 << 5),  /*!< timer 2 interrupt mask */
    IRQ_TIMER3 = (1 << 6),  /*!< timer 3 interrupt mask */
    IRQ_SERIAL = (1 << 7),  /*!< serial interrupt mask */
    IRQ_DMA0 = (1 << 8),    /*!< DMA 0 interrupt mask */
    IRQ_DMA1 = (1 << 9),    /*!< DMA 1 interrupt mask */
    IRQ_DMA2 = (1 << 10),   /*!< DMA 2 interrupt mask */
    IRQ_DMA3 = (1 << 11),   /*!< DMA 3 interrupt mask */
    IRQ_KEYPAD = (1 << 12), /*!< Keypad interrupt mask */
    IRQ_GAMEPAK = (1 << 13) /*!< horizontal blank interrupt mask */
};

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief Initialises the gba interrupt code.
    void irqInit();

    /// @brief sets the interrupt handler for a particular interrupt.
    /// @param mask
    /// @param function
    IntFunc irqSet(IRQMask mask, IntFunc function);

    /// @brief allows an interrupt to occur.
    /// @param mask
    void irqEnable(IRQMask mask);

    /// @brief prevents an interrupt occuring.
    /// @param mask
    void irqDisable(IRQMask mask);

#ifdef __cplusplus
} // extern "C"
#endif
