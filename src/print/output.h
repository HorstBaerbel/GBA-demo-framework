#pragma once

#include <cstdarg>
#include <cstddef>

namespace Debug
{
    /// @brief Maximum size of characters in print buffer
    constexpr unsigned int MaxPrintBufferSize = 128;

    /// @brief Print string to emulator
    void print(const char *s);

    /// @brief Print a formatted string to a buffer
    void vsnprintf(char *buf, size_t bufsz, const char *fmt, va_list args);

    /// @brief Print a formatted string to emulator
    void printf(const char *fmt, ...);
}
