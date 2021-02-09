#pragma once

#include "fp32.h"
#include "vec.h"
#include "mat.h"

namespace Math
{

    template <typename T>
    struct quat_t
    {
        vec4_t<T> q;

        constexpr quat_t() {}
        constexpr quat_t(T x, T y, T z, T w) : q({x, y, z, w}) {}
        /*template <typename R>
		constexpr quat_t(const quat_t<R> & b) : q(b.q) {}
		template<class B = int, typename std::enable_if<!std::is_same<B, int32_t>::value>::type...>
		constexpr quat_t(int _x, int _y, int _z, int _w) : q(_x, _y, _z, _w) {}
		constexpr quat_t(int32_t _x, int32_t _y, int32_t _z, int32_t _w) : q(_x, _y, _z, _w) {}
        constexpr quat_t(float _x, float _y, float _z, float _w) : q(_x, _y, _z, _w) {}*/
        quat_t(const vec4_t<T> &q) : q(q){};

        quat_t(const vec3_t<T> &axis, T angle) { fromAxisAngle(axis, angle); };
        void fromAxisAngle(const vec3_t<T> &axis, T angle)
        {
            T sz;
            T cz;
            sincos(angle * 0.5f, sz, cz);
            q.x = axis.x * sz;
            q.y = axis.y * sz;
            q.z = axis.z * sz;
            q.w = cz;
        }

        quat_t &operator=(quat_t b)
        {
            q = b.q;
            return *this;
        }

        inline const T &operator[](unsigned int col) const { return q[col]; }
        inline T &operator[](unsigned int col) { return q[col]; }

        explicit operator mat3x3_t<T>() const
        {
            // See: http://www.geometrictools.com/Documentation/LinearAlgebraicQuaternions.pdf
            mat3x3_t<T> tmp;
            tmp.m[0] = {1 - 2 * (q.y * q.y + q.z * q.z), 2 * (q.x * q.y - q.z * q.w), 2 * (q.x * q.z + q.y * q.w)};
            tmp.m[1] = {2 * (q.x * q.y + q.z * q.w), 1 - 2 * (q.x * q.x + q.z * q.z), 2 * (q.y * q.z - q.x * q.w)};
            tmp.m[2] = {2 * (q.x * q.z - q.y * q.w), 2 * (q.y * q.z + q.x * q.w), 1 - 2 * (q.x * q.x + q.y * q.y)};
            return tmp;
        }
        mat3x3_t<T> toRotation() const { return mat3x3_t<T>(*this); }

        quat_t &operator*=(const quat_t &b)
        {
            auto tmp = q;
            q.x = tmp.x * b.w + tmp.y * b.z - tmp.z * b.y + tmp.w * b.x;
            q.y = -tmp.x * b.z + tmp.y * b.w + tmp.z * b.x + tmp.w * b.y;
            q.z = tmp.x * b.y - tmp.y * b.x + tmp.z * b.w + tmp.w * b.z;
            q.w = -tmp.x * b.x - tmp.y * b.y - tmp.z * b.z + tmp.w * b.w;
            return *this;
        }
        quat_t &operator*=(T b)
        {
            q *= b;
            return *this;
        }
        quat_t &operator/=(const quat_t &b)
        {
            auto tmp = q;
            q.x = tmp.x * b.w - tmp.y * b.z + tmp.z * b.y - tmp.w * b.x;
            q.y = tmp.x * b.z + tmp.y * b.w - tmp.z * b.x - tmp.w * b.y;
            q.z = -tmp.x * b.y + tmp.y * b.x + tmp.z * b.w - tmp.w * b.z;
            q.w = tmp.x * b.x + tmp.y * b.y + tmp.z * b.z + tmp.w * b.w;
            return *this;
        }
        quat_t &operator/=(T b)
        {
            q *= 1.0f / b;
            return *this;
        }

        friend bool operator==(const quat_t &a, const quat_t &b) { return a.q == b.q; }
        friend bool operator!=(const quat_t &a, const quat_t &b) { return !(a == b); }

        quat_t &setIdentity()
        {
            q = {0, 0, 0, 1};
            return *this;
        }
        constexpr static quat_t identity() { return quat_t({0, 0, 0, 1}); }

        T norm() const { return q.norm(); }
        T length() const { return q.length(); }
        quat_t &normalize()
        {
            q.normalize();
            return *this;
        }
        T normalizeAndReturnLength() { return q.normalizeAndReturnLength(); }
        quat_t normalized() const
        {
            quat_t tmp(*this);
            tmp.normalize();
            return tmp;
        }

        quat_t &conjugate()
        {
            q.x = -q.x;
            q.y = -q.y;
            q.z = -q.z;
            return *this;
        }
        quat_t conjugated() const
        {
            quat_t tmp(*this);
            tmp.conjugate();
            return tmp;
        }
        T dot(const quat_t &b) const { return q.dot(b); }

        quat_t lerp(const quat_t &b, T t) const
        {
            auto angle = this->dot(b);
            T s = angle >= 0 ? 1 - t : t - 1;
            return (*this * s + b * t).normalized();
        }
        quat_t slerp(const quat_t &b, T t) const
        {
            auto angle = this->dot(b);
            T sign = angle < 0 ? -1 : 1;
            angle = abs(angle);
            T s = 1 - t;
            if (angle < 0.99f)
            {
                angle = acos(angle);
                t = angle * t;
                s = sin(angle - t);
                t = sin(t);
            }
            return (*this * (s * sign) + b * t).normalized();
        }
    } __attribute__((aligned(4), packed));

    using fp1616quat_t = quat_t<fp1616_t>; //16.16

} // namespace Math
