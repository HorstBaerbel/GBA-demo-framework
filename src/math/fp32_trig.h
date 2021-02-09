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
