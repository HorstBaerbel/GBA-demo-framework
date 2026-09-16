#pragma once

#include <cstdint>

extern const uint32_t rcp_sqrt_tab[96];
extern const uint32_t rcp_tab[8];

uint16_t sqrt(uint16_t x, uint32_t fracBits);
uint32_t sqrt(uint32_t x, uint32_t fracBits);

// Faster than sqrt(), but not as precise
uint16_t sqrtFast(uint16_t x, uint32_t fracBits);
uint32_t sqrtFast(uint32_t x, uint32_t fracBits);

uint16_t sqrtRecip(uint16_t x);
uint32_t sqrtRecip(uint32_t x);

// Faster than sqrtRecip(), but not as precise
uint16_t sqrtRecipFast(uint16_t x);
uint32_t sqrtRecipFast(uint32_t x);
