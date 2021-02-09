#pragma once

#include "fp16.h"
#include "fp32.h"
#include <type_traits>

namespace Math
{

    template <typename T>
    struct vec2_t
    {
        union
        {
            struct
            {
                T x;
                T y;
            } __attribute__((aligned (4), packed));
            T v[2];
        } __attribute__((aligned (4), packed));
        
        constexpr vec2_t() {  }
		constexpr vec2_t(T x, T y) : x(x), y(y) {}
		constexpr vec2_t(const vec2_t & b) : x(b.x), y(b.y) {}
		/*template<typename B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr vec2_t(int _x, int _y) : x(_x), y(_y) {}
        template<typename B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr vec2_t(int _x, int32_t _y) : x(_x), y(_y) {}
        template<typename B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr vec2_t(int32_t _x, int _y) : x(_x), y(_y) {}
		constexpr vec2_t(int32_t _x, int32_t _y) : x(_x), y(_y) {}
        constexpr vec2_t(float _x, float _y) : x(_x), y(_y) {}
        constexpr vec2_t(std::initializer_list<T> l) : v(l) {}
        constexpr vec2_t(std::initializer_list<int32_t> l) : v(l) {}
        template<typename B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
        constexpr vec2_t(std::initializer_list<int> l) : v(l) {}*/
        
		vec2_t & operator=(vec2_t b) { x = b.x; y = b.y; return *this; }

        inline const T & operator[](unsigned int col) const { return v[col]; }
        inline T & operator[](unsigned int col) { return v[col]; }

        vec2_t & operator+=(const vec2_t & b) { x += b.x; y += b.y; return *this; }
        vec2_t & operator+=(T b) { x += b; y += b; return *this; }
        vec2_t & operator-=(const vec2_t & b) { x -= b.x; y -= b.y; return *this; }
        vec2_t & operator-=(T b) { x -= b; y -= b; return *this; }
        vec2_t & operator*=(const vec2_t & b) { x *= b.x; y *= b.y; return *this; }
        vec2_t & operator*=(T b) { x *= b; y *= b; return *this; }
        vec2_t & operator/=(const vec2_t & b) { x /= b.x; y /= b.y; return *this; }
        vec2_t & operator/=(T b) { x /= b; y /= b; return *this; }
        vec2_t & operator<<=(uint32_t shift) { x.value <<= shift; y.value <<= shift; }
        vec2_t & operator>>=(uint32_t shift) { x.value >>= shift; y.value >>= shift; }

        friend bool operator==(const vec2_t<T> & a, const vec2_t<T> & b) { return a.x == b.x && a.y == b.y; }
		friend bool operator!=(const vec2_t<T> & a, const vec2_t<T> & b) { return !(a == b); }

        T norm() const { return (x*x + y*y); }
        T length() const { return sqrt(x*x + y*y); }
        vec2_t & normalize() { const T l = sqrt(x*x + y*y); x /= l; y /= l; return *this; }
        T normalizeAndReturnLength() { const T l = sqrt(x*x + y*y); x /= l; y /= l; return l; }
        vec2_t normalized() const { const T l = sqrt(x*x + y*y); return *this / l; }

        T dot(const vec2_t & b) const { return T(x * b.x + y * b.y); }
        T cross(const vec2_t & b) const { return x * b.y - y * b.x; }
        vec2_t perp() const { return vec2_t(y, -x); }
    } __attribute__((aligned (4), packed));

    using fp412vec2_t = vec2_t<fp412_t>; //4.12
    using fp88vec2_t = vec2_t<fp88_t>; //8.8
    using fp124vec2_t = vec2_t<fp124_t>; //12.4
    using fp824vec2_t = vec2_t<fp824_t>; //8.24
    using fp1616vec2_t = vec2_t<fp1616_t>; //16.16
    using fp248vec2_t = vec2_t<fp248_t>; //24.8

    template <typename T>
    struct vec3_t
    {
        union
        {
            struct
            {
                T x;
                T y;
                T z;
            } __attribute__((aligned (4), packed));
            T v[3];
        } __attribute__((aligned (4), packed));
        
        constexpr vec3_t() {  }
        constexpr vec3_t(T x, T y, T z) : x(x), y(y), z(z) {}
		constexpr vec3_t(const vec3_t & b) : x(b.x), y(b.y), z(b.z) {}
		/*template<class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr vec3_t(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
		constexpr vec3_t(int32_t _x, int32_t _y, int32_t _z) : x(_x), y(_y), z(_z) {}
        constexpr vec3_t(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        constexpr vec3_t(std::initializer_list<T> l) : v(l) {}*/
        
		vec3_t & operator=(vec3_t b) { x = b.x; y = b.y; z = b.z; return *this; }

        explicit inline operator vec2_t<T>() const { return vec2_t<T>(x, y); }

        inline const T & operator[](unsigned int col) const { return v[col]; }
        inline T & operator[](unsigned int col) { return v[col]; }

        vec3_t & operator+=(const vec3_t & b) { x += b.x; y += b.y; z += b.z; return *this; }
        vec3_t & operator+=(T b) { x += b; y += b; z += b; return *this; }
        vec3_t & operator-=(const vec3_t & b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
        vec3_t & operator-=(T b) { x -= b; y -= b; z -= b; return *this; }
		vec3_t & operator*=(const vec3_t & b) { x *= b.x; y *= b.y; z *= b.z; return *this; }
        vec3_t & operator*=(T b) { x *= b; y *= b; z *= b; return *this; }
        vec3_t & operator/=(const vec3_t & b) { x /= b.x; y /= b.y; z /= b.z; return *this; }
        vec3_t & operator/=(T b) { x /= b; y /= b; z /= b; return *this; }
        vec3_t & operator<<=(uint32_t shift) { x.value <<= shift; y.value <<= shift; z.value <<= shift; return *this; }
        vec3_t & operator>>=(uint32_t shift) { x.value >>= shift; y.value >>= shift; z.value >>= shift; return *this; }

        friend bool operator==(const vec3_t<T> & a, const vec3_t<T> & b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
		friend bool operator!=(const vec3_t<T> & a, const vec3_t<T> & b) { return !(a == b); }

        T norm() const { return (x*x + y*y + z*z); }
        T length() const { return sqrt(x*x + y*y + z*z); }
        vec3_t & normalize() { const T l1 = sqrtRecip(x*x + y*y + z*z); x *= l1; y *= l1; z *= l1; return *this; }
        T normalizeAndReturnLength() { const T l1 = sqrtRecip(x*x + y*y + z*z); x *= l1; y *= l1; return recip(l1); }
        vec3_t normalized() const { const T l1 = sqrtRecip(x*x + y*y + z*z); return *this * l1; }

        T dot(const vec3_t<T> & b) const { return T(x * b.x + y * b.y + z * b.z); }
        T dot(const vec2_t<T> & b) const { return T(x * b.x + y * b.y/* + z * b.z*/); }
        vec3_t cross(const vec3_t & b) const { return vec3_t(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x); }

    private:
        template <typename B = T, typename Enable = void>
        struct _dummy_for_aligment;

        template <typename B>
        struct _dummy_for_aligment<B, std::enable_if<B::BITS == 16>>
        {
            B _ignore; // for alignment to 4
        };

        template <typename B>
        struct _dummy_for_aligment<B, std::enable_if<B::BITS == 32>>
        {
        };
    } __attribute__((aligned (4), packed));

    using fp412vec3_t = vec3_t<fp412_t>; //4.12
    using fp88vec3_t = vec3_t<fp88_t>; //8.8
    using fp124vec3_t = vec3_t<fp124_t>; //12.4
    using fp824vec3_t = vec3_t<fp824_t>; //8.24
    using fp1616vec3_t = vec3_t<fp1616_t>; //16.16
    using fp248vec3_t = vec3_t<fp248_t>; //24.8

    template <typename T>
    struct vec4_t
    {
        union
        {
            struct
            {
                T x;
                T y;
                T z;
                T w;
            } __attribute__((aligned (4), packed));
            T v[4];
        } __attribute__((aligned (4), packed));
        
        constexpr vec4_t() {  }
        constexpr vec4_t(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
		constexpr vec4_t(const vec4_t & b) : x(b.x), y(b.y), z(b.z), w(b.w) {}
		/*template<class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr vec4_t(int _x, int _y, int _z, int _w) : x(_x), y(_y), z(_z), w(_w) {}
		constexpr vec4_t(int32_t _x, int32_t _y, int32_t _z, int32_t _w) : x(_x), y(_y), z(_z), w(_w) {}
        constexpr vec4_t(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
        constexpr vec4_t(std::initializer_list<T> l) : v(l) {}*/

		vec4_t & operator=(vec4_t b) { x = b.x; y = b.y; z = b.z; w = b.w; return *this; }

        explicit inline operator vec3_t<T>() const { return vec3_t<T>(x, y, z); }

        inline const T & operator[](unsigned int col) const { return v[col]; }
        inline T & operator[](unsigned int col) { return v[col]; }

        vec4_t & operator+=(const vec4_t & b) { x += b.x; y += b.y; z += b.z; w += b.w; return *this; }
        vec4_t & operator+=(T b) { x += b; y += b; z += b; w += b; return *this; }
        vec4_t & operator-=(const vec4_t & b) { x -= b.x; y -= b.y; z -= b.z; w -= b.w; return *this; }
        vec4_t & operator-=(T b) { x -= b; y -= b; z -= b; w -= b; return *this; }
        vec4_t & operator*=(const vec4_t & b) { x *= b.x; y *= b.y; z *= b.z; w *= b.w; return *this; }
        vec4_t & operator*=(T b) { x *= b; y *= b; z *= b; w *= b; return *this; }
        vec4_t & operator/=(const vec4_t & b) { x /= b.x; y /= b.y; z /= b.z; w /= b.w; return *this; }
        vec4_t & operator/=(T b) { x /= b; y /= b; z /= b; w /= b; return *this; }
        vec4_t & operator<<=(uint32_t shift) { x.value <<= shift; y.value <<= shift; z.value <<= shift; w.value <<= shift; return *this; }
        vec4_t & operator>>=(uint32_t shift) { x.value >>= shift; y.value >>= shift; z.value >>= shift; w.value >>= shift; return *this; }

		friend bool operator==(const vec4_t<T> & a, const vec4_t<T> & b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
		friend bool operator!=(const vec4_t<T> & a, const vec4_t<T> & b) { return !(a == b); }

        T norm() const { return (x*x + y*y + z*z + w*w); }
        T length() const { return sqrt(x*x + y*y + z*z + w*w); }
        vec4_t & normalize() { const T l1 = sqrt_recip(x*x + y*y + z*z + w*w); x *= l1; y *= l1; z *= l1; w *= l1; return *this; }
        T normalizeAndReturnLength() { const T l1 = sqrt_recip(x*x + y*y + z*z + w*w); x *= l1; y *= l1; return recip(l1); }
        vec4_t normalized() const { const T l1 = sqrt_recip(x*x + y*y + z*z + w*w); return *this * l1; }

        T dot(const vec4_t<T> & b) const { return T(x * b.x + y * b.y + z * b.z + w * b.w); }
        T dot(const vec3_t<T> & b) const { return T(x * b.x + y * b.y + z * b.z/* + w * b.w*/); }
    } __attribute__((aligned (4), packed));

    using fp412vec4_t = vec4_t<fp412_t>; //4.12
    using fp88vec4_t = vec4_t<fp88_t>; //8.8
    using fp124vec4_t = vec4_t<fp124_t>; //12.4
    using fp824vec4_t = vec4_t<fp824_t>; //8.24
    using fp1616vec4_t = vec4_t<fp1616_t>; //16.16
    using fp248vec4_t = vec4_t<fp248_t>; //24.8
	
	#include "vec_lerp.h"
    #include "vec_arith.h"

}
