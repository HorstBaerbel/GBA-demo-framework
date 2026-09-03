#pragma once

#ifndef __ASSEMBLER__
#include <cstdint>

namespace Audio
{

    static constexpr uint32_t RESAMPLER_PRECISION = 18; // this is good up to 262144 Hz sample rate
    static constexpr int32_t RESAMPLER_POSITION_ONE = (1 << RESAMPLER_PRECISION);
    static constexpr int32_t RESAMPLER_POSITION_TWO = (2 << RESAMPLER_PRECISION);

    /// @brief Stores resampler data for linear audio resampling and filtering
    struct LinearResamplerChannelData
    {
        int16_t pcmHistory[2]; // history of two previous filtered 16-bit PCM samples
        int16_t pcmRaw;        // previous unfiltered 16-bit PCM sample
        int32_t position;      // current interpolated sample position in Q14.18 format (good up to 262144 Hz sample rate)
        int32_t step;          // input sample rate / output sample rate in Q14.18 format (good up to 262144 Hz sample rate)
    } __attribute__((aligned(4)));

    /// @brief Resampler data for stereo samples
    using LinearResamplerData = LinearResamplerChannelData[2];

}
#else

#define RESAMPLER_PRECISION 18 // this is good up to 262144 Hz sample rate
#define RESAMPLER_POSITION_ONE (1 << RESAMPLER_PRECISION)
#define RESAMPLER_POSITION_TWO (2 << RESAMPLER_PRECISION)
#define RESAMPLER_OFFSET_PCM_HISTORY_0 0
#define RESAMPLER_OFFSET_PCM_HISTORY_1 2
#define RESAMPLER_OFFSET_PCM_RAW 4
#define RESAMPLER_OFFSET_POSITION 8
#define RESAMPLER_OFFSET_STEP 12
#define RESAMPLER_SIZEOF_CHANNEL_DATA 16

#endif
