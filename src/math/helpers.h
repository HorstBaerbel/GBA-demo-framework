#pragma once

#include <cstdint>

// Count leading zeros from start to end
// returns 32 when x is empty
template <typename T>
uint32_t countLeadingZeros(T x);

// Count trailing zeros from end to start
// return 32 if x is empty
template <typename T>
uint32_t countTrailingZeros(T x);

// Return position of highest bit set
template <typename T>
uint32_t highestBitSet(T x);

// Return position of lowest bit set
template <typename T>
uint32_t lowestBitSet(T x);

// Check if adding two numbers does not overflow the type
template <typename T>
bool isAddSafe(T a, T b);

// Check if multiplying two numbers does not overflow the type
template <typename T>
bool isMulSafe(T a, T b);
