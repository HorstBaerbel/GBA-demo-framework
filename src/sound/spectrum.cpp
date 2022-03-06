#include "spectrum.h"

#include "math/fp32.h"
#include "memory/memory.h"

//#define CHECK_CPU_TIME
#ifdef CHECK_CPU_TIME
#include "fptime.h"

#endif

#include "print/print.h"

// Define this to build a FFT DIT swizzle table and output it to console
//#define BUILD_SWIZZLE_TABLE

// Define this to use a precalculated FFT DIT swizzle table
//#define USE_SWIZZLE_TABLE

// Define this to apply a von Hann window function to the sample data
#define ENABLE_WINDOWING

namespace Spectrum
{

    constexpr int32_t NrOfSamples = 256;                       // # of samples to analyze per FFT (stereo sample count)
    constexpr int32_t DownSampleFactor = 2;                    // Downsample factor for audio
    constexpr int32_t SampleRateHz = 31536 / DownSampleFactor; // Sample rate / playback rate of audio data in buffer

    IWRAM_DATA ALIGN(4) static int8_t fftdata[NrOfSamples]; // Real/imaginary FFT values for input/output

    // Split FFT results into spectrum / frequency bins logarithmicaly
    // See: https://dsp.stackexchange.com/questions/49436/scale-fft-frequency-range-for-a-bars-graph
    // And: https://dsp.stackexchange.com/questions/40766/calculating-values-of-frequency-bins-in-python
    // The FFT produces N/2 values for 0 to SampleRate/2
    // -> Bin number k to bin frequency:
    // f = k / NrOfSamples/2 * SampleRateHz/2
    // -> Bin frequency to bin index k
    // k = f * NrOfSamples/2 / SampleRateHz/2
    // Bin #0 is crap / DC offset, so we don't use it.
    // Then we split the remaining bins logarithmicaly, that is, the number of bins the bars use increases logarithmicaly.
    // This is useful due to tonal frequencies doubling each octave.
    // static constexpr int32_t BinMinHz = SampleRateHz / NrOfSamples; // -> ~123 Hz for 256 samples, 32kHz sample rate
    // static constexpr int32_t BinMaxHz = 16000.0F;
    // static constexpr float BinLogStart = 0;                                       // 2^0 = 1
    // static const float BinLogEnd = log2((BinMaxHz * NrOfSamples) / SampleRateHz); // 2^7.02 = ~130) for 256 samples, 32kHz sample rate
    // static const float BinLogRange = BinLogEnd - BinLogStart;

    struct BandInfo
    {
        uint16_t start;       // first FFT bin for bar
        uint16_t end;         // last FFT bin for bar
        int32_t oneOverCount; // (1 << 16) / bin count
    } ALIGN_PACK(4);

    // Real/imaginary FFT values for input/output
    // Bin indices can be calculated using:
    // start = 2 ^ (BinLogStart + BinLogRange * i / NrOfBands)
    // end = 2 ^ (BinLogStart + BinLogRange * (i + 1) / NrOfBands)
    IWRAM_DATA ALIGN(4) static const BandInfo fftbands[NrOfBands] = {
        {1, 2, 65536 / 2},
        {3, 6, 65536 / 4},
        {7, 14, 65536 / 8},
        {15, 30, 65536 / 16},
        {31, 62, 65536 / 32},
        {63, 127, 65536 / 64}};

    IWRAM_DATA ALIGN(4) static Bands spectrum; // Current spectrum result

    static constexpr int32_t FFTSineFull = 256;                 // full length of sine wave
    static constexpr int32_t FFTSineTableMod = FFTSineFull - 1; // modulo value for accessing sine table
    static constexpr int32_t FFTSineTableLog2 = 8;              // log2(FFTSineFull)

    // Pseudo-sine/cosine function for 2*pi == FFTSineFull.
    // Shifting by pi/2 (FFTSineFull / 4), gives sine.
    // The first FFTSineFull / 4 values are appended again to save on access operations
    IWRAM_DATA ALIGN(4) static const int8_t FFTSineTable[FFTSineFull] = {
        0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31,
        34, 37, 40, 43, 46, 49, 51, 54, 57, 60,
        63, 65, 68, 71, 73, 76, 78, 81, 83, 85,
        88, 90, 92, 94, 96, 98, 100, 102, 104, 106,
        107, 109, 111, 112, 113, 115, 116, 117, 118, 120,
        121, 122, 122, 123, 124, 125, 125, 126, 126, 126,
        127, 127, 127, 127, 127, 127, 127, 126, 126, 126,
        125, 125, 124, 123, 122, 122, 121, 120, 118, 117,
        116, 115, 113, 112, 111, 109, 107, 106, 104, 102,
        100, 98, 96, 94, 92, 90, 88, 85, 83, 81,
        78, 76, 73, 71, 68, 65, 63, 60, 57, 54,
        51, 49, 46, 43, 40, 37, 34, 31, 28, 25,
        22, 19, 16, 12, 9, 6, 3, 0, -3, -6,
        -9, -12, -16, -19, -22, -25, -28, -31, -34, -37,
        -40, -43, -46, -49, -51, -54, -57, -60, -63, -65,
        -68, -71, -73, -76, -78, -81, -83, -85, -88, -90,
        -92, -94, -96, -98, -100, -102, -104, -106, -107, -109,
        -111, -112, -113, -115, -116, -117, -118, -120, -121, -122,
        -122, -123, -124, -125, -125, -126, -126, -126, -127, -127,
        -127, -127, -127, -127, -127, -126, -126, -126, -125, -125,
        -124, -123, -122, -122, -121, -120, -118, -117, -116, -115,
        -113, -112, -111, -109, -107, -106, -104, -102, -100, -98,
        -96, -94, -92, -90, -88, -85, -83, -81, -78, -76,
        -73, -71, -68, -65, -63, -60, -57, -54, -51, -49,
        -46, -43, -40, -37, -34, -31, -28, -25, -22, -19,
        -16, -12, -9, -6, -3};

    // Precalculated von Hanning window function values for 64 samples
    IWRAM_DATA ALIGN(4) static const uint8_t HannWindow[NrOfSamples]{
        0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4,
        5, 6, 6, 8, 9, 10, 11, 12, 14, 15,
        17, 18, 20, 22, 23, 25, 27, 29, 31, 33,
        35, 38, 40, 42, 45, 47, 49, 52, 54, 57,
        60, 62, 65, 68, 71, 73, 76, 79, 82, 85,
        88, 91, 94, 97, 100, 103, 106, 109, 113, 116,
        119, 122, 125, 128, 131, 135, 138, 141, 144, 147,
        150, 153, 156, 159, 162, 165, 168, 171, 174, 177,
        180, 183, 186, 189, 191, 194, 197, 199, 202, 204,
        207, 209, 212, 214, 216, 218, 221, 223, 225, 227,
        229, 231, 232, 234, 236, 238, 239, 241, 242, 243,
        245, 246, 247, 248, 249, 250, 251, 252, 252, 253,
        253, 254, 254, 255, 255, 255, 255, 255, 255, 255,
        255, 254, 254, 253, 253, 252, 252, 251, 250, 249,
        248, 247, 246, 245, 243, 242, 241, 239, 238, 236,
        234, 232, 231, 229, 227, 225, 223, 221, 218, 216,
        214, 212, 209, 207, 204, 202, 199, 197, 194, 191,
        189, 186, 183, 180, 177, 174, 171, 168, 165, 162,
        159, 156, 153, 150, 147, 144, 141, 138, 135, 131,
        128, 125, 122, 119, 116, 113, 109, 106, 103, 100,
        97, 94, 91, 88, 85, 82, 79, 76, 73, 71,
        68, 65, 62, 60, 57, 54, 52, 49, 47, 45,
        42, 40, 38, 35, 33, 31, 29, 27, 25, 23,
        22, 20, 18, 17, 15, 14, 12, 11, 10, 9,
        8, 6, 6, 5, 4, 3, 2, 2, 1, 1,
        1, 0, 0, 0, 0};

#ifdef USE_SWIZZLE_TABLE
    IWRAM_DATA ALIGN(4) static const uint8_t SwizzleTable[NrOfSamples / 2] = {
        0, 64, 32, 96, 16, 80, 48, 112, 8, 72, 40, 104, 24, 88, 56, 120, 4, 68, 36, 100, 20, 84, 52, 116, 12, 76, 44, 108, 28, 92, 60, 124, 2, 66, 34, 98, 18, 82, 50, 114, 10, 74, 42, 106, 26, 90, 58, 122, 6, 70, 38, 102, 22, 86, 54, 118, 14, 78, 46, 110, 30, 94, 62, 126, 1, 65, 33, 97, 17, 81, 49, 113, 9, 73, 41, 105, 25, 89, 57, 121, 5, 69, 37, 101, 21, 85, 53, 117, 13, 77, 45, 109, 29, 93, 61, 125, 3, 67, 35, 99, 19, 83, 51, 115, 11, 75, 43, 107, 27, 91, 59, 123, 7, 71, 39, 103, 23, 87, 55, 119, 15, 79, 47, 111, 31, 95, 63, 127};
#endif

    // Fixed-point multiplication & scaling. Scaling ensures that result remains 16-bit
    inline int8_t FixedMul(int8_t a, int8_t b)
    {

        // Multiply, then shift right one less bit (i.e. 7-1)
        int16_t c = (static_cast<int16_t>(a) * static_cast<int16_t>(b)) >> 6;
        // last bit shifted out = rounding-bit
        b = c & 0x01;
        // last shift + rounding bit
        return (c >> 1) + b;
    }

    // Apply amplitude correction factor to compensate for windowing
    // See: https://community.sw.siemens.com/s/article/window-correction-factors
    constexpr int16_t FFTWindowAmplitudeCorrectionFactor = 2;
    //constexpr int16_t FFTWindowEnergyCorrectionFactor = 1.63;

    // Perform windowing on sampled data to eliminate noise in FFT bands
    // Von Hann (raised cosine) function with simple 16-bit arithmetic to compensate for rounding errors
    // out(n) = in(n) * (0.5 - 0.5 * (cos(2 * PI * i / (NrOfSamples - 1))))
    // Window multiplies are precalculated for speed
    IWRAM_FUNC void FFTWindow(int8_t *even, int8_t *odd)
    {
        for (int32_t i = 0; i < NrOfSamples / 2; i++)
        {
            int32_t hi = i << 1;
            // multiply by von Hann window value and shift back one bit less (8-1)
            int32_t ce = (static_cast<int16_t>(even[i]) * static_cast<int16_t>(HannWindow[hi])) >> 7;
            int32_t co = (static_cast<int16_t>(odd[i]) * static_cast<int16_t>(HannWindow[hi + 1])) >> 7;
            // shift back last bit, round and write back to array
            even[i] = (ce >> 1) + (ce & 0x01);
            odd[i] = (co >> 1) + (co & 0x01);
        }
    }

    // Perform fixed-point forward fast Fourier transform.
    // real[n], imag[n] are real and imaginary arrays, both INPUT AND RESULT (in-place FFT)
    // FFT size used is NrOfSamples as defined in header
    IWRAM_FUNC void FFT(int8_t *real8, int8_t *imag8)
    {
        constexpr int32_t n = NrOfSamples / 2;
#ifdef BUILD_SWIZZLE_TABLE
        static uint8_t table[NrOfSamples / 2] = {0};
        for (int i = 0; i < NrOfSamples / 2; i++)
        {
            table[i] = i;
        }
#endif
#ifndef USE_SWIZZLE_TABLE
        constexpr int32_t nn = n - 1;
        // decimation in time - re-order data
        int16_t mr = 0;
        for (int16_t m = 1; m <= nn; ++m)
        {
            int16_t l = n;
            do
            {
                l >>= 1;
            } while (mr + l > nn);
            mr = (mr & (l - 1)) + l;
            if (mr <= m)
                continue;
            auto temp = real8[m];
            real8[m] = real8[mr];
            real8[mr] = temp;
            temp = imag8[m];
            imag8[m] = imag8[mr];
            imag8[mr] = temp;
#ifdef BUILD_SWIZZLE_TABLE
            table[mr] = m;
            table[m] = mr;
        }
        for (int i = 0; i < NrOfSamples / 2; i++)
        {
            printf("%d,", table[i]);
        }
#else
        }
#endif
#endif
        int32_t k = FFTSineTableLog2 - 1;
        int32_t l = 1;
        while (l < n)
        {
            // Fixed scaling, for proper normalization. There will be log2(n) passes, so this results in an overall factor of 1/n,
            // distributed to maximize arithmetic accuracy. It may not be obvious, but the shift will be performed
            // on each data point exactly once, during this pass.
            const int32_t iStep = l << 1;
            for (int32_t m = 0; m < l; ++m)
            {
                int32_t j = m << k;
                // calculate sine / cosine index: 0 <= j < FFTSineFull / 2
                int8_t wr = FFTSineTable[j & FFTSineTableMod] >> 1;
                int8_t wi = (-FFTSineTable[(j + FFTSineFull / 4) & FFTSineTableMod]) >> 1;
                for (int32_t i = m; i < n; i += iStep)
                {
                    j = i + l;
                    int32_t tr = FixedMul(wr, real8[j]) - FixedMul(wi, imag8[j]);
                    int32_t ti = FixedMul(wr, imag8[j]) + FixedMul(wi, real8[j]);
                    int32_t qr = real8[i] >> 1;
                    int32_t qi = imag8[i] >> 1;
                    real8[j] = qr - tr;
                    imag8[j] = qi - ti;
                    real8[i] = qr + tr;
                    imag8[i] = qi + ti;
                }
            }
            --k;
            l = iStep;
        }
    }

    // Average stereo samples to mono, downsample 2-to-1 and distribute even/odd samples into real/imaginary arrays respectively
    // This reads NrOfSamples * 2 * 2 samples (stereo, downsampled) from samples and writes NrOfSamples / 2 samples to real8 and imag8 each
    // See: https://www.musicdsp.org/en/latest/Filters/214-fast-downsampling-with-antialiasing.html
    IWRAM_FUNC void AverageDownsample2WindowSwizzleShuffle(const int8_t *left, const int8_t *right, int8_t *real8, int8_t *imag8)
    {
        auto left8 = reinterpret_cast<const int8_t *>(left);
        auto right8 = reinterpret_cast<const int8_t *>(right);
        int32_t filterStateEven = 0; // 0.25 * previous input sample (0 is a good starting value for non-periodic waveforms)
        int32_t filterStateOdd = 0;  // 0.25 * previous input sample (0 is a good starting value for non-periodic waveforms)
        for (uint32_t i = 0; i < NrOfSamples / 2; i++)
        {
            // Average stereo samples and filter with triangle filter [0.25, 0.5, 0.25]
            int32_t even = filterStateEven + ((static_cast<int32_t>(*left8++) + static_cast<int32_t>(*right8++)) >> 2);
            int32_t odd = filterStateOdd + ((static_cast<int32_t>(*left8++) + static_cast<int32_t>(*right8++)) >> 2);
            filterStateEven = (static_cast<int32_t>(*left8++) + static_cast<int32_t>(*right8++)) >> 3;
            filterStateOdd = (static_cast<int32_t>(*left8++) + static_cast<int32_t>(*right8++)) >> 3;
            even += filterStateEven;
            odd += filterStateOdd;
#ifdef ENABLE_WINDOWING
            // apply windowing
            int32_t hi = i << 1;
            // multiply by von Hann window value and shift back one bit less (8-1)
            int32_t ce = (static_cast<int16_t>(even) * static_cast<int16_t>(HannWindow[hi])) >> 7;
            int32_t co = (static_cast<int16_t>(odd) * static_cast<int16_t>(HannWindow[hi + 1])) >> 7;
            // shift back last bit, round and write back to array
            even = (ce >> 1) + (ce & 0x01);
            odd = (co >> 1) + (co & 0x01);
#endif
            // Reorder samples for FFT. Even samples go to first half of the array, odd ones to second half
#ifdef USE_SWIZZLE_TABLE
            // Also swizzle indices for DIT FFT while we're at it
            int32_t swizzleIndex = SwizzleTable[i];
            real8[swizzleIndex] = static_cast<int8_t>(even);
            imag8[swizzleIndex] = static_cast<int8_t>(odd);
#else
            real8[i] = static_cast<int8_t>(even);
            imag8[i] = static_cast<int8_t>(odd);
#endif
        }
    }

    // Average stereo samples to mono and distributing even/odd samples into real/imaginary arrays respectively
    // This reads NrOfSamples * 2 samples (stereo) from samples and writes NrOfSamples / 2 samples to real8 and imag8 each
    IWRAM_FUNC void AverageShuffle(const int8_t *left, const int8_t *right, int8_t *real8, int8_t *imag8)
    {
        auto left8 = reinterpret_cast<const int8_t *>(left);
        auto right8 = reinterpret_cast<const int8_t *>(right);
        for (uint32_t i = 0; i < NrOfSamples / 2; i++)
        {
            // Average stereo samples and reorder samples for FFT
            // Even samples go to first half of the array, odd ones to second half
            int32_t even = (static_cast<int32_t>(*left8++) + static_cast<int32_t>(*right8++)) >> 1;
            real8[i] = static_cast<int8_t>(even);
            int32_t odd = (static_cast<int32_t>(*left8++) + static_cast<int32_t>(*right8++)) >> 1;
            imag8[i] = static_cast<int8_t>(odd);
        }
    }

    uint32_t sqrt16(uint16_t x)
    {
        uint32_t root = 0;
        uint32_t place = 0x00004000;
        while (place > x)
        {
            place = place >> 2;
        }
        while (place)
        {
            if (x >= root + place)
            {
                x = x - root - place;
                root = root + (place << 1);
            }
            root = root >> 1;
            place = place >> 2;
        }
        return root;
    }

    // Calculate spectrum bands from FFT result
    // See: https://dsp.stackexchange.com/questions/52958/how-can-i-get-the-power-of-a-specific-frequency-band-after-fft
    IWRAM_FUNC void SpectrumFromFFT(int8_t *real8, int8_t *imag8)
    {
        //static Math::fp1616_t maxValue = 0;
        for (uint32_t i = 0; i < NrOfBands; i++)
        {
            auto const &band = fftbands[i]; // the band we've currently processing
            int32_t tempLevel = 0;          // accumulated level
            for (unsigned int vi = band.start; vi <= band.end; vi++)
            {
                // convert complex values to squared magnitude (max. 127*127+127*127 = 32258), actually a bit less due to windowing
#ifdef ENABLE_WINDOWING
                int16_t real = static_cast<int16_t>(real8[vi]) * FFTWindowAmplitudeCorrectionFactor;
                int16_t imag = static_cast<int16_t>(imag8[vi]) * FFTWindowAmplitudeCorrectionFactor;
#else
                int16_t real = static_cast<int16_t>(real8[vi]);
                int16_t imag = static_cast<int16_t>(imag8[vi]);
#endif
                auto magnitudeSqr = static_cast<uint16_t>(static_cast<uint16_t>(real * real) + static_cast<uint16_t>(imag * imag)) >> 1;
                if (magnitudeSqr > 32)
                {
                    int32_t value = sqrt16(magnitudeSqr);
                    tempLevel += value;
                }
            }
            // normalize level by bins used
            tempLevel *= band.oneOverCount;
            // apply cheap log2 to spectrum value
            tempLevel = int32_t(Math::log2_B(Math::fp1616_t::fromRaw(tempLevel * 4096)));
            // calculate new spectrum value by averaging old and new values
            //tempLevel = (tempLevel + spectrum[i]) >> 1;
            spectrum[i] = static_cast<uint8_t>(tempLevel);
        }
    }

    // Forward FFT on array of real numbers. FFT using half-size complex FFT by distributing even/odd samples into real/imaginary arrays respectively
    IWRAM_FUNC void update(const int8_t *left, const int8_t *right, uint32_t size)
    {
#ifdef CHECK_CPU_TIME
        Time::startTimer();
#endif
        // clear old spectrum
        for (uint32_t i = 0; i < NrOfBands; i++)
        {
            spectrum[i] = 0;
        }
        // set up real/imaginary data
        auto real = reinterpret_cast<int8_t *>(&fftdata);
        auto imag = reinterpret_cast<int8_t *>(&fftdata[NrOfSamples / 2]);
        // do multiple FFTs for audio buffer
        for (int offset = 0; (offset + DownSampleFactor * NrOfSamples) < static_cast<int32_t>(size); offset += DownSampleFactor * NrOfSamples)
        {
            // prepare sample data for FFT
            if constexpr (DownSampleFactor == 2)
            {
                AverageDownsample2WindowSwizzleShuffle(left + offset, right + offset, real, imag);
            }
            else
            {
                AverageShuffle(left + offset, right + offset, real, imag);
#ifdef ENABLE_WINDOWING
                // Apply windowing
                FFTWindow(real, imag);
#endif
            }
            // calculate FFT
            FFT(real, imag);
            // build spectrum from FFT result (averages multiple spectrums)
            SpectrumFromFFT(real, imag);
        }
#ifdef CHECK_CPU_TIME
        printf("Spectrum: %f ms", Time::endTimer() * 1000);
#endif
    }

    const Bands &getSpectrum()
    {
        return spectrum;
    }
}
