#pragma once

#include "sys/base.h"

#include <cstdint>

/// @brief Generate random number in the range of T,
/// e.g. random<uint16_t>() -> [0,65535]
template <typename T>
[[nodiscard]] T random() IWRAM_FUNC;

/// @brief Seed random number generator
extern "C" void srandom(uint32_t seed = 0x55A029A6) IWRAM_FUNC;