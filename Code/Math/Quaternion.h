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

        //inline Quaternion operator *(const Quaternion &o) const
        //{

        //}

        //inline Quaternion operator *(const float o) const
        //{

        //}

        //inline Quaternion operator /(const float o) const
        //{

        //}
    };
}

#endif // !__QUATERNION_H__
