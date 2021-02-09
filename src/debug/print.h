#pragma once

#include "args.h"

/// @brief Output formatted values or strings to emulators debug console
/// Supported formats are %d, %x, %s.
/// Fixed point values will be formatted as floating point values.
template <typename... Args>
void printf(const char *format, const Args &... args)
{
    Arg arg_array[] = {args...};
    do_printf(format, arg_array, sizeof...(Args));
}
