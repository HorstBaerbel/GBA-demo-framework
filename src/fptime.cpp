#include "fptime.h"
#include "sys/interrupts.h"

#include "sys/timers.h"

// The system clock is 16.78MHz (F=16*1024*1024 Hz), one cycle is thus approx. 59.59ns
constexpr uint16_t TimerDividerBits = 2;                  // (0=F/1, 1=F/64, 2=F/256, 3=F/1024)
constexpr int32_t TimerIncrement = 164;                   // 65536/164=400 -> 1/400=2.5ms
constexpr uint16_t TimerVblankTime = 1097;                // ,269248 == every ~16.743 ms
constexpr int32_t TimerVblankInterval = ((1 << 16) / 60); // vblank interval as 16.16 fixed-point

namespace Time
{

    IWRAM_DATA static int32_t current = 0;                // Time since timer was started
    IWRAM_DATA static int32_t increment = TimerIncrement; // Timer increment value added every tick

    // Called each timer tick to increase time value
    IWRAM_FUNC void timerTick()
    {
        current += increment;
    }

    void start()
    {
        irqSet(IRQMask::IRQ_TIMER3, timerTick);
        irqEnable(IRQMask::IRQ_TIMER3);
        REG_TM3CNT_L = 65536 - TimerIncrement;
        REG_TM3CNT_H = TIMER_START | TIMER_IRQ | TimerDividerBits;
    }

    void stop()
    {
        irqDisable(IRQMask::IRQ_TIMER3);
    }

    IWRAM_FUNC TimePoint now()
    {
        return Math::fp1616_t::fromRaw(current);
    }

    void setNow(TimePoint time)
    {
        current = time.raw();
    }

    void setScale(Math::fp1616_t scaleBy)
    {
        increment = (int32_t)(scaleBy * Math::fp1616_t::fromRaw(TimerIncrement));
    }

    void startTimer()
    {
        REG_TM2CNT_L = 0;
        REG_TM2CNT_H = TIMER_START | 2;
    }

    Duration endTimer()
    {
        REG_TM2CNT_H = 0;
        return Math::fp1616_t::fromRaw(REG_TM2CNT_L);
    }

} //namespace Time
