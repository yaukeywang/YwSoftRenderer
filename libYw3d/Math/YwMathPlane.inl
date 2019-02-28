// Add by yaukey at 2018-03-06.
// Plane class.

#ifndef __YW_MATH_PLANE_INL__
#define __YW_MATH_PLANE_INL__

#include "YwMathUtility.h"
#include "YwMathPlane.h"

namespace yw
{
    inline Plane::Plane() : a(0.0f), b(0.0f), c(0.0f), d(0.0f)
    {
    }

    inline Plane::Plane(const float a, const float b, const float c, const float d)
    {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
    }

    inline Plane::Plane(const Vector3& n, const float d) : normal(n)
    {
        this->w = d;
    }

    inline Plane::Plane(const Plane& p)
    {
        this->a = p.a;
        this->b = p.b;
        this->c = p.c;
        this->d = p.d;
    }

    inline const Plane& Plane::operator =(const Plane& p)
    {
        this->a = p.a;
        this->b = p.b;
        this->c = p.c;
        this->d = p.d;

        return *this;
    }

    inline Plane::operator float*()
    {
        return &a;
    }

    inline Plane::operator const float*() const
    {
        return &a;
    }

    inline Plane Plane::operator +() const
    {
        return *this;
    }

    inline Plane Plane::operator -() const
    {
        return Plane(-normal, w);
    }

    inline float Plane::operator *(const Vector3& v) const
    {
        return Vector3Dot(normal, v) + w;
    }

    inline float Plane::operator *(const Vector4& v) const
    {
        return Vector3Dot(normal, v) + w * v.w;
    }
}

#endif // !__YW_MATH_PLANE_INL__
