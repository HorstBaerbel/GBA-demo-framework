#pragma once

#include <cstdint>
#include <type_traits>
//#include <initializer_list>

//#define CHECK_OVERFLOW 1 // define this to check for overflow on assigment or arithmetic
//#define CHECK_PRECISION_LOSS 1 // define this to check for precision loss on arithmetic
//#define CHECK_ERROR 1 // define this to check for arithmetic errors like division by 0 or sqrt of a negative number
//#define PRINT_MESSAGE true // define this true print a message to the emulator when a fixed-point assert happens, else only the errors are counted
#if defined(CHECK_OVERFLOW) || defined(CHECK_PRECISION_LOSS)
#include "fpassert.h"
#include "fphelpers.h"
#endif

namespace Math
{

	// Fixed-point number template
	template <unsigned int F>
	struct fp16_t
	{
	private:
		int16_t value;

	public:
		constexpr static const uint32_t BITS = 16;						   // number of bits in number
		constexpr static const uint32_t BITSF = F;						   // number of bits for fractional part
		constexpr static const uint32_t BITSQ = (BITS - F);				   // number of bits for integer part
		constexpr static const uint32_t MASKF = ((1 << F) - 1);			   // mask to get (unsigned) fractional part
		constexpr static const uint32_t MASKQ = (~MASKF);				   // mask to get integer part
		constexpr static const uint32_t VALUE_MAX = (1 << (BITS - F - 1)); // maximum value that can be stored
		constexpr static const uint32_t VALUE_MIN = -(VALUE_MAX - 1);	   // minimum value that can be stored

		constexpr inline fp16_t() {}
		template <unsigned int T>
		constexpr inline fp16_t(fp16_t<T> b) : value(b.raw()) {}
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr inline fp16_t(int b) : value(((int16_t)b) << F) {}
		constexpr inline fp16_t(int32_t b) : value((int16_t)b << F) {}
		constexpr inline fp16_t(float b) : value(b * (1 << F)) {}
		//constexpr inline fp16_t(std::initializer_list<int32_t> l) : value(*l.begin()) {}

		inline fp16_t &operator=(fp16_t b)
		{
			value = b.raw();
			return *this;
		}

		template <unsigned int T>
		inline fp16_t &operator=(fp16_t<T> b)
		{
			if (T > F)
			{
#ifdef CHECK_PRECISION_LOSS
				fpassert_precision(lowestBitSet(b.raw()) > F, PRINT_MESSAGE);
#endif
				value = b.raw() >> (T - F);
			}
			else if (T < F)
			{
#ifdef CHECK_OVERFLOW
				fpassert_overflow(highestBitSet(b.raw()) <= (15 - F), PRINT_MESSAGE);
#endif
				value = b.raw() << (F - T);
			}
			else
			{
				value = b.raw();
			}
			return *this;
		}

		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		inline fp16_t &operator=(int b)
		{
			value = ((int16_t)b) << F;
			return *this;
		}

		inline fp16_t &operator=(int32_t b)
		{
#ifdef CHECK_OVERFLOW
			fpassert_overflow(highestBitSet(b) <= (15 - F), PRINT_MESSAGE);
#endif
			value = (int16_t)b << F;
			return *this;
		}

		inline fp16_t &operator=(float b)
		{
#ifdef CHECK_OVERFLOW
			fpassert_overflow(highestBitSet(b) <= (15 - F), PRINT_MESSAGE);
#endif
			value = b * (1 << F);
			return *this;
		}

		constexpr inline int16_t raw() const { return value; }
		static constexpr inline fp16_t<F> fromRaw(int16_t a) { return {a}; }

		explicit inline operator int32_t() const { return value >> F; }
		explicit inline operator int16_t() const { return value >> F; }
		explicit inline operator int8_t() const { return value >> F; }
		explicit inline operator float() { return (float)value / (float)(1 << F); }

		explicit inline operator uint32_t() const
		{
#ifdef CHECK_OVERFLOW
			fpassert_overflow(value < 0, PRINT_MESSAGE);
#endif
			return value >> F;
		}

		explicit inline operator uint16_t() const
		{
#ifdef CHECK_OVERFLOW
			fpassert_overflow(value < 0, PRINT_MESSAGE);
#endif
			return value >> F;
		}

		explicit inline operator uint8_t() const
		{
#ifdef CHECK_OVERFLOW
			fpassert_overflow(value < 0, PRINT_MESSAGE);
#endif
			return value >> F;
		}

		inline fp16_t &operator+=(fp16_t<F> b);
		inline fp16_t &operator-=(fp16_t<F> b);
		inline fp16_t &operator*=(fp16_t<F> b);
		fp16_t &operator/=(fp16_t<F> b);

		inline fp16_t &operator<<=(uint32_t shift);
		inline fp16_t &operator>>=(uint32_t shift);
		inline fp16_t &operator+=(int32_t b);
		inline fp16_t &operator-=(int32_t b);
		inline fp16_t &operator*=(int32_t b);
		fp16_t &operator/=(int32_t b);
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		inline fp16_t &operator+=(int16_t b) { return *this += (int32_t)b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		inline fp16_t &operator-=(int16_t b) { return *this -= (int32_t)b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		inline fp16_t &operator*=(int16_t b) { return *this *= (int32_t)b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		fp16_t &operator/=(int16_t b) { return *this /= (int32_t)b; }

		// Comparison ------------------------------------------------------------------------------------------------
		friend inline bool operator==(fp16_t<F> a, fp16_t<F> b) { return a.value == b.value; }
		friend inline bool operator!=(fp16_t<F> a, fp16_t<F> b) { return a.value != b.value; }
		friend inline bool operator>(fp16_t<F> a, fp16_t<F> b) { return a.value > b.value; }
		friend inline bool operator>=(fp16_t<F> a, fp16_t<F> b) { return a.value >= b.value; }
		friend inline bool operator<(fp16_t<F> a, fp16_t<F> b) { return a.value < b.value; }
		friend inline bool operator<=(fp16_t<F> a, fp16_t<F> b) { return a.value <= b.value; }

		friend inline bool operator==(fp16_t<F> a, int32_t b) { return ((a.value >> F) == b) && ((a.value & MASKF) == 0); }
		friend inline bool operator!=(fp16_t<F> a, int32_t b) { return !(a == b); }
		friend inline bool operator>(fp16_t<F> a, int32_t b) { return ((a.value >> F) > b) || (((a.value >> F) == b) && ((a.value & MASKF) > 0)); }
		friend inline bool operator>=(fp16_t<F> a, int32_t b) { return !(a < b); }
		friend inline bool operator<(fp16_t<F> a, int32_t b) { return (a.value >> F) < b; }
		friend inline bool operator<=(fp16_t<F> a, int32_t b) { return !(a > b); }

		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		friend inline bool operator==(fp16_t<F> a, int16_t b) { return ((a.value >> F) == b) && ((a.value & MASKF) == 0); }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		friend inline bool operator!=(fp16_t<F> a, int16_t b) { return !(a == b); }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		friend inline bool operator>(fp16_t<F> a, int16_t b) { return ((a.value >> F) > b) || (((a.value >> F) == b) && ((a.value & MASKF) > 0)); }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		friend inline bool operator>=(fp16_t<F> a, int16_t b) { return !(a > b); }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		friend inline bool operator<(fp16_t<F> a, int16_t b) { return (a.value >> F) < b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		friend inline bool operator<=(fp16_t<F> a, int16_t b) { return !(a < b); }

		static const fp16_t<F> EPSILON;
		static const fp16_t<F> ZERO;
		static const fp16_t<F> QUARTER;
		static const fp16_t<F> HALF;
		static const fp16_t<F> THREE_QUARTER;
		static const fp16_t<F> ONE;
		static const fp16_t<F> PI;
		static const fp16_t<F> PI_2;
		static const fp16_t<F> PI_HALF;
		static const fp16_t<F> PI_3_HALF;
		static const fp16_t<F> PI_QUARTER;
		static const fp16_t<F> PI_3_QUARTER;
		static const fp16_t<F> ONE_OVER_PI;
		static const fp16_t<F> ONE_OVER_PI_2;
	} __attribute__((packed));

	using fp412_t = fp16_t<12>; //4.12
	using fp88_t = fp16_t<8>;	//8.8
	using fp124_t = fp16_t<4>;	//12.4

	// arithmetic -----------------------------------------------------------------------------------------------

#include "fp16_arith.h"
//#include "fp16_trig.h"
#include "fp16_minmax.h"
#include "fp16_lerp.h"

} // namespace Math
