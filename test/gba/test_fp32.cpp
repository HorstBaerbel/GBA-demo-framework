#include "debug/print.h"
//#include "random.h"
#include "math/fp32.h"

//disable GCC warnings for using char * here...
#pragma GCC diagnostic ignored "-Wwrite-strings"

namespace Test
{

	using namespace Math;

	void math_fp32()
	{
		printf("Fundamental types...");
		printf("Size of unsigned char: %d\n", (uint32_t)sizeof(unsigned char));
		printf("Size of unsigned short: %d\n", (uint32_t)sizeof(unsigned short));
		printf("Size of unsigned int: %d\n", (uint32_t)sizeof(unsigned int));
		printf("Size of long unsigned int: %d\n", (uint32_t)sizeof(long unsigned int));
		printf("Size of uint8_t: %d\n", (uint32_t)sizeof(uint8_t));
		printf("Size of uint16_t: %d\n", (uint32_t)sizeof(uint16_t));
		printf("Size of uint32_t: %d\n", (uint32_t)sizeof(uint32_t));
		printf("Size of void*: %d\n", (uint32_t)sizeof(void *));

		printf("Fixed-point math tests...");
		printf("\nfp824_t\n");
		printf("5.25 + 100 =   105.25 --> %d\n", fp824_t(5.25f) + fp824_t(100));
		printf("100 + -0.25 =   99.75 --> %d\n", fp824_t(100) + fp824_t(-0.25f));
		printf("0.25 - 100 =   -99.75 --> %d\n", fp824_t(0.25f) - fp824_t(100));
		printf("-10 - 99.25 = -109.25 --> %d\n", fp824_t(-10) - fp824_t(99.25f));
		printf("5.25 * 9.5 =   49.875 --> %d\n", fp824_t(5.25f) * fp824_t(9.5f));
		printf("0.033 * -100 =   -3.3 --> %d\n", fp824_t(0.033f) * fp824_t(-100));
		printf("0.1 * 0.1 =      0.01 --> %d\n", fp824_t(0.1f) * fp824_t(0.1f));
		printf("100 / 3 =        33.3 --> %d\n", fp824_t(100) / fp824_t(3));
		printf("1 / -0.25 =      -4.0 --> %d\n", fp824_t(1) / fp824_t(-0.25f));
		printf("1.234 / 100 = 0.01234 --> %d\n", fp824_t(1.234f) / fp824_t(100));
		printf("trunc(fp824_t(2.7))  --> %d\n", trunc(fp824_t(2.7f)));
		printf("trunc(fp824_t(-2.7)) --> %d\n", trunc(fp824_t(-2.7f)));
		printf("frac(fp824_t(2.7))   --> %d\n", frac(fp824_t(2.7f)));
		printf("frac(fp824_t(-2.7))  --> %d\n", frac(fp824_t(-2.7f)));
		printf("floor(fp824_t(2.7))  --> %d\n", floor(fp824_t(2.7f)));
		printf("floor(fp824_t(-2.7)) --> %d\n", floor(fp824_t(-2.7f)));
		printf("ceil(fp824_t(2.3))   --> %d\n", ceil(fp824_t(2.3f)));
		printf("ceil(fp824_t(-2.3))  --> %d\n", ceil(fp824_t(-2.3f)));
		printf("round(fp824_t(2.3))  --> %d\n", round(fp824_t(2.3f)));
		printf("round(fp824_t(-2.3)) --> %d\n", round(fp824_t(-2.3f)));
		printf("round(fp824_t(2.7))  --> %d\n", round(fp824_t(2.7f)));
		printf("round(fp824_t(-2.7)) --> %d\n", round(fp824_t(-2.7f)));
		printf("fp1616_t(123.45) to fp824_t  --> %d\n", fp824_t(fp1616_t(123.4f)));
		printf("fp1616_t(-123.45) to fp824_t --> %d\n", fp824_t(fp1616_t(-123.4f)));
		printf("fp248_t(123.45) to fp824_t   --> %d\n", fp824_t(fp248_t(123.4f)));
		printf("fp248_t(-123.45) to fp824_t  --> %d\n", fp824_t(fp248_t(-123.4f)));
		printf("sin(fp824_t(0))    =  0.00000 --> %d\n", sin(fp824_t(0)));
		printf("sin(fp824_t(1))    =  0.84147 --> %d\n", sin(fp824_t(1)));
		printf("sin(fp824_t(-1))   = -0.84147 --> %d\n", sin(fp824_t(-1)));
		printf("sin(fp824_t(PI/4)) =  0.70710 --> %d\n", sin(fp824_t(fp824_t::PI_QUARTER)));
		printf("sin(fp824_t(PI/2)) =  1.00000 --> %d\n", sin(fp824_t(fp824_t::PI_HALF)));
		printf("sin(fp824_t(PI))   =  0.00000 --> %d\n", sin(fp824_t(fp824_t::PI)));
		printf("sin(fp824_t(2*PI)) =  0.00000 --> %d\n", sin(fp824_t(fp824_t::PI_2)));
		printf("cos(fp824_t(0))    =  1.00000 --> %d\n", cos(fp824_t(0)));
		printf("cos(fp824_t(1))    =  0.54030 --> %d\n", cos(fp824_t(1)));
		printf("cos(fp824_t(-1))   =  0.54030 --> %d\n", cos(fp824_t(-1)));
		printf("cos(fp824_t(PI/4)) =  0.70710 --> %d\n", cos(fp824_t(fp824_t::PI_QUARTER)));
		printf("cos(fp824_t(PI/2)) =  0.00000 --> %d\n", cos(fp824_t(fp824_t::PI_HALF)));
		printf("cos(fp824_t(PI))   = -1.00000 --> %d\n", cos(fp824_t(fp824_t::PI)));
		printf("cos(fp824_t(2*PI)) =  1.00000 --> %d\n", cos(fp824_t(fp824_t::PI_2)));
		printf("tan(fp824_t(0))    =  0.00000 --> %d\n", tan(fp824_t(0)));
		printf("tan(fp824_t(0.5))  =  0.54630 --> %d\n", tan(fp824_t(0.5f)));
		printf("tan(fp824_t(1))    =  1.55740 --> %d\n", tan(fp824_t(1)));
		printf("tan(fp824_t(-1))   = -1.55740 --> %d\n", tan(fp824_t(-1)));
		printf("tan(fp824_t(PI/4)) =  1.00000 --> %d\n", tan(fp824_t(fp824_t::PI_QUARTER)));
		printf("tan(fp824_t(PI/2)) =  127.999 --> %d\n", tan(fp824_t(fp824_t::PI_HALF)));
		printf("tan(fp824_t(PI))   =  0.00000 --> %d\n", tan(fp824_t(fp824_t::PI)));
		printf("\nfp1616_t\n");
		printf("5.25 + 30000 =    30005.25 --> %d\n", fp1616_t(5.25f) + fp1616_t(30000));
		printf("30000 + -0.25 =   29999.75 --> %d\n", fp1616_t(30000) + fp1616_t(-0.25f));
		printf("0.25 - 30000 =   -29999.75 --> %d\n", fp1616_t(0.25f) - fp1616_t(30000));
		printf("-10 - 12345.67 = -12355.67 --> %d\n", fp1616_t(-10) - fp1616_t(12345.67f));
		printf("123.4 * 56.7 =     6996.78 --> %d\n", fp1616_t(123.4f) * fp1616_t(56.7f));
		printf("0.0333 * -10000 =   -333.0 --> %d\n", fp1616_t(0.0333f) * fp1616_t(-10000));
		printf("0.1 * 0.1 =           0.01 --> %d\n", fp1616_t(0.1f) * fp1616_t(0.1f));
		printf("12345 / 3 =         4115.0 --> %d\n", fp1616_t(12345) / fp1616_t(3));
		printf("1 / -0.25 =           -4.0 --> %d\n", fp1616_t(1) / fp1616_t(-0.25f));
		printf("1.234 / 100 =      0.01234 --> %d\n", fp1616_t(1.234f) / fp1616_t(100));
		printf("trunc(fp1616_t(2.7))  --> %d\n", trunc(fp1616_t(2.7f)));
		printf("trunc(fp1616_t(-2.7)) --> %d\n", trunc(fp1616_t(-2.7f)));
		printf("frac(fp1616_t(2.7))   --> %d\n", frac(fp1616_t(2.7f)));
		printf("frac(fp1616_t(-2.7))  --> %d\n", frac(fp1616_t(-2.7f)));
		printf("floor(fp1616_t(2.7))  --> %d\n", floor(fp1616_t(2.7f)));
		printf("floor(fp1616_t(-2.7)) --> %d\n", floor(fp1616_t(-2.7f)));
		printf("ceil(fp1616_t(2.3))   --> %d\n", ceil(fp1616_t(2.3f)));
		printf("ceil(fp1616_t(-2.3))  --> %d\n", ceil(fp1616_t(-2.3f)));
		printf("round(fp1616_t(2.3))  --> %d\n", round(fp1616_t(2.3f)));
		printf("round(fp1616_t(-2.3)) --> %d\n", round(fp1616_t(-2.3f)));
		printf("round(fp1616_t(2.7))  --> %d\n", round(fp1616_t(2.7f)));
		printf("round(fp1616_t(-2.7)) --> %d\n", round(fp1616_t(-2.7f)));
		printf("fp824_t(123.45) to fp1616_t   --> %d\n", fp1616_t(fp824_t(123.4f)));
		printf("fp824_t(-123.45) to fp1616_t  --> %d\n", fp1616_t(fp824_t(-123.4f)));
		printf("fp248_t(12345.6) to fp1616_t  --> %d\n", fp1616_t(fp248_t(12345.6f)));
		printf("fp248_t(-12345.6) to fp1616_t --> %d\n", fp1616_t(fp248_t(-12345.6f)));
		printf("sin(fp1616_t(0))    =  0.00000 --> %d\n", sin(fp1616_t(0)));
		printf("sin(fp1616_t(1))    =  0.84147 --> %d\n", sin(fp1616_t(1)));
		printf("sin(fp1616_t(-1))   = -0.84147 --> %d\n", sin(fp1616_t(-1)));
		printf("sin(fp1616_t(PI/4)) =  0.70710 --> %d\n", sin(fp1616_t(fp1616_t::PI_QUARTER)));
		printf("sin(fp1616_t(PI/2)) =  1.00000 --> %d\n", sin(fp1616_t(fp1616_t::PI_HALF)));
		printf("sin(fp1616_t(PI))   =  0.00000 --> %d\n", sin(fp1616_t(fp1616_t::PI)));
		printf("sin(fp1616_t(2*PI)) =  0.00000 --> %d\n", sin(fp1616_t(fp1616_t::PI_2)));
		printf("cos(fp1616_t(0))    =  1.00000 --> %d\n", cos(fp1616_t(0)));
		printf("cos(fp1616_t(1))    =  0.54030 --> %d\n", cos(fp1616_t(1)));
		printf("cos(fp1616_t(-1))   =  0.54030 --> %d\n", cos(fp1616_t(-1)));
		printf("cos(fp1616_t(PI/4)) =  0.70710 --> %d\n", cos(fp1616_t(fp1616_t::PI_QUARTER)));
		printf("cos(fp1616_t(PI/2)) =  0.00000 --> %d\n", cos(fp1616_t(fp1616_t::PI_HALF)));
		printf("cos(fp1616_t(PI))   = -1.00000 --> %d\n", cos(fp1616_t(fp1616_t::PI)));
		printf("cos(fp1616_t(2*PI)) =  1.00000 --> %d\n", cos(fp1616_t(fp1616_t::PI_2)));
		printf("tan(fp1616_t(0))    =  0.00000 --> %d\n", tan(fp1616_t(0)));
		printf("tan(fp1616_t(0.5))  =  0.54630 --> %d\n", tan(fp1616_t(0.5f)));
		printf("tan(fp1616_t(1))    =  1.55740 --> %d\n", tan(fp1616_t(1)));
		printf("tan(fp1616_t(-1))   = -1.55740 --> %d\n", tan(fp1616_t(-1)));
		printf("tan(fp1616_t(PI/4)) =  1.00000 --> %d\n", tan(fp1616_t(fp1616_t::PI_QUARTER)));
		printf("tan(fp1616_t(PI/2)) =  32767.9 --> %d\n", tan(fp1616_t(fp1616_t::PI_HALF)));
		printf("tan(fp1616_t(PI))   =  0.00000 --> %d\n", tan(fp1616_t(fp1616_t::PI)));
		printf("\nfp248_t\n");
		printf("5.25 + 1000000 =  1000005.25 --> %d\n", fp248_t(5.25f) + fp248_t(1000000));
		printf("1000000 + -0.25 = 9999999.75 --> %d\n", fp248_t(1000000) + fp248_t(-0.25f));
		printf("0.25 - 1000000 = -9999999.75 --> %d\n", fp248_t(0.25f) - fp248_t(1000000));
		printf("-10 - 1234567.8 = -1234577.8 --> %d\n", fp248_t(-10) - fp248_t(1234567.8f));
		printf("1234.5 * 567.8 =    700949.1 --> %d\n", fp248_t(1234.5f) * fp248_t(567.8f));
		printf("0.3 * -1000000 =     -300000 --> %d\n", fp248_t(0.3f) * fp248_t(-1000000));
		printf("0.1 * 0.1 =             0.01 --> %d\n", fp248_t(0.1f) * fp248_t(0.1f));
		printf("1234567 / 3 =       411522.3 --> %d\n", fp248_t(1234567) / fp248_t(3));
		printf("1 / -0.25 =             -4.0 --> %d\n", fp248_t(1) / fp248_t(-0.25f));
		printf("1.234 / 10 =          0.1234 --> %d\n", fp248_t(1.234f) / fp248_t(10));
		printf("trunc(fp248_t(2.7))  --> %d\n", trunc(fp248_t(2.7f)));
		printf("trunc(fp248_t(-2.7)) --> %d\n", trunc(fp248_t(-2.7f)));
		printf("frac(fp248_t(2.7))   --> %d\n", frac(fp248_t(2.7f)));
		printf("frac(fp248_t(-2.7))  --> %d\n", frac(fp248_t(-2.7f)));
		printf("floor(fp248_t(2.7))  --> %d\n", floor(fp248_t(2.7f)));
		printf("floor(fp248_t(-2.7)) --> %d\n", floor(fp248_t(-2.7f)));
		printf("ceil(fp248_t(2.3))  --> %d\n", ceil(fp248_t(2.3f)));
		printf("ceil(fp248_t(-2.3)) --> %d\n", ceil(fp248_t(-2.3f)));
		printf("round(fp248_t(2.3))  --> %d\n", round(fp248_t(2.3f)));
		printf("round(fp248_t(-2.3)) --> %d\n", round(fp248_t(-2.3f)));
		printf("round(fp248_t(2.7))  --> %d\n", round(fp248_t(2.7f)));
		printf("round(fp248_t(-2.7)) --> %d\n", round(fp248_t(-2.7f)));
		printf("fp1616_t(12345.6) to fp248_t  --> %d\n", fp248_t(fp1616_t(12345.6f)));
		printf("fp1616_t(-12345.6) to fp248_t --> %d\n", fp248_t(fp1616_t(-12345.6f)));
		printf("fp824_t(123.45) to fp248_t    --> %d\n", fp248_t(fp824_t(123.4f)));
		printf("fp824_t(-123.45) to fp248_t   --> %d\n", fp248_t(fp824_t(-123.4f)));
		printf("sin(fp248_t(0))    =  0.00000 --> %d\n", sin(fp248_t(0)));
		printf("sin(fp248_t(1))    =  0.84147 --> %d\n", sin(fp248_t(1)));
		printf("sin(fp248_t(-1))   = -0.84147 --> %d\n", sin(fp248_t(-1)));
		printf("sin(fp248_t(PI/4)) =  0.70710 --> %d\n", sin(fp248_t(fp248_t::PI_QUARTER)));
		printf("sin(fp248_t(PI/2)) =  1.00000 --> %d\n", sin(fp248_t(fp248_t::PI_HALF)));
		printf("sin(fp248_t(PI))   =  0.00000 --> %d\n", sin(fp248_t(fp248_t::PI)));
		printf("sin(fp248_t(2*PI)) =  0.00000 --> %d\n", sin(fp248_t(fp248_t::PI_2)));
		printf("cos(fp248_t(0))    =  1.00000 --> %d\n", cos(fp248_t(0)));
		printf("cos(fp248_t(1))    =  0.54030 --> %d\n", cos(fp248_t(1)));
		printf("cos(fp248_t(-1))   =  0.54030 --> %d\n", cos(fp248_t(-1)));
		printf("cos(fp248_t(PI/4)) =  0.70710 --> %d\n", cos(fp248_t(fp248_t::PI_QUARTER)));
		printf("cos(fp248_t(PI/2)) =  0.00000 --> %d\n", cos(fp248_t(fp248_t::PI_HALF)));
		printf("cos(fp248_t(PI))   = -1.00000 --> %d\n", cos(fp248_t(fp248_t::PI)));
		printf("cos(fp248_t(2*PI)) =  1.00000 --> %d\n", cos(fp248_t(fp248_t::PI_2)));
		printf("tan(fp248_t(0))    =  0.00000 --> %d\n", tan(fp248_t(0)));
		printf("tan(fp248_t(0.5))  =  0.54630 --> %d\n", tan(fp248_t(0.5f)));
		printf("tan(fp248_t(1))    =  1.55740 --> %d\n", tan(fp248_t(1)));
		printf("tan(fp248_t(-1))   = -1.55740 --> %d\n", tan(fp248_t(-1)));
		printf("tan(fp248_t(PI/4)) =  1.00000 --> %d\n", tan(fp248_t(fp248_t::PI_QUARTER)));
		printf("tan(fp248_t(PI/2)) =  8388608 --> %d\n", tan(fp248_t(fp248_t::PI_HALF)));
		printf("tan(fp248_t(PI))   =  0.00000 --> %d\n", tan(fp248_t(fp248_t::PI)));
		/*for (u32 i = 0; i < 1000; ++i) {
			for (u32 j = 0; j < 10; ++j) {
				print(random(), (u32)16);
			}
			print("\n");
		}*/
		/*Math::fp1616_t a = 3.7f;
		Math::fp1616_t b = 0;
		Math::fp1616_t start = Time::getTime();
		const int32_t nrOfLoops = 1000*1000;
		for (int32_t i = 0; i < nrOfLoops; ++i)
		{
			Math::fp1616_t s = sin(a);
			Math::fp1616_t c = cos(a);
			//sincos(a, s, c);
			b = s + c;
			a = -a;
		}
		Math::fp1616_t diff = Time::getTime() - start;
		printf("b = %d\n", b);
		printf("run time: %s %s\n", timingValue(diff, nrOfLoops), timingUnit(diff, nrOfLoops));
		*/
		/*Math::fp1616_t start = -Math::fp1616_t::PI_2;
		Math::fp1616_t step = (Math::fp1616_t::PI_2 * 2) / Video::width();
		Math::fp1616_t verticalCenter = Video::height() / 2;
		Math::fp1616_t halfHeight = (Video::height() - 2) / 2;
		for (int32_t x = 0; x < Video::width(); ++x)
		{
			Math::fp1616_t s;// = sin(start + step * x);
			Math::fp1616_t c;// = cos(start + step * x);
			sincos(start + step * x, s, c);
			Video::setPixel(Video::frontBuffer(), x, (uint32_t)(verticalCenter - halfHeight * s), (uint8_t)255); //red
			Video::setPixel(Video::frontBuffer(), x, (uint32_t)(verticalCenter - halfHeight * c), (uint8_t)1); //blue
		}*/
	}

} // namespace Test