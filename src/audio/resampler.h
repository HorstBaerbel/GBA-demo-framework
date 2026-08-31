#pragma once

#include <cstdint>

namespace Audio
{

    static constexpr uint32_t RESAMPLER_PRECISION = 18; // this is good up to 262144 Hz sample rate
    static constexpr uint32_t RESAMPLER_POSITION_ONE = (1 << RESAMPLER_PRECISION);
    static constexpr uint32_t RESAMPLER_POSITION_TWO = (2 << RESAMPLER_PRECISION);

    /// @brief Stores resampler data for linear audio resampling and filtering
    struct LinearResamplerChannelData
    {
        int16_t pcmHistory[2]; // history of two previous filtered 16-bit PCM samples
        int16_t pcmRaw;        // previous unfiltered 16-bit PCM sample
        uint32_t position;  // current interpolated sample position in Q14.18 format (good up to 262144 Hz sample rate)
        uint32_t step;      // input sample rate / output sample rate in Q14.18 format (good up to 262144 Hz sample rate)
    } __attribute__((aligned(4)));

    /// @brief Resampler data for stereo samples
    using LinearResamplerData = LinearResamplerChannelData[2];

    /// @brief Initialize linear resampling
    /// @param resamplerData Resampler state
    /// @param srcRateHz Input sample rate in Hz
    /// @param dstRateHz Output sample rate in Hz
    auto ResamplerInit(LinearResamplerData &resamplerData, uint32_t srcRateHz, uint32_t dstRateHz) -> void;
}
