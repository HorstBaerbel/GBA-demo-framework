#pragma once

//DON'T include these directly!!! No namespace here, because directly included in vfp32.h

template <typename T>
inline vec2_t<T> lerp(const vec2_t<T> & a, const vec2_t<T> & b, const T & t)
{
	vec2_t<T> result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	return result;
}

template <typename T>
inline vec3_t<T> lerp(const vec3_t<T> & a, const vec3_t<T> & b, const T & t)
{
	vec3_t<T> result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	return result;
}

template <typename T>
inline vec4_t<T> lerp(const vec4_t<T> & a, const vec4_t<T> & b, const T & t)
{
	vec4_t<T> result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	result.w = lerp(a.w, b.w, t);
	return result;
}