#include "random.h"

#include <cstdint>

extern "C" [[nodiscard]] uint32_t random_u32() IWRAM_FUNC;

template <>
bool random()
{
    return (random_u32() & 1) != 0;
}

template <>
int8_t random()
{
    return (int8_t(random_u32() & 0xFF)) - 127;
}

template <>
uint8_t random()
{
    return random_u32() & 0xFF;
}

template <>
int16_t random()
{
    return (int16_t(random_u32() & 0xFFFF)) - 32767;
}

template <>
uint16_t random()
{
    return random_u32() & 0xFFFF;
}

template <>
int32_t random()
{
    return random_u32();
}

template <>
uint32_t random()
{
    return random_u32();
}
