#pragma once

//DON'T include these directly!!! No namespace here, because directly included in mat.h

template <typename T>
mat2x2_t<T> operator+(const mat2x2_t<T> & a, const mat2x2_t<T> & b)
{
	mat2x2_t<T> result(a);
    result += b;
	return result;
}

template <typename T>
mat2x2_t<T> operator+(const mat2x2_t<T> & a, T b)
{
	mat2x2_t<T> result(a);
	result += b;
	return result;
}

template <typename T>
mat2x2_t<T> operator-(const mat2x2_t<T> & a, const mat2x2_t<T> & b)
{
	mat2x2_t<T> result(a);
    result -= b;
	return result;
}

template <typename T>
mat2x2_t<T> operator-(const mat2x2_t<T> & a, T b)
{
	mat2x2_t<T> result(a);
	result -= b;
	return result;
}

template <typename T>
mat2x2_t<T> operator*(const mat2x2_t<T> & a, const mat2x2_t<T> & b)
{
	mat2x2_t<T> result(a);
    result *= b;
	return result;
}

template <typename T>
mat2x2_t<T> operator*(const mat2x2_t<T> & a, T b)
{
	mat2x2_t<T> result(a);
	result *= b;
	return result;
}

template <typename T>
mat2x2_t<T> operator/(const mat2x2_t<T> & a, const mat2x2_t<T> & b)
{
	mat2x2_t<T> result(a);
    result /= b;
	return result;
}

template <typename T>
mat2x2_t<T> operator/(const mat2x2_t<T> & a, T b)
{
	mat2x2_t<T> result(a);
	result /= b;
	return result;
}

template <typename T>
vec2_t<T> operator*(const mat2x2_t<T> & mat, const vec2_t<T> & v)
{
	return vec2_t<T>(mat.m[0].x * v.x + mat.m[1].x * v.y, mat.m[0].y * v.x + mat.m[1].y * v.y);
}

template <typename T>
mat3x3_t<T> operator+(const mat3x3_t<T> & a, const mat3x3_t<T> & b)
{
	mat3x3_t<T> result(a);
    result += b;
	return result;
}

template <typename T>
mat3x3_t<T> operator+(const mat3x3_t<T> & a, T b)
{
	mat3x3_t<T> result(a);
	result += b;
	return result;
}

template <typename T>
mat3x3_t<T> operator-(const mat3x3_t<T> & a, const mat3x3_t<T> & b)
{
	mat3x3_t<T> result(a);
    result -= b;
	return result;
}

template <typename T>
mat3x3_t<T> operator-(const mat3x3_t<T> & a, T b)
{
	mat3x3_t<T> result(a);
	result -= b;
	return result;
}

template <typename T>
mat3x3_t<T> operator*(const mat3x3_t<T> & a, const mat3x3_t<T> & b)
{
	mat3x3_t<T> result(a);
    result *= b;
	return result;
}

template <typename T>
mat3x3_t<T> operator*(const mat3x3_t<T> & a, T b)
{
	mat3x3_t<T> result(a);
	result *= b;
	return result;
}

template <typename T>
mat3x3_t<T> operator/(const mat3x3_t<T> & a, const mat3x3_t<T> & b)
{
	mat3x3_t<T> result(a);
    result /= b;
	return result;
}

template <typename T>
mat3x3_t<T> operator/(const mat3x3_t<T> & a, T b)
{
	mat3x3_t<T> result(a);
	result /= b;
	return result;
}

template <typename T>
vec3_t<T> operator*(const mat3x3_t<T> & mat, const vec3_t<T> & v)
{
	return vec3_t<T>(mat.m[0].dot(v), mat.m[1].dot(v), mat.m[2].dot(v));
}

template <typename T>
vec4_t<T> operator*(const mat3x3_t<T> & mat, const vec4_t<T> & v)
{
	return vec4_t<T>(mat.m[0].dot(v), mat.m[1].dot(v), mat.m[2].dot(v), v.w);
}

template <typename T>
mat3x4_t<T> operator+(const mat3x4_t<T> & a, const mat3x4_t<T> & b)
{
	mat3x4_t<T> result(a);
    result += b;
	return result;
}

template <typename T>
mat3x4_t<T> operator+(const mat3x4_t<T> & a, T b)
{
	mat3x4_t<T> result(a);
	result += b;
	return result;
}

template <typename T>
mat3x4_t<T> operator-(const mat3x4_t<T> & a, const mat3x4_t<T> & b)
{
	mat3x4_t<T> result(a);
    result -= b;
	return result;
}

template <typename T>
mat3x4_t<T> operator-(const mat3x4_t<T> & a, T b)
{
	mat3x4_t<T> result(a);
	result -= b;
	return result;
}

template <typename T>
mat3x4_t<T> operator*(const mat3x4_t<T> & a, const mat3x4_t<T> & b)
{
	mat3x4_t<T> result(a);
    result *= b;
	return result;
}

template <typename T>
mat3x4_t<T> operator*(const mat3x4_t<T> & a, T b)
{
	mat3x4_t<T> result(a);
	result *= b;
	return result;
}

template <typename T>
mat3x4_t<T> operator/(const mat3x4_t<T> & a, const mat3x4_t<T> & b)
{
	mat3x4_t<T> result(a);
    result /= b;
	return result;
}

template <typename T>
mat3x4_t<T> operator/(const mat3x4_t<T> & a, T b)
{
	mat3x4_t<T> result(a);
	result /= b;
	return result;
}

template <typename T>
vec3_t<T> operator*(const mat3x4_t<T> & mat, const vec3_t<T> & v)
{
	return vec3_t<T>(mat.m[0].dot(v), mat.m[1].dot(v), mat.m[2].dot(v));
}

template <typename T>
mat4x4_t<T> operator+(const mat4x4_t<T> & a, const mat4x4_t<T> & b)
{
	mat4x4_t<T> result(a);
    result += b;
	return result;
}

template <typename T>
mat4x4_t<T> operator+(const mat4x4_t<T> & a, T b)
{
	mat4x4_t<T> result(a);
	result += b;
	return result;
}

template <typename T>
mat4x4_t<T> operator-(const mat4x4_t<T> & a, const mat4x4_t<T> & b)
{
	mat4x4_t<T> result(a);
    result -= b;
	return result;
}

template <typename T>
mat4x4_t<T> operator-(const mat4x4_t<T> & a, T b)
{
	mat4x4_t<T> result(a);
	result -= b;
	return result;
}

template <typename T>
mat4x4_t<T> operator*(const mat4x4_t<T> & a, const mat4x4_t<T> & b)
{
	mat4x4_t<T> result(a);
    result *= b;
	return result;
}

template <typename T>
mat4x4_t<T> operator*(const mat4x4_t<T> & a, T b)
{
	mat4x4_t<T> result(a);
	result *= b;
	return result;
}

template <typename T>
mat4x4_t<T> operator/(const mat4x4_t<T> & a, const mat4x4_t<T> & b)
{
	mat4x4_t<T> result(a);
    result /= b;
	return result;
}

template <typename T>
mat4x4_t<T> operator/(const mat4x4_t<T> & a, T b)
{
	mat4x4_t<T> result(a);
	result /= b;
	return result;
}

template <typename T>
vec4_t<T> operator*(const mat3x4_t<T> & mat, const vec4_t<T> & v)
{
	return vec4_t<T>(mat.m[0].dot(v), mat.m[1].dot(v), mat.m[2].dot(v), v.w);
}
