#pragma once

//DON'T include these directly!!! No namespace here, because directly included in vfp32.h

template <typename T>
vec2_t<T> operator+(const vec2_t<T> & a, const vec2_t<T> & b)
{
	vec2_t<T> result(a);
    result += b;
	return result;
}

template <typename T>
vec2_t<T> operator+(const vec2_t<T> & a, T b)
{
	vec2_t<T> result(a);
	result += b;
	return result;
}

template <typename T>
vec2_t<T> operator-(const vec2_t<T> & a, const vec2_t<T> & b)
{
	vec2_t<T> result(a);
	result -= b;
	return result;
}

template <typename T>
vec2_t<T> operator-(const vec2_t<T> & a, T b)
{
	vec2_t<T> result(a);
	result -= b;
	return result;
}

//Unary minus (invert)
template <typename T>
vec2_t<T> operator-(const vec2_t<T> & a)
{
	vec2_t<T> result;
	result.x = -a.x;
    result.y = -a.y;
	return result;
}

template <typename T>
vec2_t<T> operator*(const vec2_t<T> & a, const vec2_t<T> & b)
{
	vec2_t<T> result(a);
	result *= b;
	return result;
}

template <typename T>
vec2_t<T> operator*(const vec2_t<T> & a, T b)
{
	vec2_t<T> result(a);
    result *= b;
	return result;
}

template <typename T>
vec2_t<T> operator/(const vec2_t<T> & a, const vec2_t<T> & b)
{
	vec2_t<T> result(a);
	result /= b;
	return result;
}

template <typename T>
vec2_t<T> operator/(const vec2_t<T> & a, T b)
{
	vec2_t<T> result(a);
	result /= b;
	return result;
}

template <typename T>
vec2_t<T> operator<<(const vec2_t<T> & a, uint32_t shift)
{
	vec2_t<T> result(a);
	result <<= shift;
	return result;
}

template <typename T>
vec2_t<T> operator>>(const vec2_t<T> & a, uint32_t shift)
{
	vec2_t<T> result(a);
	result >>= shift;
	return result;
}
