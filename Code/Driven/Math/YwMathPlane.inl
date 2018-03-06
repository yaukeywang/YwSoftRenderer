// Add by yaukey at 2018-03-06.
// Plane class.

#ifndef __YW_MATH_PLANE_INL__
#define __YW_MATH_PLANE_INL__

#include "YwMathUtility.h"
#include "YwMathPlane.h"

namespace yw
{
    inline Plane::Plane() : d(0.0f)
    {

    }

    inline Plane::Plane(const float na, const float nb, const float nc, const float nd)
    {
        normal.Set(na, nb, nc);
        d = nd;
    }

    inline Plane::Plane(const Vector3& n, const float nd) : normal(n), d(nd)
    {
    }

    inline Plane::Plane(const Plane& p) : normal(p.normal), d(p.d)
    {
    }

    inline const Plane& Plane::operator =(const Plane& p)
    {
        normal = p.normal;
        d = p.d;
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
