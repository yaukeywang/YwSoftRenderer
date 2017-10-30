// Add by yaukey at 2017-10-07.
// Quaternion class.

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

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
    };
}

#endif // !__QUATERNION_H__
