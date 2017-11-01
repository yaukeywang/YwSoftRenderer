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

        inline void SetRotationXYZ(float thetaX, float thetaY, float thetaZ)
        {
            float halfThetaX = thetaX * 0.5f;
            float sinHTX = sin(halfThetaX);
            float cosHTX = cos(halfThetaX);

            float halfThetaY = thetaY * 0.5f;
            float sinHTY = sin(halfThetaY);
            float cosHTY = cos(halfThetaY);

            float halfThetaZ = thetaZ * 0.5f;
            float sinHTZ = sin(halfThetaZ);
            float cosHTZ = cos(halfThetaZ);

            x = cosHTZ * cosHTY * sinHTX - sinHTZ * sinHTY * cosHTX;
            y = cosHTZ * sinHTY * cosHTX + sinHTZ * cosHTY * sinHTX;
            z = sinHTZ * cosHTY * cosHTX - cosHTZ * sinHTY * sinHTX;
            w = cosHTZ * cosHTY * cosHTX + sinHTZ * sinHTY * sinHTX;
        }

        // Static functions.
        inline static Quaternion Identity()
        {
            Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
            return value;
        }

        inline static float Dot(const Quaternion& left, const Quaternion& right)
        {
            float value = left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
            return value;
        }

        inline static Quaternion Cross(const Quaternion& left, const Quaternion& right)
        {
            Quaternion value(
                left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y,
                left.w * right.y - left.x * right.z + left.y * right.w + left.z * right.x,
                left.w * right.z + left.x * right.y - left.y * right.x + left.z * right.w,
                left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z
            );

            return value;
        }

        inline static Quaternion Conjugate(const Quaternion& o)
        {
            Quaternion value = Quaternion(-o.x, -o.y, -o.z, o.w);
            return value;
        }

        inline static Quaternion Inverse(const Quaternion& o)
        {
            Quaternion conjugate = Conjugate(o);
            Quaternion value = conjugate / o.SquaredLength();

            return value;
        }

        inline static Quaternion Difference(const Quaternion& left, const Quaternion& right)
        {
            Quaternion inverse = Inverse(left);
            Quaternion difference = Cross(inverse, right);

            return difference;
        }

        inline static Quaternion CreateRotationX(float theta)
        {
            Quaternion value;
            value.SetRotationX(theta);

            return value;
        }

        inline static Quaternion CreateRotationY(float theta)
        {
            Quaternion value;
            value.SetRotationY(theta);

            return value;
        }

        inline static Quaternion CreateRotationZ(float theta)
        {
            Quaternion value;
            value.SetRotationZ(theta);

            return value;
        }

        inline static Quaternion CreateRotationXYZ(float thetaX, float thetaY, float thetaZ)
        {
            Quaternion value;
            value.SetRotationXYZ(thetaX, thetaY, thetaZ);

            return value;
        }
    };
}

#endif // !__QUATERNION_H__
