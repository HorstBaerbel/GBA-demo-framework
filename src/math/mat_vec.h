#pragma once

//DON'T include these directly!!! No namespace here, because directly included in mat.h

template <typename T>
vec3_t<T> transformPos(const mat3x4_t<T> &m, const vec3_t<T> &v)
{
	return vec4_t<T>();
}

template <typename T>
vec3_t<T> transformDir(const mat3x4_t<T> &m, const vec3_t<T> &v)
{
	return vec4_t<T>();
}
