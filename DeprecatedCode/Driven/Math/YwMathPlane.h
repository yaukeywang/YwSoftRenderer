// Add by yaukey at 2018-03-06.
// Plane class.

#ifndef __YW_MATH_PLANE_H__
#define __YW_MATH_PLANE_H__

#include "YwMathUtility.h"
#include "YwMathVector.h"

namespace yw
{
    // Plane class.
    struct Plane
    {
    public:
        union
        {
            struct { Vector3 normal; float d; };
            struct { float a; float b; float c; float d; };
            Vector4 p;
        };

    public:
        Plane();
        Plane(const float na, const float nb, const float nc, const float nd);
        Plane(const Vector3& n, const float nd);
        Plane(const Plane& p);

    public:
        // Assign operator.
        const Plane& operator =(const Plane& p);

        // Return address of normal.
        operator float*();

        // Return address of normal.
        operator const float*() const;

        // Return this plane.
        Plane operator +() const;

        // Returns the plane with negated normal.
        Plane operator -() const;

        // Calculates the dot product with the vector.
        float operator *(const Vector3& v) const;

        // Calculates the dot product with the vector.
        float operator *(const Vector4& v) const;
    };
}

#include "YwMathPlane.inl"

#endif // !__YW_MATH_PLANE_H__
