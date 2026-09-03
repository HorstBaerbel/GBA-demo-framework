#pragma once

#include "resampler_structs.h"

#include <cstdint>

namespace Audio
{

    /// @brief Initialize linear resampling
    /// @param resamplerData Resampler state
    /// @param srcRateHz Input sample rate in Hz
    /// @param dstRateHz Output sample rate in Hz
    auto ResamplerInit(LinearResamplerData &resamplerData, uint32_t srcRateHz, uint32_t dstRateHz) -> void;
}
