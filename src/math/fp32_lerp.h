#pragma once

// DON'T include these directly!!! No namespace here, because directly included in fp32.h

template <unsigned int F>
inline fp32_t<F> ease_in(fp32_t<F> t)
{
	return (t * t * t);
}

template <unsigned int F>
inline fp32_t<F> ease_out(fp32_t<F> t)
{
	fp32_t<F> x = t - fp32_t<F>(1);
	return ((x * x * x) + fp32_t<F>(1));
}

template <unsigned int F>
inline fp32_t<F> smoothstep(fp32_t<F> t)
{
	fp32_t<F> x2 = t * t;
	return (fp32_t<F>(3) - fp32_t<F>(2) * t) * x2;
}

template <unsigned int F>
inline fp32_t<F> ease_cubic(fp32_t<F> t)
{
	if (t < fp32_t<F>::HALF)
	{
		return fp32_t<F>(4) * t * t * t;
	}
	else
	{
		fp32_t<F> x = fp32_t<F>(-2) * t + fp32_t<F>(2);
		return fp32_t<F>::ONE - (fp32_t<F>::HALF * x * x * x);
	}
}

template <unsigned int F>
inline fp32_t<F> lerp(fp32_t<F> a, fp32_t<F> b, fp32_t<F> t)
{
	//a * (1-t) + bt = a - at + bt = a + -at + bt = a + (b-a)t
	return a + (t * (b - a));
}

template <unsigned int F>
inline fp32_t<F> in_pause_out(fp32_t<F> t)
{
	fp32_t<F> x = t - fp32_t<F>(1);
	return (x * x * x);
}
