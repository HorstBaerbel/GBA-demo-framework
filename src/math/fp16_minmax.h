#pragma once

//DON'T include these directly!!! No namespace here, because directly included in fp16.h

template <unsigned int F>
inline fp16_t<F> abs(fp16_t<F> x)
{
	return (x.raw() < 0 ? -x : x);
}

template <unsigned int F>
inline fp16_t<F> frac(fp16_t<F> x)
{
	return fp16_t<F>::fromRaw((x.raw() & fp16_t<F>::MASKF) + ((x.raw() >> (fp16_t<F>::BITS - 1)) << F));
}

template <unsigned int F>
inline fp16_t<F> trunc(fp16_t<F> x)
{
	return fp16_t<F>::fromRaw((x.raw() & fp16_t<F>::MASKQ) - ((x.raw() >> (fp16_t<F>::BITS - 1)) << F));
}

template <unsigned int F>
inline fp16_t<F> floor(fp16_t<F> x)
{
	// mask out fractional part, leaving only integer
	return fp16_t<F>::fromRaw(x.raw() & fp16_t<F>::MASKQ);
}

template <typename T, unsigned int F>
inline T floor(fp16_t<F> x)
{
	// mask out fractional part, leaving only integer
	return static_cast<T>(x.raw() >> F);
}

template <unsigned int F>
inline fp16_t<F> ceil(fp16_t<F> x)
{
	// mask out fractional part, leaving only integer and add 1 for next higher number
	return fp16_t<F>::fromRaw((x.raw() & fp16_t<F>::MASKQ) + (1 << F));
}

template <typename T, unsigned int F>
inline T ceil(fp16_t<F> x)
{
	// truncate, shift to integer and add 1 for next higher number
	return static_cast<T>((x.raw() >> F) + 1);
}

template <unsigned int F>
inline fp16_t<F> round(fp16_t<F> x)
{
	// add 0.5 and truncate
	return fp16_t<F>::fromRaw((x.raw() + (1 << (F-1))) & fp16_t<F>::MASKQ);
}

template <typename T, unsigned int F>
inline T round(fp16_t<F> x)
{
	// add 0.5, truncate, shift to integer
	return static_cast<T>((x.raw() + (1 << (F-1))) >> F);
}

template <unsigned int F>
inline fp16_t<F> min(fp16_t<F> a, fp16_t<F> b)
{
	return (a.raw() < b.raw() ? a : b);
}

template <unsigned int F>
inline fp16_t<F> max(fp16_t<F> a, fp16_t<F> b)
{
	return (a.raw() > b.raw() ? a : b);
}

template <unsigned int F>
inline fp16_t<F> clamp(fp16_t<F> x, fp16_t<F> lo, fp16_t<F> hi)
{
	return max(min(x, hi), lo);
}
