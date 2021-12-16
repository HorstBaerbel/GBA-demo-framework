#pragma once

#include "math/fp32.h"
#include "print/print.h"
#include <string>
#include <utility>

/// @brief Return a properly scaled value for a time and its unit
/// If you do e.g. 1M calculations that take 37.5s, call with
/// timingUnit(37.5, 1000000) and it will return (37.5, "us"), for microseconds.
template <unsigned int F>
std::pair<Math::fp32_t<F>, const char *> timingValue(Math::fp32_t<F> time, int32_t divisor)
{
    const char *units[4] = {"s", "ms", "us", "ns"};
    //printf("time: %d\n", time);
    //printf("divisor: %d\n", divisor);
    int32_t timeDomain = 0;
    // check the domain of the divisor
    int32_t divDiv = 1;
    divisor = divisor < 0 ? -divisor : divisor;
    while (divisor / divDiv >= 10)
    {
        timeDomain--;
        divDiv *= 10;
    }
    Math::fp32_t<F> absTime = abs(time);
    // apply normalized scaling of divisor
    absTime = Math::fp32_t<F>::fromRaw(((int64_t)absTime.raw() * divDiv) / divisor);
    // check the domain of the resulting time
    int32_t timeMul = 1;
    int32_t timeDiv = 1;
    while (absTime / timeDiv >= 10)
    {
        timeDomain++;
        timeDiv *= 10;
    }
    while (absTime * timeMul < 1)
    {
        timeDomain--;
        timeMul *= 10;
    }
    // calculate the domain of the number
    //printf("timeDomain: %d\n", timeDomain);
    // move the number digits of the number according to the unit
    // numbers are always in [1, 1000) for each domain
    const char *unit = units[0];
    int64_t result = absTime.raw();
    if (timeDomain < -6)
    {
        result *= 1e9;
        unit = units[3];
    }
    else if (timeDomain < -3)
    {
        result *= 1e6;
        unit = units[2];
    }
    else if (timeDomain < 0)
    {
        result *= 1e3;
        unit = units[1];
    }
    return std::make_pair(Math::fp32_t<F>::fromRaw(result / divDiv), unit);
}
