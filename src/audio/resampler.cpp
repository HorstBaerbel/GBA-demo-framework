#include "resampler.h"

#ifdef __arm__
#include "sys/base.h"
#endif

namespace Audio
{

    auto ResamplerInit(LinearResamplerData &resamplerData, uint32_t srcRateHz, uint32_t dstRateHz) -> void
    {
        const uint32_t resamplerStep = (static_cast<uint64_t>(srcRateHz) << RESAMPLER_PRECISION) / dstRateHz;
        resamplerData[0].pcmHistory[0] = 0;
        resamplerData[0].pcmHistory[1] = 0;
        resamplerData[0].pcmRaw = 0;
        resamplerData[0].position = RESAMPLER_POSITION_TWO;
        resamplerData[0].step = resamplerStep;
        resamplerData[1].pcmHistory[0] = 0;
        resamplerData[1].pcmHistory[1] = 0;
        resamplerData[1].pcmRaw = 0;
        resamplerData[1].position = RESAMPLER_POSITION_TWO;
        resamplerData[1].step = resamplerStep;
    }

}
