#pragma once

//DON'T include these directly!!! No namespace here, because directly included in fp32.h

extern const int16_t tan_pi_half_tab[66];

template <unsigned int F>
fp32_t<F> sin(fp32_t<F> x)
{
    // From: http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648/84
    // Should be faster than Taylor or Chebishev approximation (See: https://stackoverflow.com/questions/345085/how-do-trigonometric-functions-work/394512#394512)
    //static const fp32_t<F> p = fp32_t<F>(0.225f);
    static const fp32_t<F> A = fp32_t<F>(7.589466384f); // 16 * sqrt(p)
    static const fp32_t<F> B = fp32_t<F>(1.633843458f); // (1 - p) / sqrt(p);
                                                        // wrap x within [0, 2*PI) first and bring in range [-0.5, 0.5]
    fp32_t<F> y = x * fp32_t<F>::ONE_OVER_PI_2;
    y = y - floor(y + fp32_t<F>::HALF);
    y = A * y * (fp32_t<F>::HALF - abs(y));
    return y * (B + abs(y));
    /* // other version. less exact, not faster
    static const fp32_t<F> P = fp32_t<F>(3.1f);
    static const fp32_t<F> Q = fp32_t<F>(3.6f);
    // Convert the input value to a range of -1 to 1 and wrap around
    fp32_t<F> y = frac(x * fp32_t<F>::ONE_OVER_PI);
    #if LOW_SINE_PRECISION
        return (y - y * abs(y)) << 2;
    #else
        y = y - y * abs(y);
        return y * (Q + P * abs(y));
    #endif*/
}

template <unsigned int F>
fp32_t<F> cos(fp32_t<F> x)
{
    return sin(x + fp32_t<F>::PI_HALF);
}

template <unsigned int F>
void sincos(fp32_t<F> x, fp32_t<F> &sinx, fp32_t<F> &cosx)
{
    // from here: https://dspguru.com/dsp/tricks/parabolic-approximation-of-sin-and-cos/
    //static const fp32_t<F> TWO_OVER_PI = fp32_t<F>(2.0f / 3.1415926535f);
    static const fp32_t<F> C = fp32_t<F>(0.71256755058f);
    static const fp32_t<F> B = fp32_t<F>(2.0f - 4.0f * 0.71256755058f);
    static const fp32_t<F> BM = fp32_t<F>(4.0f * 0.71256755058f - 2.0f);
    static const int32_t QUARTER_MASK = 3 << (fp32_t<F>::BITSF - 2);
    // wrap x within [0, 2*PI) first and bring in range [0, 1)
    fp32_t<F> y = x * fp32_t<F>::ONE_OVER_PI_2;
    // y = y < 0 ? y + 1 : y; //TODO: Needed?
    y = y - floor(y);
    // identify which quarter to use [0, 3], because the function is accurate only in [0, PI/2]
    //int32_t quarter = (y.raw() - 1) >> (fp32_t<F>::BITSF - 2);
    int32_t quarter = y.raw() & QUARTER_MASK; // samesame, but faster ;)
    // wrap x within [0, PI/2) and bring in range [0, 1)
    //y = x * TWO_OVER_PI;
    //y = y - floor(y);
    y = fp32_t<F>::fromRaw((y.raw() - quarter) << 2); // samesame, but faster ;)
    if (quarter == 0)
    {
        y = y - fp32_t<F>::HALF; // bring into range [-0.5, 0.5]
        fp32_t<F> temp = B * y * y + C;
        sinx = temp + y;
        cosx = temp - y;
    }
    else if (quarter == (1 << (fp32_t<F>::BITSF - 2)))
    {
        y = fp32_t<F>::HALF - y;
        fp32_t<F> temp = B * y * y + C;
        sinx = y + temp;
        cosx = y - temp;
    }
    else if (quarter == (2 << (fp32_t<F>::BITSF - 2)))
    {
        y = y - fp32_t<F>::HALF;
        fp32_t<F> temp = BM * y * y - C;
        sinx = temp - y;
        cosx = temp + y;
    }
    else
    {
        y = y - fp32_t<F>::HALF;
        fp32_t<F> temp = B * y * y + C;
        sinx = y - temp;
        cosx = y + temp;
    }
}

template <unsigned int F>
fp32_t<F> tan(fp32_t<F> x)
{
    // Variant the needs a division:
    // Pade approximant: (15x-x^3)/(15-6x^2)
    // see: https://link.springer.com/article/10.1186/s13660-016-1044-x
    // wrap x within [-PI, PI] first
    //fp32_t<F> y = x * fp32_t<F>::ONE_OVER_PI;
    //y = frac(y) * fp32_t<F>::PI;
    //fp32_t<F> yy = y*y;
    //return (15 - yy) * y / (15 - 6 * yy);
    // or (-2.5*x + 0.125 * x^3)/(-2.5 + x^2)

    fp32_t<F> s;
    fp32_t<F> c;
    sincos(x, s, c);
    return s / c;

    /*static const fp32_t<F> FOUR_OVER_PI = fp32_t<F>(4.0f / 3.1415926535f);
    static const int32_t HALF_MASK = 1 << (fp32_t<F>::BITSF - 1);
    // wrap x within [0, PI) first and bring in range [0, 1)
    fp32_t<F> y = x * fp32_t<F>::ONE_OVER_PI;
    y = y - floor(y);
    y = y < 0 ? ((1 << F) - y) : y;
    // identify which half to use [0, 1], because the table covers only [0, PI/2]
    //int32_t quarter = (y.raw() - 1) >> (fp32_t<F>::BITSF - 2);
    int32_t half = y.raw() & HALF_MASK; // samesame, but faster ;)
    // remove half from y (y then in range [0, 0.5). Then bring into range [0, 63] for table with .8 precision.
    int32_t t = (y.raw() & (~HALF_MASK)) >> (F - 15);
    // get index (top 6 bits)
    int32_t index = (t >> 8) + 1;
    // get fraction (lower 8 bits)
    t &= 0xFF;
    int32_t a;
    int32_t b;
    if (half)
    {
        a = -tan_pi_half_tab[64-index];
        b = -tan_pi_half_tab[63-index];
    }
    else
    {
        a = tan_pi_half_tab[index];
        b = tan_pi_half_tab[index+1];
    }
	return fp32_t<F>::fromRaw((a + (t * (b - a))) << (F - 8));*/
}

template <unsigned int F>
fp32_t<F> atan(fp32_t<F> x)
{
    // from here: http://nghiaho.com/?p=997
    // which cites: “Efficient approximations for the arctangent function”, Rajan, S. Sichun Wang Inkol, R. Joyal, A., May 2006
    static const fp32_t<F> A = fp32_t<F>(0.2447f);
    static const fp32_t<F> B = fp32_t<F>(0.0663f);
    // wrap x within [-1, 1]
    fp32_t<F> y = frac(x);
    return fp32_t<F>::PI_QUARTER * y - y * (abs(y) - 1) * (A + B * abs(y));
}

template <unsigned int F>
fp32_t<F> atan2(fp32_t<F> x, fp32_t<F> y)
{
    // from here: https://math.stackexchange.com/questions/1098487/atan2-faster-approximation#1105038
    static const fp32_t<F> A = fp32_t<F>(-0.0464964749f);
    static const fp32_t<F> B = fp32_t<F>(0.15931422f);
    static const fp32_t<F> C = fp32_t<F>(0.327622764f);
    fp32_t<F> xAbs = abs(x);
    fp32_t<F> yAbs = abs(y);
    fp32_t<F> a = min(xAbs, yAbs) / max(xAbs, yAbs);
    fp32_t<F> s = a * a;
    fp32_t<F> r = ((A * s + B) * s - C) * s * a + a;
    if (yAbs > xAbs)
    {
        r = fp32_t<F>::PI_HALF - r;
    }
    if (x < 0)
    {
        r = fp32_t<F>::PI - r;
    }
    if (y < 0)
    {
        r = -r;
    }
    return r;
}

// This implementation is based on Clay. S. Turner's fast binary logarithm algorithm
// C. S. Turner, "A Fast Binary Logarithm Algorithm", IEEE Signal Processing Mag., pp. 124,140, Sep. 2010
// See: https://github.com/dmoulding/log2fix
template <unsigned int F>
fp32_t<F> log2_A(fp32_t<F> a)
{
    int32_t x = a.raw();
    int32_t b = 1U << (F - 1);
    int32_t y = 0;
    if (x <= 0)
    {
        return 0;
    }
    while (x < 1U << F)
    {
        x <<= 1;
        y -= 1U << F;
    }
    while (x >= 2U << F)
    {
        x >>= 1;
        y += 1U << F;
    }
    uint32_t z = x;
    for (uint32_t i = 0; i < F; i++)
    {
        z = ((uint64_t)z * (uint64_t)z) >> F;
        if (z >= 2U << F)
        {
            z >>= 1;
            y += b;
        }
        b >>= 1;
    }
    return fp32_t<F>::fromRaw(y);
}

/* on 32-bit architectures, there is often an instruction/intrinsic for this */
inline int32_t mulhi(int32_t a, int32_t b)
{
    return (int32_t)(((int64_t)a * (int64_t)b) >> 32);
}

#define RND_SHIFT (25 - 16)
#define RND_CONST ((1 << RND_SHIFT) / 2)
#define RND_ADJUST (-2) /* established heuristically */

/* 
    compute log2(x) in s15.16 format, where x is in s15.16 format
    maximum absolute error 1.11288e-5 @ 0x5a82689f (0.707104757)
*/
template <unsigned int F>
fp32_t<F> log2_B(fp32_t<F> a)
{
    if (a <= 0)
    {
        return 0;
    }
    /* x = 2**i * (1 + f), 0 <= f < 1. Find i */
    int32_t x = a.raw();
    int32_t lz = 32;
    if (x & 0xffff0000)
    {
        lz -= 16;
        x >>= 16;
    }
    if (x & 0xff00)
    {
        lz -= 8;
        x >>= 8;
    }
    if (x & 0xf0)
    {
        lz -= 4;
        x >>= 4;
    }
    if (x & 0xc)
    {
        lz -= 2;
        x >>= 2;
    }
    lz -= (x & 0x2);
    lz -= (x & 0x1);
    int32_t i = (32 - F) - lz;
    /* force (1+f) into range [sqrt(0.5), sqrt(2)] */
    uint32_t t = (uint32_t)a.raw() << lz;
    if (t > (uint32_t)(1.414213562 * (1U << 31)))
    {
        i++;
        t = t >> 1;
    }
    /* compute log2(1+f) for f in [-0.2929, 0.4142] */
    int32_t f = t - (1U << 31);
    int32_t p = +(int32_t)(-0.206191055 * (1U << 31) - 1);
    p = mulhi(p, f) + (int32_t)(0.318199910 * (1U << 30) - 18);
    p = mulhi(p, f) + (int32_t)(-0.366491705 * (1U << 29) + 22);
    p = mulhi(p, f) + (int32_t)(0.479811855 * (1U << 28) - 2);
    p = mulhi(p, f) + (int32_t)(-0.721206390 * (1U << 27) + 37);
    p = mulhi(p, f) + (int32_t)(0.442701618 * (1U << 26) + 35);
    p = mulhi(p, f) + (f >> (31 - 25));
    /* round fractional part of the result */
    int32_t approx = (p + RND_CONST + RND_ADJUST) >> RND_SHIFT;
    /* combine integer and fractional parts of result */
    return fp32_t<F>::fromRaw((i << F) + approx);
}

template <unsigned int F>
fp32_t<F> log10(fp32_t<F> x)
{
    constexpr fp32_t<F> ONE_OVER_LOG_2_10 = fp32_t<F>::fromRaw(19728);
    return log2(x) * ONE_OVER_LOG_2_10;
}

template <unsigned int F>
fp32_t<F> ln(fp32_t<F> x)
{
    constexpr fp32_t<F> ONE_OVER_LOG_2_E = fp32_t<F>::fromRaw(45426);
    return log2(x) * ONE_OVER_LOG_2_E;
}

template <unsigned int F>
fp32_t<F> exp(fp32_t<F> x)
{
    auto y = x;
    y = y - ((y * (ln(y) - x)) >> F);
    y = y - ((y * (ln(y) - x)) >> F);
    y = y - ((y * (ln(y) - x)) >> F);
    y = y - ((y * (ln(y) - x)) >> F);
    return y;
}
