#pragma once

#include <stdint.h>

extern const uint32_t rcp_sqrt_tab[96];
extern const uint32_t rcp_tab[8];

template <typename T>
T sqrt(T x, uint32_t fracBits);

// Faster than sqrt(), but not as precise
template <typename T>
T sqrtFast(T x, uint32_t fracBits);

template <typename T>
T sqrtRecip(T x);

// Faster than sqrtRecip(), but not as precise
template <typename T>
T sqrtRecipFast(T x);
