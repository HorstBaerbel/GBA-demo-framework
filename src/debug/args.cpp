#include "args.h"
#include "itoa.h"

#define PrintBufferSize 256
char printBuffer[PrintBufferSize] = {0}; //buffer used for conversions and printf formatting

void flushBuffer(uint8_t &charsInBuffer)
{
	printBuffer[charsInBuffer] = '\0';
	print(printBuffer);
	charsInBuffer = 0;
}

void printToBuffer(char c, uint8_t &charsInBuffer)
{
	printBuffer[charsInBuffer++] = c;
	printBuffer[charsInBuffer] = '\0';
}

void printToBuffer(const char *s, uint8_t &charsInBuffer, uint8_t length = 0)
{
	uint16_t count = 0;
	while ((length == 0 || length > count) && *s != '\0')
	{
		printBuffer[charsInBuffer++] = *s++;
		count++;
		if (charsInBuffer >= PrintBufferSize - 1)
		{
			flushBuffer(charsInBuffer);
		}
	}
	printBuffer[charsInBuffer] = '\0';
}

Arg::Arg(bool value) : m_type(Type::BOOL) { m_v0.uint32_value = value; }
Arg::Arg(int8_t value) : m_type(Type::INT8) { m_v0.int32_value = value; }
Arg::Arg(uint8_t value) : m_type(Type::UINT8) { m_v0.uint32_value = value; }
Arg::Arg(int16_t value) : m_type(Type::INT16) { m_v0.int32_value = value; }
Arg::Arg(uint16_t value) : m_type(Type::UINT16) { m_v0.uint32_value = value; }
Arg::Arg(int32_t value) : m_type(Type::INT32) { m_v0.int32_value = value; }
Arg::Arg(uint32_t value) : m_type(Type::UINT32) { m_v0.uint32_value = value; }
Arg::Arg(int64_t value) : m_type(Type::INT64)
{
	m_v0.int32_value = value;
	m_v1.int32_value = value >> 32;
}
Arg::Arg(uint64_t value) : m_type(Type::UINT64)
{
	m_v0.uint32_value = value;
	m_v1.uint32_value = value >> 32;
}
Arg::Arg(Math::fp824_t value) : m_type(Type::FIXED_824) { m_v0.int32_value = value.raw(); }
Arg::Arg(Math::fp1616_t value) : m_type(Type::FIXED_1616) { m_v0.int32_value = value.raw(); }
Arg::Arg(Math::fp248_t value) : m_type(Type::FIXED_248) { m_v0.int32_value = value.raw(); }
Arg::Arg(Math::fp412_t value) : m_type(Type::FIXED_412) { m_v0.int32_value = value.raw(); }
Arg::Arg(Math::fp88_t value) : m_type(Type::FIXED_88) { m_v0.int32_value = value.raw(); }
Arg::Arg(Math::fp124_t value) : m_type(Type::FIXED_124) { m_v0.int32_value = value.raw(); }
Arg::Arg(const PrintfArray<bool> &value) : m_type(Type::BOOL)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<int8_t> &value) : m_type(Type::INT8)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<uint8_t> &value) : m_type(Type::UINT8)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<int16_t> &value) : m_type(Type::INT16)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<uint16_t> &value) : m_type(Type::UINT16)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<int32_t> &value) : m_type(Type::INT32)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<uint32_t> &value) : m_type(Type::UINT32)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<int64_t> &value) : m_type(Type::INT64)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<uint64_t> &value) : m_type(Type::UINT64)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<Math::fp824_t> &value) : m_type(Type::FIXED_824)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<Math::fp1616_t> &value) : m_type(Type::FIXED_1616)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<Math::fp248_t> &value) : m_type(Type::FIXED_248)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<Math::fp412_t> &value) : m_type(Type::FIXED_412)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<Math::fp88_t> &value) : m_type(Type::FIXED_88)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const PrintfArray<Math::fp124_t> &value) : m_type(Type::FIXED_124)
{
	m_v0.p_value = value.ptr;
	m_count = value.count;
}
Arg::Arg(const Math::fp824vec2_t &value) : m_type(Type::FIXED_824)
{
	m_v0.p_value = &value;
	m_count = 2;
}
Arg::Arg(const Math::fp1616vec2_t &value) : m_type(Type::FIXED_1616)
{
	m_v0.p_value = &value;
	m_count = 2;
}
Arg::Arg(const Math::fp248vec2_t &value) : m_type(Type::FIXED_248)
{
	m_v0.p_value = &value;
	m_count = 2;
}
Arg::Arg(const Math::fp412vec2_t &value) : m_type(Type::FIXED_412)
{
	m_v0.p_value = &value;
	m_count = 2;
}
Arg::Arg(const Math::fp88vec2_t &value) : m_type(Type::FIXED_88)
{
	m_v0.p_value = &value;
	m_count = 2;
}
Arg::Arg(const Math::fp124vec2_t &value) : m_type(Type::FIXED_124)
{
	m_v0.p_value = &value;
	m_count = 2;
}
Arg::Arg(const Math::fp824vec3_t &value) : m_type(Type::FIXED_824)
{
	m_v0.p_value = &value;
	m_count = 3;
}
Arg::Arg(const Math::fp1616vec3_t &value) : m_type(Type::FIXED_1616)
{
	m_v0.p_value = &value;
	m_count = 3;
}
Arg::Arg(const Math::fp248vec3_t &value) : m_type(Type::FIXED_248)
{
	m_v0.p_value = &value;
	m_count = 3;
}
Arg::Arg(const Math::fp412vec3_t &value) : m_type(Type::FIXED_412)
{
	m_v0.p_value = &value;
	m_count = 3;
}
Arg::Arg(const Math::fp88vec3_t &value) : m_type(Type::FIXED_88)
{
	m_v0.p_value = &value;
	m_count = 3;
}
Arg::Arg(const Math::fp124vec3_t &value) : m_type(Type::FIXED_124)
{
	m_v0.p_value = &value;
	m_count = 3;
}
Arg::Arg(const char *value) : m_type(Type::STRING) { m_v0.p_value = value; }

void Arg::do_print(const char format, uint8_t &charsInBuffer) const
{
	if (m_count > 1)
	{
		printToBuffer('(', charsInBuffer);
		for (uint16_t i = 0; i < m_count; i++)
		{
			switch (m_type)
			{
			case Type::BOOL:
				printToBuffer(reinterpret_cast<const int32_t *>(m_v0.p_value)[i] != 0 ? "true" : "false", charsInBuffer);
				break;
			case Type::INT8:
				itoa(int32_t(reinterpret_cast<const int8_t *>(m_v0.p_value)[i]), &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			case Type::UINT8:
				itoa(uint32_t(reinterpret_cast<const uint8_t *>(m_v0.p_value)[i]), &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			case Type::INT16:
				itoa(int32_t(reinterpret_cast<const int16_t *>(m_v0.p_value)[i]), &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			case Type::UINT16:
				itoa(uint32_t(reinterpret_cast<const uint16_t *>(m_v0.p_value)[i]), &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			case Type::INT32:
				itoa(reinterpret_cast<const int32_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			case Type::UINT32:
				itoa(reinterpret_cast<const uint32_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			case Type::INT64:
			{
				itoa(reinterpret_cast<const int64_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			}
			case Type::UINT64:
			{
				itoa(reinterpret_cast<const uint64_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
				break;
			}
			case Type::FIXED_824:
				fptoa(reinterpret_cast<const int32_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], Math::fp824_t::BITSF);
				break;
			case Type::FIXED_1616:
				fptoa(reinterpret_cast<const int32_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], Math::fp1616_t::BITSF);
				break;
			case Type::FIXED_248:
				fptoa(reinterpret_cast<const int32_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], Math::fp248_t::BITSF);
				break;
			case Type::FIXED_412:
				fptoa(reinterpret_cast<const int16_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], Math::fp412_t::BITSF);
				break;
			case Type::FIXED_88:
				fptoa(reinterpret_cast<const int16_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], Math::fp88_t::BITSF);
				break;
			case Type::FIXED_124:
				fptoa(reinterpret_cast<const int16_t *>(m_v0.p_value)[i], &printBuffer[charsInBuffer], Math::fp124_t::BITSF);
				break;
			default:
				break;
			}
			// find new null-terminator
			auto newEnd = charsInBuffer;
			while (newEnd < PrintBufferSize && printBuffer[newEnd++] != '\0')
			{
			}
			charsInBuffer = newEnd - 1;
			printBuffer[charsInBuffer] = '\0';
			// print separator
			if (i < m_count - 1)
			{
				printToBuffer(',', charsInBuffer);
				// if the buffer is too full we need to flush
				if (charsInBuffer > PrintBufferSize - 10)
				{
					flushBuffer(charsInBuffer);
				}
			}
		}
		printToBuffer(')', charsInBuffer);
	}
	else
	{
		switch (m_type)
		{
		case Type::BOOL:
			printToBuffer(m_v0.uint32_value != 0 ? "true" : "false", charsInBuffer);
			break;
		case Type::INT8:
		case Type::INT16:
		case Type::INT32:
			itoa(m_v0.int32_value, &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
			break;
		case Type::UINT8:
		case Type::UINT16:
		case Type::UINT32:
			itoa(m_v0.uint32_value, &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
			break;
		case Type::INT64:
		{
			int64_t v = ((int64_t)m_v0.int32_value) | ((int64_t)m_v1.int32_value) << 32;
			itoa(v, &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
			break;
		}
		case Type::UINT64:
		{
			uint64_t v = ((int64_t)m_v0.uint32_value) | ((int64_t)m_v1.uint32_value) << 32;
			itoa(v, &printBuffer[charsInBuffer], format == 'x' ? 16 : 10);
			break;
		}
		case Type::FIXED_824:
			fptoa(m_v0.int32_value, &printBuffer[charsInBuffer], Math::fp824_t::BITSF);
			break;
		case Type::FIXED_1616:
			fptoa(m_v0.int32_value, &printBuffer[charsInBuffer], Math::fp1616_t::BITSF);
			break;
		case Type::FIXED_248:
			fptoa(m_v0.int32_value, &printBuffer[charsInBuffer], Math::fp248_t::BITSF);
			break;
		case Type::FIXED_412:
			fptoa(m_v0.int32_value, &printBuffer[charsInBuffer], Math::fp412_t::BITSF);
			break;
		case Type::FIXED_88:
			fptoa(m_v0.int32_value, &printBuffer[charsInBuffer], Math::fp88_t::BITSF);
			break;
		case Type::FIXED_124:
			fptoa(m_v0.int32_value, &printBuffer[charsInBuffer], Math::fp124_t::BITSF);
			break;
		case Type::STRING:
			printToBuffer(reinterpret_cast<const char *>(m_v0.p_value), charsInBuffer);
			break;
		default:
			break;
		}
	}
	// find new null-terminator
	auto newEnd = charsInBuffer;
	while (newEnd < PrintBufferSize && printBuffer[newEnd++] != '\0')
	{
	}
	charsInBuffer = newEnd - 1;
	printBuffer[charsInBuffer] = '\0';
}

void do_printf(const char *s, const Arg *args, uint8_t numArgs)
{
	uint8_t charsInBuffer = 0;
	uint8_t argIndex = 0;
	// step through characters
	while (*s)
	{
		// check if there's a % to print an argument
		if (*s == '%')
		{
			// argument, read next char
			++s;
			// check what argument type it is
			switch (*s)
			{
			case 'd':
			case 's':
			case 'x':
				// fetch argument, if any left and print
				if (argIndex < numArgs)
				{
					// if we want to print an array, use the next argument as the count
					auto &arg = args[argIndex++];
					arg.do_print(*s, charsInBuffer);
				}
				break;
			default:
				// unknown argument type, print char
				printToBuffer(*s, charsInBuffer);
				break;
			}
		}
		else
		{
			// simple character. print it
			printToBuffer(*s, charsInBuffer);
		}
		// step to next char
		++s;
	}
	// flush pending string
	flushBuffer(charsInBuffer);
}
