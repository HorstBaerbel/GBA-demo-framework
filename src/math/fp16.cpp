#include "fp16.h"

namespace Math
{

	template<> const fp16_t<4> fp16_t<4>::EPSILON = fp16_t<4>::fromRaw(3);
	template<> const fp16_t<4> fp16_t<4>::ZERO = fp16_t<4>(0);
	template<> const fp16_t<4> fp16_t<4>::QUARTER = fp16_t<4>(0.25f);
	template<> const fp16_t<4> fp16_t<4>::HALF = fp16_t<4>(0.5f);
	template<> const fp16_t<4> fp16_t<4>::THREE_QUARTER = fp16_t<4>(0.75f);
	template<> const fp16_t<4> fp16_t<4>::ONE = fp16_t<4>(1);
	template<> const fp16_t<4> fp16_t<4>::PI = fp16_t<4>(3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::PI_2 = fp16_t<4>(2.0f * 3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::PI_HALF = fp16_t<4>(0.5f * 3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::PI_3_HALF = fp16_t<4>(3.0f * 0.5f * 3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::PI_QUARTER = fp16_t<4>(0.25f * 3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::PI_3_QUARTER = fp16_t<4>(3.0f * 0.25f * 3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::ONE_OVER_PI = fp16_t<4>(1.0f / 3.1415926535f);
	template<> const fp16_t<4> fp16_t<4>::ONE_OVER_PI_2 = fp16_t<4>(1.0f / (2.0f * 3.1415926535f));
	template<> const fp16_t<8> fp16_t<8>::EPSILON = fp16_t<8>::fromRaw(3);
	template<> const fp16_t<8> fp16_t<8>::ZERO = fp16_t<8>(0);
	template<> const fp16_t<8> fp16_t<8>::QUARTER = fp16_t<8>(0.25f);
	template<> const fp16_t<8> fp16_t<8>::HALF = fp16_t<8>(0.5f);
	template<> const fp16_t<8> fp16_t<8>::THREE_QUARTER = fp16_t<8>(0.75f);
	template<> const fp16_t<8> fp16_t<8>::ONE = fp16_t<8>(1);
	template<> const fp16_t<8> fp16_t<8>::PI = fp16_t<8>(3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::PI_2 = fp16_t<8>(2.0f * 3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::PI_HALF = fp16_t<8>(0.5f * 3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::PI_3_HALF = fp16_t<8>(3.0f * 0.5f * 3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::PI_QUARTER = fp16_t<8>(0.25f * 3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::PI_3_QUARTER = fp16_t<8>(3.0f * 0.25f * 3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::ONE_OVER_PI = fp16_t<8>(1.0f / 3.1415926535f);
	template<> const fp16_t<8> fp16_t<8>::ONE_OVER_PI_2 = fp16_t<8>(1.0f / (2.0f * 3.1415926535f));
	template<> const fp16_t<12> fp16_t<12>::EPSILON = fp16_t<12>::fromRaw(3);
	template<> const fp16_t<12> fp16_t<12>::ZERO = fp16_t<12>(0);
	template<> const fp16_t<12> fp16_t<12>::QUARTER = fp16_t<12>(0.25f);
	template<> const fp16_t<12> fp16_t<12>::HALF = fp16_t<12>(0.5f);
	template<> const fp16_t<12> fp16_t<12>::THREE_QUARTER = fp16_t<12>(0.75f);
	template<> const fp16_t<12> fp16_t<12>::ONE = fp16_t<12>(1);
	template<> const fp16_t<12> fp16_t<12>::PI = fp16_t<12>(3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::PI_2 = fp16_t<12>(2.0f * 3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::PI_HALF = fp16_t<12>(0.5f * 3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::PI_3_HALF = fp16_t<12>(3.0f * 0.5f * 3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::PI_QUARTER = fp16_t<12>(0.25f * 3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::PI_3_QUARTER = fp16_t<12>(3.0f * 0.25f * 3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::ONE_OVER_PI = fp16_t<12>(1.0f / 3.1415926535f);
	template<> const fp16_t<12> fp16_t<12>::ONE_OVER_PI_2 = fp16_t<12>(1.0f / (2.0f * 3.1415926535f));

}
