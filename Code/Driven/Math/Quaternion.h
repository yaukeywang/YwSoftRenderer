// Add by yaukey at 2017-10-07.
// Quaternion class.

#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include "MathUtility.h"
#include "Vector.h"

namespace yw
{
    // Predefine.
    struct Matrix33;
    struct Matrix44;

    // Vector 2 class.
    struct Quaternion
    {
    public:
        union
        {
            struct { float x; float y; float z; float w; };
            struct { Vector3 v; float w; };
        };

    public:
        Quaternion();
        Quaternion(float nx, float ny, float nz, float nw);
        Quaternion(const Quaternion& q);

    public:
        // Member functions.

        Quaternion& operator =(const Quaternion& q);
        Quaternion operator +() const;
        Quaternion operator -() const;
        Quaternion operator +(const Quaternion& q) const;
        Quaternion operator -(const Quaternion& q) const;
        Quaternion operator *(const Quaternion& q) const;
        Quaternion operator *(const float n) const;
        Quaternion operator /(const float n) const;
        Quaternion& operator +=(const Quaternion& q);
        Quaternion& operator -=(const Quaternion& q);
        Quaternion& operator *=(const Quaternion& q);
        Quaternion& operator *=(const float q);
        Quaternion& operator /=(const float q);

        void Set(float nx, float ny, float nz, float nw);
        void Set(const Quaternion& q);
        float Length() const;
        float SquaredLength() const;
        Quaternion& Normalize();
        void SetIdentity();
        void SetEulerX(float theta);
        void SetEulerY(float theta);
        void SetEulerZ(float theta);
        void SetEulerXYZ(float thetaX, float thetaY, float thetaZ);
    };

    // Quaternion nonmember functions.

    Quaternion operator *(const float n, const Quaternion& q);
    Quaternion QuaternionIdentity();
    float QuaternionDot(const Quaternion& left, const Quaternion& right);
    Quaternion QuaternionCross(const Quaternion& left, const Quaternion& right);
    Quaternion QuaternionConjugate(const Quaternion& q);
    Quaternion QuaternionInverse(const Quaternion& q);
    Quaternion QuaternionDifference(const Quaternion& left, const Quaternion& right);
    Quaternion& QuaternionSlerp(Quaternion& out, const Quaternion& q0, const Quaternion& q1, float t);
    Quaternion& QuaternionPow(Quaternion& out, const Quaternion& q, float exponent);
    Quaternion& QuaternionFromEulerX(Quaternion& out, float theta);
    Quaternion& QuaternionFromEulerY(Quaternion& out, float theta);
    Quaternion& QuaternionFromEulerZ(Quaternion& out, float theta);
    Quaternion QuaternionFromEuler(Quaternion& out, float thetaX, float thetaY, float thetaZ);
    Vector3& QuaternionToEuler(Vector3& out, const Quaternion& q); // ZYX euler order conversion.
    Quaternion& QuaternionFromMatrix33(Quaternion& out, const Matrix33& mat);
    Quaternion& QuaternionFromMatrix44(Quaternion& out, const Matrix44& mat);
}

#include "Quaternion.inl"

#endif // !__QUATERNION_H__
