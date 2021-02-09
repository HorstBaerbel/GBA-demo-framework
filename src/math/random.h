#pragma once

#include <gba_base.h>

/// @brief Generate random number in the range of T,
/// e.g. random<uint16_t>() -> [0,65535]
template <typename T>
T random() IWRAM_CODE;
