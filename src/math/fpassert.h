#pragma once

#include "debug_print.h"
#include "fphelpers.h"

extern u32 fpassert_precision_count; //number of operations that overflowed (e.g. multiply)
extern u32 fpassert_overflow_count; //number of operations that incurred precision loss (e.g. conversion 16.16->24.8)
extern u32 fpassert_error_count; //number of operations with error (e.g. division by 0 or sqrt of negative number)

#define fpassert_precision(expr, print_message) \
    if (!(expr)) { fpassert_precision_count++; if (print_message) { print("precision assert") } }
#define fpassert_overflow(expr, print_message) \
    if (!(expr)) { fpassert_overflow_count++; if (print_message) { print("over-/underflow assert") } }
#define fpassert_error(expr, print_message) \
    if (!(expr)) { fpassert_error_count++; if (print_message) { print("error assert") } }
