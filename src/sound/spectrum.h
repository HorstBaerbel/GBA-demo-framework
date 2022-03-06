#pragma once

#include <cstdint>

/// @brief FFT spectrum functions
namespace Spectrum
{
    constexpr uint32_t NrOfBands = 6;
    using Bands = uint8_t[NrOfBands];

    constexpr uint32_t BandMaxValue = 127; // Maximum value a result band can have

    /// @brief Calculate spectrum from signed 8bit stereo samples
    /// @param left Left channel samples
    /// @param right Right channel samples
    /// @param size Number of samples in each buffer
    void update(const int8_t *left, const int8_t *right, uint32_t size);

    /// @brief Get current spectrum. Use update() to calculate
    /// @param result Returned spectrum
    const Bands &getSpectrum();

}
