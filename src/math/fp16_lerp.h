#pragma once

// DON'T include these directly!!! No namespace here, because directly included in fp16.h

template <unsigned int F>
inline fp16_t<F> ease_in(fp16_t<F> t)
{
	return (t * t * t);
}

template <unsigned int F>
inline fp16_t<F> ease_out(fp16_t<F> t)
{
	fp16_t<F> x = t - fp16_t<F>(1);
	return ((x * x * x) + fp16_t<F>(1));
}

template <unsigned int F>
inline fp16_t<F> smoothstep(fp16_t<F> t)
{
	fp16_t<F> x2 = t * t;
	return (fp16_t<F>(3) - fp16_t<F>(2) * t) * x2;
}

template <unsigned int F>
inline fp16_t<F> lerp(fp16_t<F> a, fp16_t<F> b, fp16_t<F> t)
{
	//a * (1-t) + bt = a - at + bt = a + -at + bt = a + (b-a)t
	return a + (t * (b - a));
}

template <unsigned int F>
inline fp16_t<F> in_pause_out(fp16_t<F> t)
{
	fp16_t<F> x = t - fp16_t<F>(1);
	return (x * x * x);
}
