#include "adpcm.h"

#include "adpcm_structs.h"
#include "adpcm_tables.h"

#define ADPCM_DITHER
static constexpr uint32_t ADPCM_DITHER_SHIFT = 24;
// #define ADPCM_ROUNDING
static constexpr uint32_t ADPCM_RESAMPLER_PRECISION = 18; // this is good up to 262144 Hz sample rate
static constexpr uint32_t ADPCM_POSITION_ONE = (1 << ADPCM_RESAMPLER_PRECISION);
static constexpr uint32_t ADPCM_POSITION_TWO = (2 << ADPCM_RESAMPLER_PRECISION);

namespace Adpcm
{

    auto UpsampleInit(uint32_t srcRateHz, uint32_t dstRateHz) -> void
    {
        ADPCM_LinearResamplerData[0].history[0] = 0;
        ADPCM_LinearResamplerData[0].history[1] = 0;
        ADPCM_LinearResamplerData[0].position = 2 << ADPCM_RESAMPLER_PRECISION;
        ADPCM_LinearResamplerData[0].step = (static_cast<uint64_t>(srcRateHz) << ADPCM_RESAMPLER_PRECISION) / dstRateHz;
        ADPCM_LinearResamplerData[1].history[0] = 0;
        ADPCM_LinearResamplerData[1].history[1] = 0;
        ADPCM_LinearResamplerData[1].position = 2 << ADPCM_RESAMPLER_PRECISION;
        ADPCM_LinearResamplerData[1].step = (static_cast<uint64_t>(srcRateHz) << ADPCM_RESAMPLER_PRECISION) / dstRateHz;
    }

    auto UnCompWrite32bit_8bit_upsample(const void *data, uint32_t dataSize, uint8_t *dst[2]) -> uint32_t
    {
        //  copy frame header and skip to data
        const Audio::AdpcmFrameHeader frameHeader = Audio::AdpcmFrameHeader::read(data);
        auto data8 = reinterpret_cast<const uint8_t *>(data) + sizeof(Audio::AdpcmFrameHeader);
        // uncompress 4-bit ADPCM samples. These are stored planar / per channel, e.g. L0 L1 ... R0 R1 ...
        const auto adpcmChannelNrOfSamples = (frameHeader.uncompressedSize / sizeof(int16_t)) / frameHeader.nrOfChannels;
        // we decode one less nibble than samples, as the first sample is verbatim
        const auto adpcmChannelNrOfNibbles = adpcmChannelNrOfSamples - 1;
        uint32_t dstSamplesGenerated = 0;
        for (uint32_t channel = 0; channel < frameHeader.nrOfChannels; ++channel)
        {
            // align output buffer to next word boundary
            auto dst8 = dst[channel];
            // first sample is stored verbatim in header
            int32_t pcmData = static_cast<int16_t>(data8[0] | (data8[1] << 8));
            int32_t index = static_cast<int16_t>(data8[2] | (data8[3] << 8));
            data8 += 4;
            // we can have history from previous calls, but at most one sample,
            // because we must have output data until resampler.position >= ADPCM_POSITION_ONE
            // we can also have no history at all for a first call
            // this means we need at least one new sample here
            auto &resampler = ADPCM_LinearResamplerData[channel];
            if (resampler.position >= ADPCM_POSITION_TWO)
            {
                // no samples for a first call
                resampler.history[0] = pcmData;
                resampler.history[1] = pcmData;
                resampler.prev = pcmData;
                resampler.position -= ADPCM_POSITION_ONE;
            }
            else if (resampler.position >= ADPCM_POSITION_ONE)
            {
                // one sample from previous call
                auto pcmFiltered = resampler.prev + (resampler.prev >> 1) - (pcmData >> 1);
                resampler.history[0] = resampler.history[1];
                resampler.history[1] = pcmFiltered;
                resampler.prev = pcmData;
                resampler.position -= ADPCM_POSITION_ONE;
            }
            else
            {
                // technically an error. should never happen
                return 0;
            }
            // start output loop
            uint32_t adpcmNibblesDecoded = 0;
            uint32_t nibbles = 0;
            while (adpcmNibblesDecoded < adpcmChannelNrOfNibbles)
            {
                // check if we need more input samples
                if (resampler.position >= ADPCM_POSITION_ONE)
                {
                    // load two ADPCM nibbles every 2 ADPCM samples
                    if ((adpcmNibblesDecoded & 1) == 0)
                    {
                        nibbles = *data8++;
                    }
                    // decode nibble
                    uint32_t delta = ADPCM_DeltaTable_4bit[index][nibbles & 0x07];
                    pcmData += (nibbles & 8) ? -delta : delta;
                    index += ADPCM_IndexTable_4bit[nibbles & 0x07];
                    index = index < 0 ? 0 : index;
                    index = index > 88 ? 88 : index;
                    // clamp sample value
                    pcmData = pcmData < -32768 ? -32768 : pcmData;
                    pcmData = pcmData > 32767 ? 32767 : pcmData;
                    // apply a 2-tap FIR 6dB high-shelf filter to counter the "muffling" of the audio
                    // y[n] = x[n-1] * a0 + x[n] * a1
                    // Gain = [high gain]
                    // a0 = 0.5 * (1 + Gain)
                    // a1 = 0.5 * (1 - Gain)
                    // Gain = 10^(x/20) -> 10^(6/20)
                    // Thus @ 6dB:
                    // a0 = 1.5
                    // a1 = -0.5
                    auto pcmFiltered = resampler.prev + (resampler.prev >> 1) - (pcmData >> 1);
                    // move new PCM data to resampler history
                    resampler.history[0] = resampler.history[1];
                    resampler.history[1] = pcmFiltered;
                    resampler.prev = pcmData;
                    resampler.position -= ADPCM_POSITION_ONE;
                    // move next nibble into position
                    nibbles >>= 4;
                    adpcmNibblesDecoded++;
                }
                // check if we can produce new output samples
                while (resampler.position < ADPCM_POSITION_ONE)
                {
                    // linear interpolation of samples
                    int32_t diff = static_cast<int32_t>(resampler.history[1]) - static_cast<int32_t>(resampler.history[0]);
                    int32_t sample = static_cast<int32_t>(resampler.history[0]) + (diff * (resampler.position >> (ADPCM_RESAMPLER_PRECISION - 8)) >> 8);
                    resampler.position += resampler.step;
                    // write to output buffer
                    *dst8++ = sample >> 8;
                    dstSamplesGenerated++;
                }
            }
        }
        return dstSamplesGenerated;
    }

    auto IWRAM_FUNC UnCompWrite32bit_8bit(const void *data, uint32_t dataSize, uint8_t *dst[2]) -> uint32_t
    {
        //  copy frame header and skip to data
        const Audio::AdpcmFrameHeader frameHeader = Audio::AdpcmFrameHeader::read(data);
        auto data8 = reinterpret_cast<const uint8_t *>(data) + sizeof(Audio::AdpcmFrameHeader);
        // uncompress 4-bit ADPCM samples. These are stored planar / per channel, e.g. L0 L1 ... R0 R1 ...
        const auto adpcmDataSize = dataSize - sizeof(Audio::AdpcmFrameHeader);
        const auto adpcmChannelBlockSize = adpcmDataSize / frameHeader.nrOfChannels;
        uint32_t dstSamplesGenerated = frameHeader.nrOfChannels;
        for (uint32_t channel = 0; channel < frameHeader.nrOfChannels; ++channel)
        {
            // align output buffer to next word boundary
            auto dst8 = dst[channel];
            // first sample is stored verbatim in header
            int32_t pcmData = static_cast<int16_t>(data8[0] | (data8[1] << 8));
#ifdef ADPCM_DITHER
            pcmData += (ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT) - ADPCM_DitherState[0];
            ADPCM_DitherState[0] = ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT;
            ADPCM_DitherState[1] = ((ADPCM_DitherState[1] << 4) - ADPCM_DitherState[1]) ^ 1;
            pcmData = pcmData < -32768 ? -32768 : pcmData;
            pcmData = pcmData > 32767 ? 32767 : pcmData;
#endif
#ifdef ADPCM_ROUNDING
            *dst8++ = (pcmData + 128) >> 8;
#else
            *dst8++ = pcmData >> 8;
#endif
            int32_t index = static_cast<int16_t>(data8[2] | (data8[3] << 8));
            data8 += 4;
            uint32_t bytesLeft = adpcmChannelBlockSize - 4;
            while (bytesLeft--)
            {
                // get two ADPCM nibbles
                const uint32_t nibbles = *data8;
                // decode first nibble
                uint32_t delta = ADPCM_DeltaTable_4bit[index][nibbles & 0x07];
                if (nibbles & 8)
                    pcmData -= delta;
                else
                    pcmData += delta;
                index += ADPCM_IndexTable_4bit[nibbles & 0x07];
                index = index < 0 ? 0 : index;
                index = index > 88 ? 88 : index;
#ifdef ADPCM_DITHER
                pcmData += (ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT) - ADPCM_DitherState[0];
                ADPCM_DitherState[0] = ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT;
                ADPCM_DitherState[1] = ((ADPCM_DitherState[1] << 4) - ADPCM_DitherState[1]) ^ 1;
#endif
                pcmData = pcmData < -32768 ? -32768 : pcmData;
                pcmData = pcmData > 32767 ? 32767 : pcmData;
#ifdef ADPCM_ROUNDING
                *dst8++ = (pcmData + 128) >> 8;
#else
                *dst8++ = pcmData >> 8;
#endif
                dstSamplesGenerated++;
                // decode second nibble only if not last sample
                if (bytesLeft > 0)
                {
                    delta = ADPCM_DeltaTable_4bit[index][(nibbles >> 4) & 0x07];
                    if ((nibbles >> 4) & 8)
                        pcmData -= delta;
                    else
                        pcmData += delta;
                    index += ADPCM_IndexTable_4bit[(nibbles >> 4) & 0x07];
                    index = index < 0 ? 0 : index;
                    index = index > 88 ? 88 : index;
#ifdef ADPCM_DITHER
                    pcmData += (ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT) - ADPCM_DitherState[0];
                    ADPCM_DitherState[0] = ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT;
                    ADPCM_DitherState[1] = ((ADPCM_DitherState[1] << 4) - ADPCM_DitherState[1]) ^ 1;
#endif
                    pcmData = pcmData < -32768 ? -32768 : pcmData;
                    pcmData = pcmData > 32767 ? 32767 : pcmData;
#ifdef ADPCM_ROUNDING
                    *dst8++ = (pcmData + 128) >> 8;
#else
                    *dst8++ = pcmData >> 8;
#endif
                    dstSamplesGenerated++;
                }
                // advance input data
                data8++;
            }
        }
        return dstSamplesGenerated;
    }

    uint32_t IWRAM_FUNC UnCompGetSize_8bit(const void *data)
    {
        const Audio::AdpcmFrameHeader frameHeader = Audio::AdpcmFrameHeader::read(data);
        // if we're down-converting the PCM sample depth during decompression, adjust the uncompressed data size too
        return (static_cast<uint32_t>(frameHeader.uncompressedSize) * 8 + 7) / frameHeader.pcmBitsPerSample; // + frameHeader.nrOfChannels;
    }
}
