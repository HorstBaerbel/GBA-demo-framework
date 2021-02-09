#pragma once

#include "fp32.h"
#include "memory/memory.h"
#include "vec.h"

namespace Math
{

template <typename T>
struct mat2x2_t
{
    constexpr static const unsigned COLUMNS = 2; // number of columns
    constexpr static const unsigned ROWS = 2; // number of rows

    static const mat2x2_t identity; // identity matrix
    static const mat2x2_t zero; // all-zero matrix

    vec2_t<T> m[ROWS];

    mat2x2_t() {}
    /*template <typename R>
		mat2x2_t(const mat2x2_t<R> & b) { *this = b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		mat2x2_t(int m11, int m12, int m21, int m22) : m({{m11, m12}, {m21, m22}}) {}
		mat2x2_t(int32_t m11, int32_t m12, int32_t m21, int32_t m22) : m({{m11, m12}, {m21, m22}}) {}
        mat2x2_t(T m11, T m12, T m21, T m22) : m({{m11, m12}, {m21, m22}}) {}
		mat2x2_t(float m11, float m12, float m21, float m22) : m({{m11, m12}, {m21, m22}}) {}
        constexpr mat2x2_t(std::initializer_list<T> l) : m(l) {}*/
    mat2x2_t(const vec2_t<T> &r1, const vec2_t<T> &r2)
        : m{r1, r2} {}

    mat2x2_t &operator=(const mat2x2_t &b)
    {
        Memory::memcpy32(&m, &b.m, sizeof(m) / 4);
        return *this;
    }

    inline const vec2_t<T> &operator[](unsigned int row) const { return m[row]; }
    inline vec2_t<T> &operator[](unsigned int row) { return m[row]; }

    mat2x2_t &operator+=(const mat2x2_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            ((T *)m)[i] += b.data()[i];
        }
        return *this;
    }
    mat2x2_t &operator+=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            ((T *)m)[i] += b;
        }
        return *this;
    }
    mat2x2_t &operator-=(const mat2x2_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            ((T *)m)[i] -= b.data()[i];
        }
        return *this;
    }
    mat2x2_t &operator-=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            ((T *)m)[i] -= b;
        }
        return *this;
    }
    mat2x2_t &operator*=(const mat2x2_t &b)
    {
        T *dst = reinterpret_cast<T *>(&m[0]);
        const T *bm = reinterpret_cast<const T *>(&b.m[0]);
        vec2_t<T> r0 = m[0];
        *dst++ = r0.x * bm[0] + r0.y * bm[2];
        *dst++ = r0.x * bm[1] + r0.y * bm[3];
        vec2_t<T> r1 = m[1];
        *dst++ = r1.x * bm[0] + r1.y * bm[2];
        *dst = r1.x * bm[1] + r1.y * bm[3];
        return *this;
    }
    mat2x2_t &operator*=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            ((T *)m)[i] *= b.data()[i];
        }
        return *this;
    }
    mat2x2_t &operator/=(const mat2x2_t &b)
    {
        *this *= b.inverse();
        return *this;
    }
    mat2x2_t &operator/=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            ((T *)m)[i] /= b.data()[i];
        }
        return *this;
    }

    friend bool operator==(const mat2x2_t &a, const mat2x2_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            if (a.data()[i] != b.data()[i])
                return false;
        }
        return true;
    }
    friend bool operator!=(const mat2x2_t &a, const mat2x2_t &b) { return !(a == b); }

    mat2x2_t &setIdentity()
    {
        *this = identity;
        return *this;
    }
    mat2x2_t &setZero()
    {
        *this = zero;
        return *this;
    }

    mat2x2_t &transpose()
    {
        std::swap(m[1], m[2]);
        return *this;
    }
    mat2x2_t transposed() const { return mat2x2_t({{m[0], m[2]}, {m[1], m[3]}}); }

    T determinant() const { return m[0] * m[3] - m[1] * m[2]; }
    mat2x2_t &invert()
    {
        T det = determinant();
        if (abs(det) <= T::EPSILON)
        {
            return identity(); // cannot invert, return identity matrix
        }
        T invDet = T::ONE / det;
        m[3] *= invDet;
        m[1] *= -invDet;
        m[2] *= -invDet;
        m[0] *= invDet;
        return *this;
    }
    mat2x2_t inverse() const
    {
        mat2x2_t tmp(*this);
        return tmp.invert();
    }

    mat2x2_t &setRotate(T rad)
    {
        T c;
        T s;
        sincos(rad, s, c);
        m = {c, -s, s, c};
        return *this;
    }
    static mat2x2_t fromRotation(T rad)
    {
        T c;
        T s;
        sincos(rad, s, c);
        return mat2x2_t(c, -s, s, c);
    }
    mat2x2_t &rotate(T rad)
    {
        *this *= fromRotation(rad);
        return *this;
    }

    mat2x2_t &setShear(T sx, T sy)
    {
        m = {1, sx, sy, 1};
        return *this;
    }
    static mat2x2_t fromShear(T sx, T sy) { return mat2x2_t(1, sx, sy, 1); }
    mat2x2_t &shear(T sx, T sy)
    {
        *this *= fromShear(sx, sy);
        return *this;
    }
} __attribute__((aligned(4), packed));

template <typename T>
struct mat3x3_t
{
    constexpr static const unsigned COLUMNS = 3; // number of columns
    constexpr static const unsigned ROWS = 3; // number of rows

    static const mat3x3_t identity; // identity matrix
    static const mat3x3_t zero; // all-zero matrix

    vec3_t<T> m[ROWS];

    mat3x3_t() {}
    /*template <typename R>
		mat3x3_t(const mat3x3_t<R> & b) { *this = b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		mat3x3_t(int m11, int m12, int m13, int m21, int m22, int m23, int m31, int m32, int m33) : m({m11, m12, m13, m21, m22, m23, m31, m32, m33}) {}
		mat3x3_t(int32_t m11, int32_t m12, int32_t m13, int32_t m21, int32_t m22, int32_t m23, int32_t m31, int32_t m32, int32_t m33) : m({m11, m12, m13, m21, m22, m23, m31, m32, m33}) {}
        mat3x3_t(T m11, T m12, T m13, T m21, T m22, T m23, T m31, T m32, T m33) : m({m11, m12, m13, m21, m22, m23, m31, m32, m33}) {}
		mat3x3_t(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33) : m({m11, m12, m13, m21, m22, m23, m31, m32, m33}) {}
        constexpr mat3x3_t(std::initializer_list<T> l) : m(l) {}*/
    mat3x3_t(const vec3_t<T> &r1, const vec3_t<T> &r2, const vec3_t<T> &r3)
        : m{r1, r2, r3} {}

    mat3x3_t &operator=(const mat3x3_t &b)
    {
        Memory::memcpy32(&m, &b.m, sizeof(m) / 4);
        return *this;
    }

    inline const vec3_t<T> &operator[](unsigned int row) const { return m[row]; }
    inline vec3_t<T> &operator[](unsigned int row) { return m[row]; }

    mat3x3_t &operator+=(const mat3x3_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] += b[i];
        }
        return *this;
    }
    mat3x3_t &operator+=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] += b;
        }
        return *this;
    }
    mat3x3_t &operator-=(const mat3x3_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] -= b[i];
        }
        return *this;
    }
    mat3x3_t &operator-=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] -= b;
        }
        return *this;
    }
    mat3x3_t &operator*=(const mat3x3_t &b)
    {
        T *dst = reinterpret_cast<T *>(&m[0]);
        const T *bm = reinterpret_cast<const T *>(&b.m[0]);
        for (int i = 0; i < 3; i++)
        {
            vec3_t<T> ri = m[i];
            for (int j = 0; j < 3; j++)
            {
                *dst++ = ri.x * bm[j] + ri.y * bm[3 + j] + ri.z * bm[6 + j];
            }
        }
        return *this;
    }
    mat3x3_t &operator*=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] *= b;
        }
        return *this;
    }
    mat3x3_t &operator/=(const mat3x3_t &b)
    {
        *this *= b.inverse();
        return *this;
    }
    mat3x3_t &operator/=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] /= b;
        }
        return *this;
    }

    friend bool operator==(const mat3x3_t &a, const mat3x3_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }
    friend bool operator!=(const mat3x3_t &a, const mat3x3_t &b) { return !(a == b); }

    mat3x3_t &setIdentity()
    {
        *this = identity;
        return *this;
    }
    mat3x3_t &setZero()
    {
        *this = zero;
        return *this;
    }

    mat3x3_t &transpose()
    {
        std::swap(m[1], m[3]);
        std::swap(m[2], m[6]);
        std::swap(m[5], m[7]);
        return *this;
    }
    mat3x3_t transposed() const { return mat3x3_t(m[0], m[3], m[6], m[1], m[4], m[7], m[2], m[5], m[8]); }

    T determinant() const { return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) + m[2] * (m[3] * m[7] - m[4] * m[6]); }
    mat3x3_t &invert()
    {
        T tmp[9];
        // check if determinant is zero
        tmp[0] = m[4] * m[8] - m[5] * m[7];
        tmp[3] = m[5] * m[6] - m[3] * m[8];
        tmp[6] = m[3] * m[7] - m[4] * m[6];
        T det = m[0] * tmp[0] + m[1] * tmp[3] + m[2] * tmp[6];
        if (abs(det) <= T::EPSILON)
        {
            return identity(); // cannot invert, return identity matrix
        }
        tmp[1] = m[2] * m[7] - m[1] * m[8];
        tmp[2] = m[1] * m[5] - m[2] * m[4];
        tmp[4] = m[0] * m[8] - m[2] * m[6];
        tmp[5] = m[2] * m[3] - m[0] * m[5];
        tmp[7] = m[1] * m[6] - m[0] * m[7];
        tmp[8] = m[0] * m[4] - m[1] * m[3];
        // divide by the determinant
        T invDet = T::ONE / det;
        m[0] = invDet * tmp[0];
        m[1] = invDet * tmp[1];
        m[2] = invDet * tmp[2];
        m[3] = invDet * tmp[3];
        m[4] = invDet * tmp[4];
        m[5] = invDet * tmp[5];
        m[6] = invDet * tmp[6];
        m[7] = invDet * tmp[7];
        m[8] = invDet * tmp[8];
        return *this;
    }
    mat3x3_t inverse() const
    {
        mat3x3_t tmp(*this);
        return tmp.invert();
    }
    mat3x3_t inverseTranspose() const
    {
        mat3x3_t tmp(*this);
        tmp.invert();
        tmp.transpose();
        return tmp;
    }
    mat3x3_t inverseOrthogonalUniformScale() const
    {
        T scale = m[0].norm();
        if (scale <= T::EPSILON)
        {
            return identity(); // cannot invert, return identity matrix
        }
        scale = 1 / scale;
        mat3x3_t tmp = transposed();
        tmp *= scale;
        return tmp;
    }
    mat3x3_t inverseTransposeOrthogonalUniformScale() const
    {
        T scale = m[0].norm();
        if (scale <= T::EPSILON)
        {
            return identity(); // cannot invert, return identity matrix
        }
        scale = 1 / scale;
        return (*this * scale);
    }
} __attribute__((aligned(4), packed));

template <typename T>
struct mat3x4_t
{
    constexpr static const unsigned COLUMNS = 4; // number of columns
    constexpr static const unsigned ROWS = 3; // number of rows

    static const mat3x4_t identity; // identity matrix
    static const mat3x4_t zero; // all-zero matrix

    vec4_t<T> m[ROWS];

    mat3x4_t() {}
    /*template <typename R>
		mat3x4_t(const mat3x4_t<R> & b) { *this = b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		mat3x4_t(int m11, int m12, int m13, int m14, int m21, int m22, int m23, int m24, int m31, int m32, int m33, int m34) : m({{m11, m12, m13, m14}, {m21, m22, m23, m24}, {m31, m32, m33, m34}}) {}
		mat3x4_t(int32_t m11, int32_t m12, int32_t m13, int32_t m14, int32_t m21, int32_t m22, int32_t m23, int32_t m24, int32_t m31, int32_t m32, int32_t m33, int32_t m34) : m({{m11, m12, m13, m14}, {m21, m22, m23, m24}, {m31, m32, m33, m34}}) {}
        mat3x4_t(T m11, T m12, T m13, T m14, T m21, T m22, T m23, T m24, T m31, T m32, T m33, T m34) : m({{m11, m12, m13, m14}, {m21, m22, m23, m24}, {m31, m32, m33, m34}}) {}
		mat3x4_t(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34) : m({{m11, m12, m13, m14}, {m21, m22, m23, m24}, {m31, m32, m33, m34}}) {}*/
    mat3x4_t(const vec4_t<T> &r1, const vec4_t<T> &r2, const vec4_t<T> &r3)
        : m{r1, r2, r3} {}
    mat3x4_t(const mat3x3_t<T> &r)
        : m{{r.m[0].x, r.m[0].y, r.m[0].z, 0}, {r.m[1].x, r.m[1].y, r.m[1].z, 0}, {r.m[2].x, r.m[2].y, r.m[2].z, 0}} {}

    mat3x4_t &operator=(const mat3x4_t &b)
    {
        Memory::memcpy32(&m, &b.m, sizeof(m) / 4);
        return *this;
    }

    explicit operator mat3x3_t<T>() const { return mat3x3_t<T>({m[0].x, m[0].y, m[0].z, m[1].x, m[1].y, m[1].z, m[2].x, m[2].y, m[2].z}); }

    inline const vec4_t<T> &operator[](unsigned int row) const { return m[row]; }
    inline vec4_t<T> &operator[](unsigned int row) { return m[row]; }

    mat3x4_t &operator+=(const mat3x4_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] += b[i];
        }
        return *this;
    }
    mat3x4_t &operator+=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] += b;
        }
        return *this;
    }
    mat3x4_t &operator-=(const mat3x4_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] -= b[i];
        }
        return *this;
    }
    mat3x4_t &operator-=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] -= b;
        }
        return *this;
    }
    mat3x4_t &operator*=(const mat3x4_t &b)
    {
        T *dst = reinterpret_cast<T *>(&m[0]);
        const T *bm = reinterpret_cast<const T *>(&b.m[0]);
        for (int i = 0; i < 3; i++)
        {
            vec4_t<T> ri = m[i]; // copy the row from m that we're processing. we're overwriting it...
            for (int j = 0; j < 3; j++)
            {
                *dst++ = ri.x * bm[j] + ri.y * bm[4 + j] + ri.z * bm[8 + j];
            }
            *dst++ = ri.x * bm[3] + ri.y * bm[4 + 3] + ri.z * bm[8 + 3] + ri.w;
        }
        return *this;
    }
    mat3x4_t &operator*=(const mat3x3_t<T> &b)
    {
        T *dst = reinterpret_cast<T *>(&m[0]);
        const T *bm = reinterpret_cast<const T *>(&b.m[0]);
        for (int i = 0; i < 3; i++)
        {
            vec4_t<T> ri = m[i]; // copy the row from m that we're processing. we're overwriting it...
            for (int j = 0; j < 3; j++)
            {
                *dst++ = ri.x * bm[j] + ri.y * bm[3 + j] + ri.z * bm[6 + j];
            }
            dst++; // the 4th entry / ri.w stays the same
        }
        return *this;
    }
    mat3x4_t &operator*=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] *= b;
        }
        return *this;
    }
    mat3x4_t &operator/=(const mat3x4_t &b)
    {
        *this *= b.inverse();
        return *this;
    }
    mat3x4_t &operator/=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] /= b;
        }
        return *this;
    }

    friend bool operator==(const mat3x4_t &a, const mat3x4_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }
    friend bool operator!=(const mat3x4_t &a, const mat3x4_t &b) { return !(a == b); }

    mat3x4_t &setIdentity()
    {
        *this = identity;
        return *this;
    }
    mat3x4_t &setZero()
    {
        *this = zero;
        return *this;
    }

    T determinant() const { return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) + m[2] * (m[3] * m[7] - m[4] * m[6]); }

    mat3x3_t<T> inverseOrthogonalUniformScale() const
    {
        T scale = m[0].norm();
        if (scale <= T::EPSILON)
        {
            return identity(); // cannot invert, return identity matrix
        }
        scale = 1 / scale;
        mat3x3_t<T> tmp = (mat3x3_t<T>(*this)).transposed();
        tmp *= scale;
        return tmp;
    }
    mat3x4_t inverseTransposeOrthogonalUniformScale() const
    {
        T scale = m[0].norm();
        if (scale <= T::EPSILON)
        {
            return identity(); // cannot invert, return identity matrix
        }
        scale = 1 / scale;
        return (*this * scale);
    }

    static mat3x4_t fromTranslate(const vec3_t<T> &t) { return mat3x4_t({1, 0, 0, t.x}, {0, 1, 0, t.y}, {0, 0, 1, t.z}); }
    static mat3x4_t fromRotate(const mat3x3_t<T> &r) { return mat3x4_t(r); }
    static mat3x4_t fromScale(T s) { return mat3x4_t({s, 0, 0, 0}, {0, s, 0, 0}, {0, 0, s, 0}); }
    static mat3x4_t fromTRS(const vec3_t<T> &t, const mat3x3_t<T> &r, T s) { return mat3x4_t(fromTranslate(t) * fromRotate(r) * fromScale(s)); }
} __attribute__((aligned(4), packed));

template <typename T>
struct mat4x4_t
{
    constexpr static const unsigned COLUMNS = 4; // number of columns
    constexpr static const unsigned ROWS = 4; // number of rows

    static const mat4x4_t identity; // identity matrix
    static const mat4x4_t zero; // all-zero matrix

    vec4_t<T> m[ROWS];

    mat4x4_t() {}
    /*template <typename R>
		mat4x4_t(const mat4x4_t<R> & b) { *this = b; }
		template <class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		mat4x4_t(int m11, int m12, int m13, int m14, int m21, int m22, int m23, int m24, int m31, int m32, int m33, int m34, int m41, int m42, int m43, int m44) : m({m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44}) {}
		mat4x4_t(int32_t m11, int32_t m12, int32_t m13, int32_t m14, int32_t m21, int32_t m22, int32_t m23, int32_t m24, int32_t m31, int32_t m32, int32_t m33, int32_t m34, int32_t m41, int32_t m42, int32_t m43, int32_t m44) : m({m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44}) {}
        mat4x4_t(T m11, T m12, T m13, T m14, T m21, T m22, T m23, T m24, T m31, T m32, T m33, T m34, T m41, T m42, T m43, T m44) : m({m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44}) {}
		mat4x4_t(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) : m({m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44}) {}
        constexpr mat4x4_t(std::initializer_list<T> l) : m(l) {}*/
    mat4x4_t(const vec4_t<T> &r1, const vec4_t<T> &r2, const vec4_t<T> &r3, const vec4_t<T> &r4)
        : m{r1, r2, r3, r4} {}

    mat4x4_t &operator=(const mat4x4_t &b)
    {
        Memory::memcpy32(&m, &b.m, sizeof(m) / 4);
        return *this;
    }

    inline const vec4_t<T> &operator[](unsigned int row) const { return m[row]; }
    inline vec4_t<T> &operator[](unsigned int row) { return m[row]; }

    mat4x4_t &operator+=(const mat4x4_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] += b[i];
        }
        return *this;
    }
    mat4x4_t &operator+=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] += b;
        }
        return *this;
    }
    mat4x4_t &operator-=(const mat4x4_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] -= b[i];
        }
        return *this;
    }
    mat4x4_t &operator-=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] -= b;
        }
        return *this;
    }
    mat4x4_t &operator*=(const mat4x4_t &b)
    {
        T *srcdst = &m;
        const T *bm = &b.m;
        for (int i = 0; i < 4; i++)
        {
            vec4_t<T> ri = m[i];
            for (int j = 0; j < 4; j++)
            {
                *srcdst++ = ri.x * bm[j] + ri.y * bm[4 + j] + ri.z * bm[8 + j] + ri.w * bm[12 + j];
            }
        }
        return *this;
    }
    mat4x4_t &operator*=(const mat3x4_t<T> &b)
    {
        T *srcdst = &m;
        const T *bm = &b.m;
        for (int i = 0; i < 4; i++)
        {
            const vec4_t<T> &ri = m[i];
            for (int j = 0; j < 3; j++)
            {
                *srcdst++ = ri.x * bm[j] + ri.y * bm[4 + j] + ri.z * bm[8 + j];
            }
            *srcdst++ = ri.x * bm[3] + ri.y * bm[7] + ri.z * bm[11] + ri.w;
        }
        return *this;
    }
    mat4x4_t &operator*=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] *= b;
        }
        return *this;
    }
    mat4x4_t &operator/=(const mat4x4_t &b)
    {
        *this *= b.inverse();
        return *this;
    }
    mat4x4_t &operator/=(T b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            m[i] /= b;
        }
        return *this;
    }

    friend bool operator==(const mat4x4_t &a, const mat4x4_t &b)
    {
        for (unsigned int i = 0; i < ROWS * COLUMNS; ++i)
        {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }
    friend bool operator!=(const mat4x4_t &a, const mat4x4_t &b) { return !(a == b); }

    mat4x4_t &setIdentity()
    {
        *this = identity;
        return *this;
    }
    mat4x4_t &setZero()
    {
        *this = zero;
        return *this;
    }

    mat4x4_t &transpose()
    {
        std::swap(m[1], m[4]);
        std::swap(m[2], m[8]);
        std::swap(m[3], m[12]);
        std::swap(m[6], m[9]);
        std::swap(m[7], m[13]);
        std::swap(m[11], m[14]);
        return *this;
    }
    mat4x4_t transposed() const { return mat4x4_t(m[0], m[4], m[8], m[12], m[1], m[5], m[9], m[13], m[2], m[6], m[10], m[14], m[3], m[7], m[11], m[15]); }

    T determinant() const { return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) + m[2] * (m[3] * m[7] - m[4] * m[6]); }
    mat4x4_t &invert()
    {
        return *this;
    }
    mat4x4_t inverse() const
    {
        mat4x4_t tmp(*this);
        return tmp.invert();
    }
} __attribute__((aligned(4), packed));

using fp1616mat2x2_t = mat2x2_t<fp1616_t>; //2x2 16.16 matrix (2D rotation)
using fp1616mat3x3_t = mat3x3_t<fp1616_t>; //3x3 16.16 matrix (3D rotation / normal matrix)
using fp1616mat3x4_t = mat3x4_t<fp1616_t>; //3x4 16.16 matrix (3D world transform)
using fp1616mat4x4_t = mat4x4_t<fp1616_t>; //4x4 16.16 matrix (3D perspective transform)

#include "mat_arith.h"
#include "mat_vec.h"

} // namespace Math
