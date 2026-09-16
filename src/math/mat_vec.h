#pragma once

// DON'T include these directly!!! No namespace here, because directly included in mat.h

/// @brief Transform point using world / camera matrix
/// Assumes v.w is 1
template <typename T>
vec3_t<T> transformPoint(const mat3x4_t<T> &m, const vec3_t<T> &v)
{
	vec4_t<T> result;
	result.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
	result.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
	result.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
	return result;
}

/// @brief Transform normal using transpose inverse of world / camera matrix
/// Assumes v.w is 0
template <typename T>
vec3_t<T> transformNormal(const mat3x3_t<T> &m, const vec3_t<T> &v)
{
	vec4_t<T> result;
	result.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
	result.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
	result.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
	return result;
}

/// @brief Transform point using view / projection / clip matrix
/// Assumes v.w is 1
template <typename T>
vec4_t<T> transformPoint(const mat4x4_t<T> &m, const vec3_t<T> &v)
{
	vec4_t<T> result;
	result.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
	result.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
	result.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
	result.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
	return result;
}

/// @brief Project point using view / projection / clip matrix
/// Assumes v.w is 1
template <typename T>
vec4_t<T> projectPoint(const mat4x4_t<T> &m, const vec3_t<T> &v)
{
	vec4_t<T> result;
	result.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
	result.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
	result.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];
	result.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
	return result;
}
