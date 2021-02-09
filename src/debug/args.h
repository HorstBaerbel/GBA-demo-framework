#pragma once

#include <utility>
#include <stdint.h>
#include "math/fp32.h"
#include "math/fp16.h"
#include "math/vec.h"
#include "output.h"

//--- formatted printing ------------------------------------------------------

template <typename PT>
struct PrintfArray
{
	const PT *ptr;
	uint16_t count;
	PrintfArray(const PT *_ptr, uint16_t _count) : ptr(_ptr), count(_count) {}
} __attribute__((packed));

class Arg
{
private:
	enum class Type : uint16_t
	{
		BOOL,
		INT8,
		UINT8,
		INT16,
		UINT16,
		INT32,
		UINT32,
		INT64,
		UINT64,
		FIXED_824,
		FIXED_1616,
		FIXED_248,
		FIXED_412,
		FIXED_88,
		FIXED_124,
		STRING
	};
	Type m_type;
	uint16_t m_count = 1;
	union
	{
		int32_t int32_value;
		uint32_t uint32_value;
		const void *p_value;
	} m_v0;
	union
	{
		int32_t int32_value;
		uint32_t uint32_value;
	} m_v1;

public:
	Arg(bool value);
	Arg(int8_t value);
	Arg(uint8_t value);
	Arg(int16_t value);
	Arg(uint16_t value);
	Arg(int32_t value);
	Arg(uint32_t value);
	Arg(int64_t value);
	Arg(uint64_t value);
	Arg(Math::fp32_t<24> value);
	Arg(Math::fp32_t<16> value);
	Arg(Math::fp32_t<8> value);
	Arg(Math::fp16_t<12> value);
	Arg(Math::fp16_t<8> value);
	Arg(Math::fp16_t<4> value);
	template <typename T, typename std::enable_if<sizeof(T) == 4, T>::type * = nullptr>
	Arg(void *value) : m_type(Type::UINT32)
	{
		m_v0.uint32_value = reinterpret_cast<uint32_t>(value);
	}
	template <typename T, typename std::enable_if<sizeof(T) == 8, T>::type * = nullptr>
	Arg(void *value) : m_type(Type::UINT32)
	{
		auto temp = reinterpret_cast<uint64_t>(value);
		m_v0.uint32_value = static_cast<uint32_t>(temp);
		m_v1.uint32_value = static_cast<uint32_t>(temp >> 32);
	}
	Arg(const PrintfArray<bool> &value);
	Arg(const PrintfArray<int8_t> &value);
	Arg(const PrintfArray<uint8_t> &value);
	Arg(const PrintfArray<int16_t> &value);
	Arg(const PrintfArray<uint16_t> &value);
	Arg(const PrintfArray<int32_t> &value);
	Arg(const PrintfArray<uint32_t> &value);
	Arg(const PrintfArray<int64_t> &value);
	Arg(const PrintfArray<uint64_t> &value);
	Arg(const PrintfArray<Math::fp824_t> &value);
	Arg(const PrintfArray<Math::fp1616_t> &value);
	Arg(const PrintfArray<Math::fp248_t> &value);
	Arg(const Math::vec2_t<Math::fp824_t> &value);
	Arg(const Math::vec2_t<Math::fp1616_t> &value);
	Arg(const Math::vec2_t<Math::fp248_t> &value);
	Arg(const Math::vec3_t<Math::fp824_t> &value);
	Arg(const Math::vec3_t<Math::fp1616_t> &value);
	Arg(const Math::vec3_t<Math::fp248_t> &value);
	Arg(const PrintfArray<Math::fp412_t> &value);
	Arg(const PrintfArray<Math::fp88_t> &value);
	Arg(const PrintfArray<Math::fp124_t> &value);
	Arg(const Math::vec2_t<Math::fp412_t> &value);
	Arg(const Math::vec2_t<Math::fp88_t> &value);
	Arg(const Math::vec2_t<Math::fp124_t> &value);
	Arg(const Math::vec3_t<Math::fp412_t> &value);
	Arg(const Math::vec3_t<Math::fp88_t> &value);
	Arg(const Math::vec3_t<Math::fp124_t> &value);
	Arg(const char *value);
	void do_print(const char format, uint32_t count, uint8_t &charsInBuffer) const;
	void do_print(const char format, uint8_t &charsInBuffer) const;
} __attribute__((aligned(4), packed));

void do_printf(const char *s, const Arg *args, uint8_t numArgs);
