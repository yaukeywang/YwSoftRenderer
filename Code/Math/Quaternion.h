// Add by yaukey at 2017-10-07.
// Quaternion class.

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "MathUtility.h"

namespace yw
{
    // Vector 2 class.
    struct Quaternion
    {
    public:
        float x;
        float y;
        float z;
        float w;

    public:
        Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
        {
        }

        Quaternion(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw)
        {
        }

        Quaternion(const Quaternion& o) : x(o.x), y(o.y), z(o.z)
        {
        }

    public:
        // Member functions.

        inline Quaternion& operator =(const Quaternion& o)
        {
            x = o.x;
            y = o.y;
            z = o.z;
            w = o.w;
        }

        inline Quaternion operator +() const
        {
            return *this;
        }

        inline Quaternion operator -() const
        {
            return Quaternion(-x, -y, -z, -w);
        }

        inline Quaternion operator +(const Quaternion &o) const
        {
            return Quaternion(x + o.x, y + o.y, z + o.z, w + o.w);
        }

        inline Quaternion operator -(const Quaternion &o) const
        {
            return Quaternion(x - o.x, y - o.y, z - o.z, w - o.w);
        }

        inline Quaternion operator *(const Quaternion &o) const
        {
            Quaternion value;
            value.x = w * o.x + x * o.w + y * o.z - z * o.y;
            value.y = w * o.y - x * o.z + y * o.w + z * o.x;
            value.z = w * o.z + x * o.y - y * o.x + z * o.w;
            value.w = w * o.w - x * o.x - y * o.y - z * o.z;

            return value;
        }

        inline Quaternion operator *(const float o) const
        {
            Quaternion value(x * o, y * o, z * o, w * o);
            return value;
        }

        inline Quaternion operator /(const float o) const
        {
            float oneOverO = 1.0f / o;
            Quaternion value(x * oneOverO, y * oneOverO, z * oneOverO, w * oneOverO);

            return value;
        }

        inline Quaternion& operator +=(const Quaternion &o)
        {
            x += o.x;
            y += o.y;
            z += o.z;
            w += o.w;

            return *this;
        }

        inline Quaternion& operator -=(const Quaternion &o)
        {
            x -= o.x;
            y -= o.y;
            z -= o.z;
            w -= o.w;

            return *this;
        }

        inline Quaternion& operator *=(const Quaternion &o)
        {
            Quaternion value;
            value.x = w * o.x + x * o.w + y * o.z - z * o.y;
            value.y = w * o.y - x * o.z + y * o.w + z * o.x;
            value.z = w * o.z + x * o.y - y * o.x + z * o.w;
            value.w = w * o.w - x * o.x - y * o.y - z * o.z;
            *this = value;

            return *this;
        }

        inline Quaternion& operator *=(const float o)
        {
            x *= o;
            y *= o;
            z *= o;
            w *= o;

            return *this;
        }

        inline Quaternion& operator /=(const float o)
        {
            float oneOverO = 1.0f / o;
            x *= oneOverO;
            y *= oneOverO;
            z *= oneOverO;
            w *= oneOverO;

            return *this;
        }

        inline float Length() const
        {
            float value = sqrt(x * x + y * y + z * z + w * w);
            return value;
        }

        inline float SquaredLength() const
        {
            float value = x * x + y * y + z * z + w * w;
            return value;
        }

        inline Quaternion& Normalize()
        {
            float length = Length();
            *this /= length;

            return *this;
        }

        inline void SetIdentity()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 1.0f;
        }

        inline void SetRotationX(float theta)
        {
            float halfTheta = theta * 0.5f;
            float s = sin(halfTheta);
            float c = cos(halfTheta);

            x = s;
            y = 0.0f;
            z = 0.0f;
            w = c;
        }

        inline void SetRotationY(float theta)
        {
            float halfTheta = theta * 0.5f;
            float s = sin(halfTheta);
            float c = cos(halfTheta);

            x = 0.0f;
            y = s;
            z = 0.0f;
            w = c;
        }

        inline void SetRotationZ(float theta)
        {
            float halfTheta = theta * 0.5f;
            float s = sin(halfTheta);
            float c = cos(halfTheta);

            x = 0.0f;
            y = 0.0f;
            z = s;
            w = c;
        }

        // Static functions.
        static inline Quaternion Identity()
        {
            Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
            return value;
        }

        static inline float Dot(const Quaternion& left, const Quaternion& right)
        {
            float value = left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
            return value;
        }

        static inline Quaternion Cross(const Quaternion& left, const Quaternion& right)
        {
            Quaternion value(
                left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y,
                left.w * right.y - left.x * right.z + left.y * right.w + left.z * right.x,
                left.w * right.z + left.x * right.y - left.y * right.x + left.z * right.w,
                left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z
            );

            return value;
        }

        static inline Quaternion Conjugate(const Quaternion& o)
        {
            Quaternion value = Quaternion(-o.x, -o.y, -o.z, o.w);
            return value;
        }

        static inline Quaternion CreateRotationX(float theta)
        {
            Quaternion value;
            value.SetRotationX(theta);

            return value;
        }

        static inline Quaternion CreateRotationY(float theta)
        {
            Quaternion value;
            value.SetRotationY(theta);

            return value;
        }

        static inline Quaternion CreateRotationZ(float theta)
        {
            Quaternion value;
            value.SetRotationZ(theta);

            return value;
        }
    };
}

#endif // !__QUATERNION_H__
