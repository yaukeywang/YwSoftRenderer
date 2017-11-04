// Add by yaukey at 2017-10-07.
// Quaternion class.

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "MathUtility.h"
#include "Vector.h"

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

        Quaternion(const Quaternion& q) : x(q.x), y(q.y), z(q.z)
        {
        }

    public:
        // Member functions.

        inline Quaternion& operator =(const Quaternion& q)
        {
            x = q.x;
            y = q.y;
            z = q.z;
            w = q.w;
        }

        inline Quaternion operator +() const
        {
            return *this;
        }

        inline Quaternion operator -() const
        {
            return Quaternion(-x, -y, -z, -w);
        }

        inline Quaternion operator +(const Quaternion& q) const
        {
            return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
        }

        inline Quaternion operator -(const Quaternion& q) const
        {
            return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
        }

        inline Quaternion operator *(const Quaternion& q) const
        {
            Quaternion value;
            value.x = w * q.x + x * q.w + y * q.z - z * q.y;
            value.y = w * q.y - x * q.z + y * q.w + z * q.x;
            value.z = w * q.z + x * q.y - y * q.x + z * q.w;
            value.w = w * q.w - x * q.x - y * q.y - z * q.z;

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

        inline Quaternion& operator +=(const Quaternion& q)
        {
            x += q.x;
            y += q.y;
            z += q.z;
            w += q.w;

            return *this;
        }

        inline Quaternion& operator -=(const Quaternion& q)
        {
            x -= q.x;
            y -= q.y;
            z -= q.z;
            w -= q.w;

            return *this;
        }

        inline Quaternion& operator *=(const Quaternion& q)
        {
            Quaternion value;
            value.x = w * q.x + x * q.w + y * q.z - z * q.y;
            value.y = w * q.y - x * q.z + y * q.w + z * q.x;
            value.z = w * q.z + x * q.y - y * q.x + z * q.w;
            value.w = w * q.w - x * q.x - y * q.y - z * q.z;
            *this = value;

            return *this;
        }

        inline Quaternion& operator *=(const float q)
        {
            x *= q;
            y *= q;
            z *= q;
            w *= q;

            return *this;
        }

        inline Quaternion& operator /=(const float q)
        {
            float oneOverO = 1.0f / q;
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

        inline void SetEulerX(float theta)
        {
            float halfTheta = theta * 0.5f;
            float s = sin(halfTheta);
            float c = cos(halfTheta);

            x = s;
            y = 0.0f;
            z = 0.0f;
            w = c;
        }

        inline void SetEulerY(float theta)
        {
            float halfTheta = theta * 0.5f;
            float s = sin(halfTheta);
            float c = cos(halfTheta);

            x = 0.0f;
            y = s;
            z = 0.0f;
            w = c;
        }

        inline void SetEulerZ(float theta)
        {
            float halfTheta = theta * 0.5f;
            float s = sin(halfTheta);
            float c = cos(halfTheta);

            x = 0.0f;
            y = 0.0f;
            z = s;
            w = c;
        }

        inline void SetEulerXYZ(float thetaX, float thetaY, float thetaZ)
        {
            // Use ZYX order.

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

        inline static Quaternion Conjugate(const Quaternion& q)
        {
            Quaternion value = Quaternion(-q.x, -q.y, -q.z, q.w);
            return value;
        }

        inline static Quaternion Inverse(const Quaternion& q)
        {
            Quaternion conjugate = Conjugate(q);
            Quaternion value = conjugate / q.SquaredLength();

            return value;
        }

        inline static Quaternion Difference(const Quaternion& left, const Quaternion& right)
        {
            Quaternion inverse = Inverse(left);
            Quaternion difference = Cross(inverse, right);

            return difference;
        }

        inline static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t)
        {
            // q0 if t is less equal smaller border 0.
            if (t <= 0.0f)
            {
                return q0;
            }

            // q1 if t is greater equal bigger border 1.
            if (t >= 1.0f)
            {
                return q1;
            }

            // Get the temp quaternion.
            float x1 = 0.0f;
            float y1 = 0.0f;
            float z1 = 0.0f;
            float w1 = 0.0f;

            // Get cos omega by dot product.
            float cosOmega = Dot(q0, q1);

            // If dot product is negative, adjust signs in order to choose the shortest path between q0 and q1.
            if (cosOmega < 0.0f)
            {
                x1 = -q1.x;
                y1 = -q1.y;
                z1 = -q1.z;
                w1 = -q1.w;
            }
            else
            {
                x1 = q1.x;
                y1 = q1.y;
                z1 = q1.z;
                w1 = q1.w;
            }

            // Check if the angle between q0 and q1 is almost 0.
            float k0 = 0.0f;
            float k1 = 0.0f;
            if (cosOmega > 0.9999f)
            {
                // Linear lerp.
                k0 = 1.0f - t;
                k1 = t;
            }
            else
            {
                float omega = acosf(cosOmega);
                float oneOverSinOmega = 1.0f / sinf(omega);
                k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
                k1 = sinf(t * omega) * oneOverSinOmega;
            }

            // Combine new quaternion by qt = q0 * k0 + q1 * k1;
            Quaternion value(
                q0.x * k0 + x1 * k1,
                q0.y * k0 + y1 * k1,
                q0.z * k0 + z1 * k1,
                q0.w * k0 + w1 * k1
            );

            return value;
        }

        inline static Quaternion Pow(const Quaternion& q, float exponent)
        {
            // Check normalized quaternion.
            if (q.w > 0.9999f)
            {
                return q;
            }

            // Get alpha (alpha = theta / 2).
            float alpha = acosf(q.w);

            // Get new alpha.
            float newAlpha = alpha * exponent;

            // Get new w component.
            float w = cosf(newAlpha);

            // Get multiply.
            // [x, y, z, w] => [sin(alpha) * n, cos(alpha)]
            // newAlpha = alpha * exponent.
            // n = [x, y, z] / sin(alpha), multiply = sin(newAlpha) / sin(alpha), n' = (sin(alpha) * n) * multiply.
            float multiply = sinf(newAlpha) / sinf(alpha);

            // Get new sin(t * alpha) * n.
            float x = q.x * multiply;
            float y = q.y * multiply;
            float z = q.z * multiply;

            // Combine a new quaternion.
            Quaternion value(x, y, z, w);

            return value;
        }

        inline static Quaternion FromEulerX(float theta)
        {
            Quaternion value;
            value.SetEulerX(theta);

            return value;
        }

        inline static Quaternion FromEulerY(float theta)
        {
            Quaternion value;
            value.SetEulerY(theta);

            return value;
        }

        inline static Quaternion FromEulerZ(float theta)
        {
            Quaternion value;
            value.SetEulerZ(theta);

            return value;
        }

        inline static Quaternion FromEuler(float thetaX, float thetaY, float thetaZ)
        {
            Quaternion value;
            value.SetEulerXYZ(thetaX, thetaY, thetaZ);

            return value;
        }

        // ZYX euler order conversion.
        inline static bool ToEuler(Vector3& out, const Quaternion& q)
        {
            // from http://www.geometrictools.com/Documentation/EulerAngles.pdf.
            // ZYX order.

            float col20 = 2.0f * (q.x * q.z + q.y * q.w);
            if (col20 < 0.9999f)
            {
                if (col20 > -0.9999f)
                {
                    out.y = asin(-col20);
                    out.z = atan2(2.0f * (q.x * q.y - q.z * q.w), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
                    out.x = atan2(2.0f * (q.y * q.z - q.x * q.w), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));

                    return true;
                }
                else // r20 = −1
                {
                    // Not a unique solution : thetaX − thetaZ = atan2(−r12 , r11).
                    out.y = YW_PI * 0.5f;
                    out.z = -atan2(-2.0f * (q.y * q.z + q.x * q.w), 1.0f - 2.0f * (q.x * q.x + q.z * q.z));
                    out.x = 0.0f;

                    return false;
                }
            }
            else // r20 = +1
            {
                // Not a unique solution : thetaX + thetaZ = atan2(−r12 , r11).
                out.y = -YW_PI * 0.5f;
                out.z = atan2(-2.0f * (q.y * q.z + q.x * q.w), 1.0f - 2.0f * (q.x * q.x + q.z * q.z));
                out.x = 0.0f;

                return false;
            }
        }
    };
}

#endif // !__QUATERNION_H__
