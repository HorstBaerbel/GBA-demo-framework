#pragma once

//DON'T include these directly!!! No namespace here, because directly included in fp32.h

template <unsigned int F>
inline fp32_t<F> abs(fp32_t<F> x)
{
	return (x.raw() < 0 ? -x : x);
}

template <unsigned int F>
inline fp32_t<F> frac(fp32_t<F> x)
{
	return fp32_t<F>::fromRaw((x.raw() & fp32_t<F>::MASKF) + ((x.raw() >> (fp32_t<F>::BITS - 1)) << F));
}

template <unsigned int F>
inline fp32_t<F> trunc(fp32_t<F> x)
{
	return fp32_t<F>::fromRaw((x.raw() & fp32_t<F>::MASKQ) - ((x.raw() >> (fp32_t<F>::BITS - 1)) << F));
}

template <unsigned int F>
inline fp32_t<F> floor(fp32_t<F> x)
{
	// mask out fractional part, leaving only integer
	return fp32_t<F>::fromRaw(x.raw() & fp32_t<F>::MASKQ);
}

template <typename T, unsigned int F>
inline T floor(fp32_t<F> x)
{
	// mask out fractional part, leaving only integer
	return static_cast<T>(x.raw() >> F);
}

template <unsigned int F>
inline fp32_t<F> ceil(fp32_t<F> x)
{
	// mask out fractional part, leaving only integer and add 1 for next higher number
	return fp32_t<F>::fromRaw((x.raw() & fp32_t<F>::MASKQ) + (1 << F));
}

template <typename T, unsigned int F>
inline T ceil(fp32_t<F> x)
{
	// truncate, shift to integer and add 1 for next higher number
	return static_cast<T>((x.raw() >> F) + 1);
}

template <unsigned int F>
inline fp32_t<F> round(fp32_t<F> x)
{
	// add 0.5 and truncate
	return fp32_t<F>::fromRaw((x.raw() + (1 << (F-1))) & fp32_t<F>::MASKQ);
}

template <typename T, unsigned int F>
inline T round(fp32_t<F> x)
{
	// add 0.5, truncate, shift to integer
	return static_cast<T>((x.raw() + (1 << (F-1))) >> F);
}

template <unsigned int F>
inline fp32_t<F> min(fp32_t<F> a, fp32_t<F> b)
{
	return (a.raw() < b.raw() ? a : b);
}

template <unsigned int F>
inline fp32_t<F> max(fp32_t<F> a, fp32_t<F> b)
{
	return (a.raw() > b.raw() ? a : b);
}

template <unsigned int F>
inline fp32_t<F> clamp(fp32_t<F> x, fp32_t<F> lo, fp32_t<F> hi)
{
	return max(min(x, hi), lo);
}
