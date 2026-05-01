#pragma once

#include <cstdint>

#include <sys/timers.h>

namespace Debug
{

    /// @brief Start a short-term timer (< 1s). For benchmarking etc.
    /// Runs on Timer #2.
    inline void startTimer()
    {
        REG_TM2CNT_L = 0;
        REG_TM2CNT_H = TIMER_START | 2;
    }

    /// @brief Pause the short-term timer.
    /// Runs on Timer #2.
    inline void pauseTimer()
    {
        REG_TM2CNT_H = 0;
    }

    /// @brief Continue the short-term timer after pauseTimer().
    /// Runs on Timer #2.
    inline void continueTimer()
    {
        // We need to set the whole register again, as this will keep the counter value.
        // Only setting the start bit will use the reload value, which is 0 (see above)
        REG_TM2CNT |= (TIMER_START | 2) << 16;
    }

    /// @brief Stop short-term timer and return result in s.
    /// Runs on Timer #2.
    inline int32_t endTimer()
    {
        REG_TM2CNT_H = 0;
        return REG_TM2CNT_L;
    }

} // namespace Time

/// @brief Start timing of a section
#define TIME_SECTION_START() Debug::startTimer();

/// @brief End timing of a section and print averaged runtime duration in ms
/// @param interval Sampling / averaging interval in seconds
/// @param printFunc Function that should print the section time, e.g. printf()
/// @param printString Format string that should be passed to printFunc, e.g. "%.2fms"
/// @note Function will pass the accumulated time in ms to the printFunc as a 16.16 fixed-point value.
///       Make sure you put this in a namespace, else you will get multiple defined symbols
#define TIME_SECTION_END(interval, printFunc, printString)                   \
    {                                                                        \
        static EWRAM_DATA int32_t sectionDuration = 0;                       \
        static EWRAM_DATA int32_t sectionCount = 0;                          \
        sectionDuration += Debug::endTimer();                                \
        sectionCount++;                                                      \
        if (sectionDuration >= interval)                                     \
        {                                                                    \
            printFunc(printString, (sectionDuration * 1000) / sectionCount); \
            sectionDuration = 0;                                             \
            sectionCount = 0;                                                \
        }                                                                    \
    }
