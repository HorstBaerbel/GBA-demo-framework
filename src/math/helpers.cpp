#include "helpers.h"

template <>
uint32_t countLeadingZeros(uint32_t x)
{
    uint32_t exp = 32;
    if (x & 0xffff0000)
    {
        exp -= 16;
        x >>= 16;
    }
    if (x & 0xff00)
    {
        exp -= 8;
        x >>= 8;
    }
    if (x & 0xf0)
    {
        exp -= 4;
        x >>= 4;
    }
    if (x & 0xc)
    {
        exp -= 2;
        x >>= 2;
    }
    exp -= (x & 0x2);
    exp -= (x & 0x1);
    return exp;
}

template <>
uint32_t countLeadingZeros(uint16_t x)
{
    uint32_t exp = 16;
    if (x & 0xff00)
    {
        exp -= 8;
        x >>= 8;
    }
    if (x & 0xf0)
    {
        exp -= 4;
        x >>= 4;
    }
    if (x & 0xc)
    {
        exp -= 2;
        x >>= 2;
    }
    exp -= (x & 0x2);
    exp -= (x & 0x1);
    return exp;
}

template <>
uint32_t countTrailingZeros(uint32_t x)
{
    uint32_t exp = 0;
    if ((x & 0x0000ffff) == 0)
    {
        exp += 16;
        x >>= 16;
    }
    if ((x & 0x00ff) == 0)
    {
        exp += 8;
        x >>= 8;
    }
    if ((x & 0x0f) == 0)
    {
        exp += 4;
        x >>= 4;
    }
    if ((x & 0x3) == 0)
    {
        exp += 2;
        x >>= 2;
    }
    exp += ((x & 0x2) == 0);
    exp += ((x & 0x1) == 0);
    return exp;
}

template <>
uint32_t countTrailingZeros(uint16_t x)
{
    uint32_t exp = 0;
    if ((x & 0x00ff) == 0)
    {
        exp += 8;
        x >>= 8;
    }
    if ((x & 0x0f) == 0)
    {
        exp += 4;
        x >>= 4;
    }
    if ((x & 0x3) == 0)
    {
        exp += 2;
        x >>= 2;
    }
    exp += ((x & 0x2) == 0);
    exp += ((x & 0x1) == 0);
    return exp;
}

template <>
uint32_t highestBitSet(int32_t x)
{
    // mask out sign bit and count how many bits are set
    return countLeadingZeros(static_cast<uint32_t>(x < 0 ? -x : x));
}

template <>
uint32_t highestBitSet(int16_t x)
{
    // mask out sign bit and count how many bits are set
    return countLeadingZeros(static_cast<uint16_t>(x < 0 ? -x : x));
}

template <>
uint32_t lowestBitSet(int32_t x)
{
    // mask out sign bit and count how many bits are set
    return countTrailingZeros(static_cast<uint32_t>(x < 0 ? -x : x));
}

template <>
uint32_t lowestBitSet(int16_t x)
{
    // mask out sign bit and count how many bits are set
    return countTrailingZeros(static_cast<uint16_t>(x < 0 ? -x : x));
}

template <>
bool isAddSafe(int32_t a, int32_t b)
{
    uint32_t a_bits = highestBitSet(a);
    uint32_t b_bits = highestBitSet(b);
    return (a_bits < 31 && b_bits < 31);
}

template <>
bool isAddSafe(int16_t a, int16_t b)
{
    uint32_t a_bits = highestBitSet(a);
    uint32_t b_bits = highestBitSet(b);
    return (a_bits < 15 && b_bits < 15);
}

template <>
bool isMulSafe(int32_t a, int32_t b)
{
    uint32_t a_bits = highestBitSet(a);
    uint32_t b_bits = highestBitSet(b);
    return (a_bits + b_bits <= 31);
}

template <>
bool isMulSafe(int16_t a, int16_t b)
{
    uint32_t a_bits = highestBitSet(a);
    uint32_t b_bits = highestBitSet(b);
    return (a_bits + b_bits <= 15);
}
