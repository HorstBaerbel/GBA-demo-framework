#include "adpcm.h"

#include "adpcm_structs.h"
#include "adpcm_tables.h"

namespace Audio
{

    // #define ADPCM_ROUNDING // Uncomment to apply rounding when converting to 8-bit
    // #define ADPCM_DITHER  // Uncomment to apply dithering to PCM data
    static constexpr uint32_t ADPCM_DITHER_SHIFT = 24;

#define ADPCM_APPLY_HIGH_SHELF // Uncomment to apply 3.5dB high-shelf filter before resampling

    /// @brief Decode one channel of ADPCM data
    /// @param data8 Input ADPCM data (pointer modified)
    /// @param dst8 Output sample buffer
    /// @param nrOfNibbles Number of ADPCM nibbles to decode
    auto IWRAM_FUNC ADPCMDecodeChannel_8bit(const uint8_t *&data8, uint8_t *dst8, const uint32_t nrOfNibbles) -> uint32_t
    {
        // first sample is stored verbatim in header
        int32_t pcmData = static_cast<int16_t>(data8[0] | (data8[1] << 8));
        int32_t index = static_cast<int16_t>(data8[2] | (data8[3] << 8));
        data8 += 4;
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
        uint32_t nibblesDecoded = 0;
        uint32_t nibbles = 0;
        while (nibblesDecoded < nrOfNibbles)
        {
            // load two ADPCM nibbles every 2 ADPCM samples
            if ((nibblesDecoded & 1) == 0)
            {
                nibbles = *data8++;
            }
            // decode nibble
            uint32_t delta = ADPCM_DeltaTable_4bit[index][nibbles & 0x07];
            pcmData += (nibbles & 8) ? -delta : delta;
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
            // move next nibble into position
            nibbles >>= 4;
            nibblesDecoded++;
        }
        return nrOfNibbles + 1;
    }

    /// @brief Decode one channel of ADPCM data while resampling
    /// Applies a 2-tap FIR 3.5dB high-shelf filter to counter the "muffling" of the audio
    /// y[n] = x[n-1] * a0 + x[n] * a1
    /// Gain = [high gain]
    /// a0 = 0.5 * (1 + Gain), a1 = 0.5 * (1 - Gain)
    /// Gain = 10^(x/20) -> 10^(6/20)
    /// Thus @ 3.5dB:
    /// a0 = 1.25, a1 = -0.25
    /// @param data8 Input ADPCM data (pointer modified)
    /// @param dst8 Output sample buffer
    /// @param nrOfNibbles Number of ADPCM nibbles to decode
    /// @param resamplerData Pointer to reampler runtime information (content modified)
    auto IWRAM_FUNC ADPCMDecodeChannel_8bit_resample(const uint8_t *&data8, uint8_t *dst8, const uint32_t nrOfNibbles, LinearResamplerChannelData &resamplerData) -> uint32_t
    {
        // first sample is stored verbatim in header
        int32_t pcmData = static_cast<int16_t>(data8[0] | (data8[1] << 8));
        int32_t index = static_cast<int16_t>(data8[2] | (data8[3] << 8));
        data8 += 4;
#ifdef ADPCM_DITHER
        pcmData += (ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT) - ADPCM_DitherState[0];
        ADPCM_DitherState[0] = ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT;
        ADPCM_DitherState[1] = ((ADPCM_DitherState[1] << 4) - ADPCM_DitherState[1]) ^ 1;
        pcmData = pcmData < -32768 ? -32768 : pcmData;
        pcmData = pcmData > 32767 ? 32767 : pcmData;
#endif
        // we can have history from previous calls, but at most one sample,
        // because we must have output data until resamplerData.position >= RESAMPLER_POSITION_ONE
        // we can also have no history at all for a first call
        // this means we need at least one new sample here
        if (resamplerData.position >= RESAMPLER_POSITION_TWO)
        {
            // no samples for a first call
            resamplerData.pcmHistory[0] = pcmData;
            resamplerData.pcmHistory[1] = pcmData;
            resamplerData.pcmRaw = pcmData;
            resamplerData.position -= RESAMPLER_POSITION_ONE;
        }
        else if (resamplerData.position >= RESAMPLER_POSITION_ONE)
        {
            // one sample from previous call
            // move new PCM data to resampler history
            resamplerData.pcmHistory[0] = resamplerData.pcmHistory[1];
#ifdef ADPCM_APPLY_HIGH_SHELF
            // apply high-shelf filter
            auto pcmFiltered = resamplerData.pcmRaw + (resamplerData.pcmRaw >> 2) - (pcmData >> 2);
            resamplerData.pcmHistory[1] = pcmFiltered;
            resamplerData.pcmRaw = pcmData;
#else
            resamplerData.pcmHistory[1] = pcmData;
#endif
            resamplerData.position -= RESAMPLER_POSITION_ONE;
        }
        // else
        // {
        //      // technically an error. should never happen
        //      return;
        // }
        // start output loop
        uint32_t nibblesDecoded = 0;
        uint32_t nibbles = 0;
        uint32_t dstSamplesGenerated = 0;
        while (nibblesDecoded < nrOfNibbles)
        {
            // check if we need more input samples
            if (resamplerData.position >= RESAMPLER_POSITION_ONE)
            {
                // load two ADPCM nibbles every 2 ADPCM samples
                if ((nibblesDecoded & 1) == 0)
                {
                    nibbles = *data8++;
                }
                // decode nibble
                uint32_t delta = ADPCM_DeltaTable_4bit[index][nibbles & 0x07];
                pcmData += (nibbles & 8) ? -delta : delta;
                index += ADPCM_IndexTable_4bit[nibbles & 0x07];
                index = index < 0 ? 0 : index;
                index = index > 88 ? 88 : index;
#ifdef ADPCM_DITHER
                pcmData += (ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT) - ADPCM_DitherState[0];
                ADPCM_DitherState[0] = ADPCM_DitherState[1] >> ADPCM_DITHER_SHIFT;
                ADPCM_DitherState[1] = ((ADPCM_DitherState[1] << 4) - ADPCM_DitherState[1]) ^ 1;
#endif
                // clamp sample value
                pcmData = pcmData < -32768 ? -32768 : pcmData;
                pcmData = pcmData > 32767 ? 32767 : pcmData;
                // move new PCM data to resampler history
                resamplerData.pcmHistory[0] = resamplerData.pcmHistory[1];
#ifdef ADPCM_APPLY_HIGH_SHELF
                // apply high-shelf filter
                auto pcmFiltered = resamplerData.pcmRaw + (resamplerData.pcmRaw >> 2) - (pcmData >> 2);
                resamplerData.pcmHistory[1] = pcmFiltered;
                resamplerData.pcmRaw = pcmData;
#else
                resamplerData.pcmHistory[1] = pcmData;
#endif
                resamplerData.position -= RESAMPLER_POSITION_ONE;
                // move next nibble into position
                nibbles >>= 4;
                nibblesDecoded++;
            }
            // check if we can produce new output samples
            while (resamplerData.position < RESAMPLER_POSITION_ONE)
            {
                // linear interpolation of samples
                int32_t diff = static_cast<int32_t>(resamplerData.pcmHistory[1]) - static_cast<int32_t>(resamplerData.pcmHistory[0]);
                int32_t sample = static_cast<int32_t>(resamplerData.pcmHistory[0]) + (diff * (resamplerData.position >> (RESAMPLER_PRECISION - 8)) >> 8);
                resamplerData.position += resamplerData.step;
                // write to output buffer
#ifdef ADPCM_ROUNDING
                *dst8++ = (sample + 128) >> 8;
#else
                *dst8++ = sample >> 8;
#endif
                dstSamplesGenerated++;
            }
        }
        return dstSamplesGenerated;
    }

    auto IWRAM_FUNC ADPCMUnCompWrite8bit_8bit(const void *data, [[maybe_unused]] uint32_t dataSize, uint8_t *dst[2], LinearResamplerData *resamplerData) -> uint32_t
    {
        // copy frame header and skip to data
        const Audio::AdpcmFrameHeader frameHeader = Audio::AdpcmFrameHeader::read(data);
        auto data8 = reinterpret_cast<const uint8_t *>(data) + sizeof(Audio::AdpcmFrameHeader);
        // uncompress 4-bit ADPCM samples. These are stored planar / per channel, e.g. L0 L1 ... R0 R1 ...
        const auto adpcmChannelNrOfSamples = (frameHeader.uncompressedSize / sizeof(int16_t)) / static_cast<uint8_t>(frameHeader.nrOfChannels);
        // we decode one less nibble than samples, as the first sample is verbatim
        const auto adpcmChannelNrOfNibbles = adpcmChannelNrOfSamples - 1;
        uint32_t dstSamplesGenerated = 0;
        for (uint32_t channel = 0; channel < frameHeader.nrOfChannels; ++channel)
        {
            // check if we want to decode with resampling or not
            if (resamplerData)
            {
                dstSamplesGenerated += ADPCMDecodeChannel_8bit_resample(data8, dst[channel], adpcmChannelNrOfNibbles, (*resamplerData)[channel]);
            }
            else
            {
                dstSamplesGenerated += ADPCMDecodeChannel_8bit(data8, dst[channel], adpcmChannelNrOfNibbles);
            }
        }
        return dstSamplesGenerated;
    }

    uint32_t IWRAM_FUNC ADPCMUnCompGetSize_8bit(const void *data)
    {
        const Audio::AdpcmFrameHeader frameHeader = Audio::AdpcmFrameHeader::read(data);
        // if we're down-converting the PCM sample depth during decompression, adjust the uncompressed data size too
        return (static_cast<uint32_t>(frameHeader.uncompressedSize) * 8 + 7) / frameHeader.pcmBitsPerSample; // + frameHeader.nrOfChannels;
    }
}
