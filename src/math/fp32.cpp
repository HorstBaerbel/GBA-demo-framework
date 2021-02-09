#include "fp32.h"

namespace Math
{

	template<> const fp32_t<8> fp32_t<8>::EPSILON = fp32_t<8>::fromRaw(3);
	template<> const fp32_t<8> fp32_t<8>::ZERO = fp32_t<8>(0);
	template<> const fp32_t<8> fp32_t<8>::QUARTER = fp32_t<8>(0.25f);
	template<> const fp32_t<8> fp32_t<8>::HALF = fp32_t<8>(0.5f);
	template<> const fp32_t<8> fp32_t<8>::THREE_QUARTER = fp32_t<8>(0.75f);
	template<> const fp32_t<8> fp32_t<8>::ONE = fp32_t<8>(1);
	template<> const fp32_t<8> fp32_t<8>::PI = fp32_t<8>(3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::PI_2 = fp32_t<8>(2.0f * 3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::PI_HALF = fp32_t<8>(0.5f * 3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::PI_3_HALF = fp32_t<8>(3.0f * 0.5f * 3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::PI_QUARTER = fp32_t<8>(0.25f * 3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::PI_3_QUARTER = fp32_t<8>(3.0f * 0.25f * 3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::ONE_OVER_PI = fp32_t<8>(1.0f / 3.1415926535f);
	template<> const fp32_t<8> fp32_t<8>::ONE_OVER_PI_2 = fp32_t<8>(1.0f / (2.0f * 3.1415926535f));
	template<> const fp32_t<16> fp32_t<16>::EPSILON = fp32_t<16>::fromRaw(3);
	template<> const fp32_t<16> fp32_t<16>::ZERO = fp32_t<16>(0);
	template<> const fp32_t<16> fp32_t<16>::QUARTER = fp32_t<16>(0.25f);
	template<> const fp32_t<16> fp32_t<16>::HALF = fp32_t<16>(0.5f);
	template<> const fp32_t<16> fp32_t<16>::THREE_QUARTER = fp32_t<16>(0.75f);
	template<> const fp32_t<16> fp32_t<16>::ONE = fp32_t<16>(1);
	template<> const fp32_t<16> fp32_t<16>::PI = fp32_t<16>(3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::PI_2 = fp32_t<16>(2.0f * 3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::PI_HALF = fp32_t<16>(0.5f * 3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::PI_3_HALF = fp32_t<16>(3.0f * 0.5f * 3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::PI_QUARTER = fp32_t<16>(0.25f * 3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::PI_3_QUARTER = fp32_t<16>(3.0f * 0.25f * 3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::ONE_OVER_PI = fp32_t<16>(1.0f / 3.1415926535f);
	template<> const fp32_t<16> fp32_t<16>::ONE_OVER_PI_2 = fp32_t<16>(1.0f / (2.0f * 3.1415926535f));
	template<> const fp32_t<24> fp32_t<24>::EPSILON = fp32_t<24>::fromRaw(3);
	template<> const fp32_t<24> fp32_t<24>::ZERO = fp32_t<24>(0);
	template<> const fp32_t<24> fp32_t<24>::QUARTER = fp32_t<24>(0.25f);
	template<> const fp32_t<24> fp32_t<24>::HALF = fp32_t<24>(0.5f);
	template<> const fp32_t<24> fp32_t<24>::THREE_QUARTER = fp32_t<24>(0.75f);
	template<> const fp32_t<24> fp32_t<24>::ONE = fp32_t<24>(1);
	template<> const fp32_t<24> fp32_t<24>::PI = fp32_t<24>(3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::PI_2 = fp32_t<24>(2.0f * 3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::PI_HALF = fp32_t<24>(0.5f * 3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::PI_3_HALF = fp32_t<24>(3.0f * 0.5f * 3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::PI_QUARTER = fp32_t<24>(0.25f * 3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::PI_3_QUARTER = fp32_t<24>(3.0f * 0.25f * 3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::ONE_OVER_PI = fp32_t<24>(1.0f / 3.1415926535f);
	template<> const fp32_t<24> fp32_t<24>::ONE_OVER_PI_2 = fp32_t<24>(1.0f / (2.0f * 3.1415926535f));

}
