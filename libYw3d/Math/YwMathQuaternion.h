﻿// Add by yaukey at 2017-10-07.
// Quaternion class.

#ifndef __YW_MATH_QUATERNION_H__
#define __YW_MATH_QUATERNION_H__

#include "YwMathUtility.h"
#include "YwMathVector.h"

namespace yw
{
    // Predefine.
    struct Matrix33;
    struct Matrix44;

    // Quaternion class.
    struct Quaternion
    {
    public:
        float x; float y; float z; float w;

    public:
        Quaternion();
        Quaternion(float x, float y, float z, float w);
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

        void Set(float x, float y, float z, float w);
        void Set(const Quaternion& q);
        float Length() const;
        float SquaredLength() const;
        Quaternion& Normalize();
        void SetIdentity();
        void SetRotationAboutX(const float theta);
        void SetRotationAboutY(const float theta);
        void SetRotationAboutZ(const float theta);
        void SetRotationAboutXYZ(const float thetaX, const float thetaY, const float thetaZ);
        void SetRotationAboutAxis(const Vector3& axis, const float theta);
    };

    // Quaternion nonmember functions.

    // Operator of quaternion with number.
    Quaternion operator *(const float n, const Quaternion& q);

    // Set quaternion identity.
    Quaternion& QuaternionIdentity(Quaternion& out);

    // Normalize a quaternion.
    Quaternion& QuaternionNormalize(Quaternion& out, const Quaternion& v);

    // Dot product of quaternion.
    float QuaternionDot(const Quaternion& left, const Quaternion& right);

    // Cross product of quaternion.
    Quaternion& QuaternionCross(Quaternion& out, const Quaternion& left, const Quaternion& right);

    // Get conjugate of quaternion.
    Quaternion& QuaternionConjugate(Quaternion& out, const Quaternion& q);

    // Get inverse of quaternion.
    Quaternion& QuaternionInverse(Quaternion& out, const Quaternion& q);

    // Get difference of quaternion.
    Quaternion& QuaternionDifference(Quaternion& out, const Quaternion& left, const Quaternion& right);

    // Get slerp of quaternion.
    Quaternion& QuaternionSlerp(Quaternion& out, const Quaternion& q0, const Quaternion& q1, const float t);

    // Get pow of quaternion.
    Quaternion& QuaternionPow(Quaternion& out, const Quaternion& q, const float exponent);

    // Get quaternion from x of euler.
    Quaternion& QuaternionFromRotationAboutX(Quaternion& out, const float theta);

    // Get quaternion from y of euler.
    Quaternion& QuaternionFromRotationAboutY(Quaternion& out, const float theta);

    // Get quaternion from z of euler.
    Quaternion& QuaternionFromRotationAboutZ(Quaternion& out, const float theta);

    // Get quaternion from rotation about axis with theta.
    Quaternion& QuaternionFromRotationAboutAxis(Quaternion& out, const Vector3& axis, const float theta);

    // Get quaternion from x, y and z of euler.
    Quaternion QuaternionFromEuler(Quaternion& out, const float thetaX, const float thetaY, const float thetaZ);

    // Get x, y and z of euler from quaternion.
    Vector3& QuaternionToEuler(Vector3& out, const Quaternion& q); // ZYX euler order conversion.

    // Get quaternion from matrix 3x3.
    Quaternion& QuaternionFromMatrix33(Quaternion& out, const Matrix33& m);

    // Get quaternion from matrix 4x4.
    Quaternion& QuaternionFromMatrix44(Quaternion& out, const Matrix44& m);
}

#include "YwMathQuaternion.inl"

#endif // !__YW_MATH_QUATERNION_H__
