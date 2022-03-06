#pragma once

//DON'T include these directly!!! No namespace here, because directly included in fp32.h

#include "fparith.h"
#include "helpers.h"

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator+=(fp32_t<F> b)
{
#ifdef CHECK_OVERFLOW
	fpassert_overflow(is_add_safe(value, b.value), PRINT_MESSAGE);
#endif
	value += b.value;
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator+(fp32_t<F> a, fp32_t<F> b)
{
	fp32_t<F> result(a);
	result += b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator+=(int32_t b)
{
#ifdef CHECK_OVERFLOW
	fpassert_overflow(highestBitSet(b) <= (31-F)), PRINT_MESSAGE);
	fpassert_overflow(is_add_safe(value, b << F), PRINT_MESSAGE);
#endif
	value += b << F;
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator+(fp32_t<F> a, int32_t b)
{
	fp32_t<F> result(a);
	result += b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> operator+(int32_t a, fp32_t<F> b)
{
	return b + a;
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator-=(fp32_t<F> b)
{
#ifdef CHECK_OVERFLOW
	fpassert_overflow(is_add_safe(value, b.value), PRINT_MESSAGE);
#endif
	value -= b.value;
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator-(fp32_t<F> a, fp32_t<F> b)
{
	fp32_t<F> result(a);
	result -= b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator-=(int32_t b)
{
#ifdef CHECK_OVERFLOW
	fpassert_overflow(highestBitSet(b) <= (31-F)), PRINT_MESSAGE);
	fpassert_overflow(is_add_safe(value, b << F), PRINT_MESSAGE);
#endif
	value -= b << F;
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator-(fp32_t<F> a, int32_t b)
{
	fp32_t<F> result(a);
	result -= b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> operator-(int32_t a, fp32_t<F> b)
{
	fp32_t<F> result(a);
	result -= b;
	return result;
}

// Unary minus (invert)
template <unsigned int F>
inline fp32_t<F> operator-(fp32_t<F> a)
{
	return fp32_t<F>::fromRaw(-a.raw());
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator*=(fp32_t<F> b)
{
#ifdef CHECK_OVERFLOW
	//We're using a 64 bit multiply, thus this is always safe. this will be different for 32bit multiplies, so we count the overflows
	fpassert_overflow(is_mul_safe(value, b.value), false);
#endif
	/*#if defined(__GNUC__) && defined(__arm__)
	//Code from here: http://me.henri.net/fp-div.html
	asm("smull r2, r3, %[a], %[b] @; i_fpmul()\n\t"
		"mov %[c], r2, lsr #16\n\t"
		"orr %[c], r2, r3, asl #16"
		: [c] "=r" (value) //output register
		: [a] "0" (value), [b] "r" (b.value) //input registers
		: "r2", "r3"); //clobberred registers
#else*/
	//add 0.5 after multiplying to round result, then shift down
	value = ((long long)value * b.value + (1 << (F - 1))) >> F;
	//#endif
	return *this;
}

/*template<unsigned int T, unsigned int B = F, typename std::enable_if<T < F>::type...>
fp32_t &operator*=(fp32_t<T> b)
{
	//add 0.5 after multiplying to round result, then shift down
	value = ((long long)value * b.value + (1 << (F-1))) >> F;
}

template<unsigned int T, unsigned int B = F, typename std::enable_if<T > F>::type...>
fp32_t &operator*=(fp32_t<T> b)
{

}*/

template <unsigned int F>
inline fp32_t<F> operator*(fp32_t<F> a, fp32_t<F> b)
{
	fp32_t<F> result(a);
	result *= b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator*=(int32_t b)
{
#ifdef CHECK_OVERFLOW
	fpassert_overflow(highestBitSet(b) <= (31-F)), PRINT_MESSAGE);
	fpassert_overflow(is_mul_safe(value, b << F), false);
#endif
	/*#if defined(__GNUC__) && defined(__arm__)
	//Code from here: http://me.henri.net/fp-div.html
	asm("smull r2, r3, %[a], %[b] @; i_fpmul()\n\t"
		"mov %[c], r2, lsr #16\n\t"
		"orr %[c], r2, r3, asl #16"
		: [c] "=r" (value) //output register
		: [a] "0" (value), [b] "r" (b.value) //input registers
		: "r2", "r3"); //clobberred registers
#else*/
	//add 0.5 after multiplying to round result, then shift down
	value = ((long long)value * (b << F) + (1 << (F - 1))) >> F;
	//#endif
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator*(fp32_t<F> a, int32_t b)
{
	fp32_t<F> result(a);
	result *= b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> operator*(int32_t a, fp32_t<F> b)
{
	return b * a;
}

template <unsigned int F>
fp32_t<F> &fp32_t<F>::operator/=(fp32_t<F> b)
{
	if (b.value == 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		value = 0;
		return *this;
	}
#ifdef CHECK_OVERFLOW
	fpassert_overflow(highestBitSet(b.value), PRINT_MESSAGE);
#endif
	/*#if defined(__GNUC__) && defined(__arm__)
	//Code from here: http://me.henri.net/fp-div.html
	//TODO: Only for 16.16!!!
	int32_t numerator = value;
	int32_t denominator = b.value;
	int32_t quotient;
	asm("num     .req %[numerator]      @ Map Register Equates\n\t"
		"den     .req %[denominator]\n\t"
		"mod     .req r2\n\t"
		"cnt     .req r3\n\t"
		"quo     .req r4\n\t"
		"sign    .req r12\n\t"
		//set sign and ensure numerator and denominator are positive
		"cmp den, #0                    @ exceptioin if den == zero\n\t"
		"beq .div0\n\t"
		"eor sign, num, den             @ sign = num ^ den\n\t"
		"rsbmi den, den, #0             @ den = -den if den < 0\n\t"
		"subs mod, den, #1              @ mod = den - 1\n\t"
		"beq .div1                      @ return if den == 1\n\t"
		"movs cnt, num                  @ num = -num if num < 0\n\t"
		"rsbmi num, num, #0\n\t"
		//skip if deniminator >= numerator
		"movs cnt, num, lsr #16         @ return if den >= num << 16\n\t"
		"bne .cont\n\t"
		"cmp den, num, lsl #16\n\t"
		"bhs .numLeDen\n\t"
	"\n.cont:\n\t"
		//test if denominator is a power of two
		"tst den, mod                   @ if(den & (den - 1) == 0)\n\t"
		"beq .powerOf2                  @ den is power of 2\n\t
		//count leading zeros
		"stmfd sp!, {r4}                @ push r4 (quo) onto the stack\n\t"
		"mov cnt, #28                   @ count difference in leading zeros\n\t"
		"mov mod, num, lsr #4           @ between num and den\n\t"
		"cmp den, mod, lsr #12; subls cnt, cnt, #16; movls mod, mod, lsr #16\n\t"
		"cmp den, mod, lsr #4 ; subls cnt, cnt, #8 ; movls mod, mod, lsr #8\n\t"
		"cmp den, mod         ; subls cnt, cnt, #4 ; movls mod, mod, lsr #4\n\t"
		//shift numerator left by cnt bits
		"mov num, num, lsl cnt          @ mod:num = num << cnt\n\t"
		"mov quo, #0\n\t"
		"rsb den, den, #0               @ negate den for divide loop\n\t"
		//skip cnt iterations in the divide loop
		"adds num, num, num             @ start: num = mod:num / den\n\t"
		"add pc, pc, cnt, lsl #4        @ skip cnt x 4 x 4 iterations\n\t"
		"nop                            @ nop instruction takes care of pipeline\n\t"
		//inner loop unrolled x 48
		".rept 47                       @ inner loop x 48\n\t"
		"    adcs mod, den, mod, lsl #1\n\t"
		"    subcc mod, mod, den\n\t"
		"    adc quo, quo, quo\n\t"
		"    adds num, num, num\n\t"
		".endr\n\t"
		"adcs mod, den, mod, lsl #1\n\t"
		"subcc mod, mod, den\n\t"
		"adc quo, quo, quo\n\t"
		//negate quotient if signed
		"cmp sign, #0                   @ negate quotient if sign < 0\n\t"
		"mov num, quo\n\t"
		"rsbmi num, num, #0\n\t"
		"ldmfd sp!, {r4}                @ pop r4 (quo) off the stack\n\t"
		"mov pc, lr                     @return\n\t"
		//divide by zero handler
	"\n.div0:\n\t"
		"mov num, #0\n\t"
		"mov pc, lr                     @return\n\t"
		//divide by one handler
	"\n.div1:\n\t"
		"cmp sign, #0\n\t"
		"mov num, num, asl #16\n\t"
		"rsbmi num, num, #0\n\t"
		"mov pc, lr                     @return\n\t"
		//numerator less than or equal to denominator handler
	"\n.numLeDen:\n\t"
		"mov num, #0                    @ quotient = 0 if num < den\n\t"
		"moveq num, sign, asr #31       @ negate quotient if sign < 0\n\t"
		"orreq num, num, #1             @ quotient = 1 if num == den\n\t"
		"mov pc, lr                     @return\n\t"
		//power of two handler
	"\n.powerOf2:\n\t"
		"mov cnt, #0\n\t"
		"cmp den, #(1 << 16); movhs cnt, #16    ; movhs den, den, lsr #16\n\t"
		"cmp den, #(1 << 8) ; addhs cnt, cnt, #8; movhs den, den, lsr #8\n\t"
		"cmp den, #(1 << 4) ; addhs cnt, cnt, #4; movhs den, den, lsr #4\n\t"
		"cmp den, #(1 << 2) ; addhi cnt, cnt, #3; addls cnt, cnt, den, lsr #1\n\t"
		"rsb mod, cnt, #32\n\t"
		"mov den, num, lsr #16          @ den:num = num << 16\n\t"
		"mov num, num, lsl #16\n\t"
		"mov num, num, lsr cnt          @ num = num >> cnt | den << mod\n\t"
		"orr num, num, den, lsl mod\n\t"
		"cmp sign, #0\n\t"
		"rsbmi num, num, #0             @ negate quotient if sign < 0"
		: [quotient] "=r" (quotient) //output registers
		: [numerator] "0" (numerator), [denominator] "r" (denominator) //input registers
		: "r2", "r3", "r12"); //clobbered registers
	value = quotient;
#else*/
	//add 0.5 before division to round result before dividing
	value = ((int64_t)value << F) / b.value;
	//#endif
	return *this;
}

template <unsigned int F>
fp32_t<F> operator/(fp32_t<F> a, fp32_t<F> b)
{
	fp32_t<F> result(a);
	result /= b;
	return result;
}

template <unsigned int F>
fp32_t<F> &fp32_t<F>::operator/=(int32_t b)
{
	if (value == 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		value = 0;
		return *this;
	}
#ifdef CHECK_OVERFLOW
	//We're using a 64 bit divident, thus this is always safe. this will be different for 32bit divides
	fpassert_overflow(highestBitSet(b) <= (31-F)), PRINT_MESSAGE);
#endif
	/*#if defined(__GNUC__) && defined(__arm__)
	//Code from here: http://me.henri.net/fp-div.html
	//TODO: Only for 16.16!!!
	int32_t numerator = value;
	int32_t denominator = b.value;
	int32_t quotient;
	asm("num     .req %[numerator]      @ Map Register Equates\n\t"
		"den     .req %[denominator]\n\t"
		"mod     .req r2\n\t"
		"cnt     .req r3\n\t"
		"quo     .req r4\n\t"
		"sign    .req r12\n\t"
		//set sign and ensure numerator and denominator are positive
		"cmp den, #0                    @ exceptioin if den == zero\n\t"
		"beq .div0\n\t"
		"eor sign, num, den             @ sign = num ^ den\n\t"
		"rsbmi den, den, #0             @ den = -den if den < 0\n\t"
		"subs mod, den, #1              @ mod = den - 1\n\t"
		"beq .div1                      @ return if den == 1\n\t"
		"movs cnt, num                  @ num = -num if num < 0\n\t"
		"rsbmi num, num, #0\n\t"
		//skip if deniminator >= numerator
		"movs cnt, num, lsr #16         @ return if den >= num << 16\n\t"
		"bne .cont\n\t"
		"cmp den, num, lsl #16\n\t"
		"bhs .numLeDen\n\t"
	"\n.cont:\n\t"
		//test if denominator is a power of two
		"tst den, mod                   @ if(den & (den - 1) == 0)\n\t"
		"beq .powerOf2                  @ den is power of 2\n\t
		//count leading zeros
		"stmfd sp!, {r4}                @ push r4 (quo) onto the stack\n\t"
		"mov cnt, #28                   @ count difference in leading zeros\n\t"
		"mov mod, num, lsr #4           @ between num and den\n\t"
		"cmp den, mod, lsr #12; subls cnt, cnt, #16; movls mod, mod, lsr #16\n\t"
		"cmp den, mod, lsr #4 ; subls cnt, cnt, #8 ; movls mod, mod, lsr #8\n\t"
		"cmp den, mod         ; subls cnt, cnt, #4 ; movls mod, mod, lsr #4\n\t"
		//shift numerator left by cnt bits
		"mov num, num, lsl cnt          @ mod:num = num << cnt\n\t"
		"mov quo, #0\n\t"
		"rsb den, den, #0               @ negate den for divide loop\n\t"
		//skip cnt iterations in the divide loop
		"adds num, num, num             @ start: num = mod:num / den\n\t"
		"add pc, pc, cnt, lsl #4        @ skip cnt x 4 x 4 iterations\n\t"
		"nop                            @ nop instruction takes care of pipeline\n\t"
		//inner loop unrolled x 48
		".rept 47                       @ inner loop x 48\n\t"
		"    adcs mod, den, mod, lsl #1\n\t"
		"    subcc mod, mod, den\n\t"
		"    adc quo, quo, quo\n\t"
		"    adds num, num, num\n\t"
		".endr\n\t"
		"adcs mod, den, mod, lsl #1\n\t"
		"subcc mod, mod, den\n\t"
		"adc quo, quo, quo\n\t"
		//negate quotient if signed
		"cmp sign, #0                   @ negate quotient if sign < 0\n\t"
		"mov num, quo\n\t"
		"rsbmi num, num, #0\n\t"
		"ldmfd sp!, {r4}                @ pop r4 (quo) off the stack\n\t"
		"mov pc, lr                     @return\n\t"
		//divide by zero handler
	"\n.div0:\n\t"
		"mov num, #0\n\t"
		"mov pc, lr                     @return\n\t"
		//divide by one handler
	"\n.div1:\n\t"
		"cmp sign, #0\n\t"
		"mov num, num, asl #16\n\t"
		"rsbmi num, num, #0\n\t"
		"mov pc, lr                     @return\n\t"
		//numerator less than or equal to denominator handler
	"\n.numLeDen:\n\t"
		"mov num, #0                    @ quotient = 0 if num < den\n\t"
		"moveq num, sign, asr #31       @ negate quotient if sign < 0\n\t"
		"orreq num, num, #1             @ quotient = 1 if num == den\n\t"
		"mov pc, lr                     @return\n\t"
		//power of two handler
	"\n.powerOf2:\n\t"
		"mov cnt, #0\n\t"
		"cmp den, #(1 << 16); movhs cnt, #16    ; movhs den, den, lsr #16\n\t"
		"cmp den, #(1 << 8) ; addhs cnt, cnt, #8; movhs den, den, lsr #8\n\t"
		"cmp den, #(1 << 4) ; addhs cnt, cnt, #4; movhs den, den, lsr #4\n\t"
		"cmp den, #(1 << 2) ; addhi cnt, cnt, #3; addls cnt, cnt, den, lsr #1\n\t"
		"rsb mod, cnt, #32\n\t"
		"mov den, num, lsr #16          @ den:num = num << 16\n\t"
		"mov num, num, lsl #16\n\t"
		"mov num, num, lsr cnt          @ num = num >> cnt | den << mod\n\t"
		"orr num, num, den, lsl mod\n\t"
		"cmp sign, #0\n\t"
		"rsbmi num, num, #0             @ negate quotient if sign < 0"
		: [quotient] "=r" (quotient) //output registers
		: [numerator] "0" (numerator), [denominator] "r" (denominator) //input registers
		: "r2", "r3", "r12"); //clobbered registers
	value = quotient;
#else*/
	//add 0.5 before division to round result before dividing
	value = ((int64_t)value << F) / (b << F);
	//#endif
	return *this;
}

template <unsigned int F>
fp32_t<F> operator/(fp32_t<F> a, int32_t b)
{
	fp32_t<F> result(a);
	result /= b;
	return result;
}

template <unsigned int F>
fp32_t<F> operator/(int32_t a, fp32_t<F> b)
{
	fp32_t<F> result(a);
	result /= b;
	return result;
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator<<=(uint32_t shift)
{
#ifdef CHECK_OVERFLOW
	fpassert_overflow((highestBitSet(value) + shift) > 31, PRINT_MESSAGE);
#endif
	value <<= shift;
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator<<(fp32_t<F> a, uint32_t shift)
{
	fp32_t<F> result(a);
	result <<= shift;
	return result;
}

template <unsigned int F>
inline fp32_t<F> &fp32_t<F>::operator>>=(uint32_t shift)
{
#ifdef CHECK_PRECISION_LOSS
	fpassert_precision((lowestBitSet(value) - shift) < 0, PRINT_MESSAGE);
#endif
	value >>= shift;
	return *this;
}

template <unsigned int F>
inline fp32_t<F> operator>>(fp32_t<F> a, uint32_t shift)
{
	fp32_t<F> result(a);
	result >>= shift;
	return result;
}

template <unsigned int F>
fp32_t<F> sqrt(fp32_t<F> a)
{
	if (a.raw() <= 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		return 0;
	}
	return fp32_t<F>::fromRaw(static_cast<int32_t>(sqrt(static_cast<uint32_t>(a.raw()), F)));
}

template <unsigned int F>
fp32_t<F> sqrtFast(fp32_t<F> a)
{
	if (a.raw() <= 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		return 0;
	}
	return fp32_t<F>::fromRaw(static_cast<int32_t>(sqrtFast(static_cast<uint32_t>(a.raw()), F)));
}

template <unsigned int F>
fp32_t<F> sqrtRecip(fp32_t<F> a)
{
	if (a.raw() <= 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		return 0;
	}
	return fp32_t<F>::fromRaw(static_cast<int32_t>(sqrtRecip(static_cast<uint32_t>(a.raw()))));
}

template <unsigned int F>
fp32_t<F> sqrtRecipFast(fp32_t<F> a)
{
	if (a.raw() <= 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		return 0;
	}
	return fp32_t<F>::fromRaw(static_cast<int32_t>(sqrtRecipFast(static_cast<uint32_t>(a.raw()))));
}

template <unsigned int F>
fp32_t<(32 - F)> recip(fp32_t<F> a)
{
	if (a.value == 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		return 0;
	}
	const int32_t value = a.raw() < 0 ? -a.raw() : a.raw();
	int32_t exp = countLeadingZeros(static_cast<uint32_t>(value));
	int32_t x = ((int32_t)rcp_tab[(value >> (28 - exp)) & 0x7]) << 2;
	exp -= 16;

	if (exp <= 0)
		x >>= -exp;
	else
		x <<= exp;
	// two iterations of newton-raphson: x = x*(2 - a*x)
	x = (x * ((2 << (32 - F)) - ((value * x) >> F))) >> (32 - F);
	x = (x * ((2 << (32 - F)) - ((value * x) >> F))) >> (32 - F);
	// sign result and return
	return fp32_t<32 - F>::fromRaw(a.raw() < 0 ? -x : x);
}

template <unsigned int F>
fp32_t<(32 - F)> recipFast(const fp32_t<F> &a)
{
	if (a.value == 0)
	{
#ifdef CHECK_ERROR
		fpassert_error(false, PRINT_MESSAGE);
#endif
		return 0;
	}
	const int32_t value = a.raw() < 0 ? -a.raw() : a.raw();
	int32_t exp = countLeadingZeros(static_cast<uint32_t>(value));
	int32_t x = ((int32_t)rcp_tab[(value >> (28 - exp)) & 0x7]) << 2;
	exp -= 16;

	if (exp <= 0)
		x >>= -exp;
	else
		x <<= exp;
	// only one iterations of newton-raphson: x = x*(2 - a*x)
	x = (x * ((2 << (32 - F)) - ((value * x) >> F))) >> (32 - F);
	//x = ((int64_t)x * ((2<<(32-F)) - (((int64_t)value * x) >> F))) >> (32-F);
	// sign result and return
	return fp32_t<32 - F>::fromRaw(a.raw() < 0 ? -x : x);
}
