// Add by yaukey at 2018-03-06.
// Plane class.

#ifndef __YW_MATH_PLANE_INL__
#define __YW_MATH_PLANE_INL__

#include "YwMathUtility.h"
#include "YwMathPlane.h"

namespace yw
{
    inline Plane::Plane() : normal(), d(0.0f)
    {
    }

    inline Plane::Plane(const float a, const float b, const float c, const float d)
    {
        normal.Set(a, b, c);
        this->d = d;
    }

    inline Plane::Plane(const Vector3& n, const float d) : normal(n)
    {
        this->d = d;
    }

    inline Plane::Plane(const Plane& p) : normal(p.normal), d(p.d)
    {
    }

    inline const Plane& Plane::operator =(const Plane& p)
    {
        normal = p.normal;
        d = p.d;

        return *this;
    }

    inline Plane::operator float*()
    {
        return &normal.x;
    }

    inline Plane::operator const float*() const
    {
        return &normal.x;
    }

    inline Plane Plane::operator +() const
    {
        return *this;
    }

    inline Plane Plane::operator -() const
    {
        return Plane(-normal, d);
    }

    inline float Plane::operator *(const Vector3& v) const
    {
        return Vector3Dot(normal, v) + d;
    }

    inline float Plane::operator *(const Vector4& v) const
    {
        return Vector3Dot(normal, v) + d * v.w;
    }
}

#endif // !__YW_MATH_PLANE_INL__
