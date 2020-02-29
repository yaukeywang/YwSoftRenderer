// Add by yaukey at 2017-09-19.
// All vector class.

#ifndef __YW_MATH_VECTOR_H__
#define __YW_MATH_VECTOR_H__

#include "YwMathUtility.h"

namespace yw
{
    // Predefine.
    struct Vector3;
    struct Vector4;
    struct Matrix33;
    struct Matrix44;

    // Vector 2 class.
    struct Vector2
    {
    public:
        union
        {
            float m[2];
            struct { float x; float y; };
            struct { float r; float g; };
        };

    public:
        Vector2();
        Vector2(const float n);
        Vector2(const Vector2& v);
        Vector2(const float x, const float y);
        Vector2(const Vector3& v);
        Vector2(const Vector4& v);

    public:
        // Member functions.

        Vector2& operator =(const float n);
        Vector2& operator =(const Vector2& v);
        Vector2& operator =(const Vector3& v);
        Vector2& operator =(const Vector4& v);
        bool operator ==(const Vector2& v) const;
        bool operator !=(const Vector2& v) const;
        Vector2 operator -() const;
        Vector2 operator +(const Vector2& v) const;
        Vector2 operator -(const Vector2& v) const;
        Vector2 operator *(const Vector2& v) const;
        Vector2 operator *(const float n) const;
        Vector2 operator /(const float n) const;
        Vector2& operator +=(const Vector2& v);
        Vector2& operator -=(const Vector2& v);
        Vector2& operator *=(const Vector2& v);
        Vector2& operator *=(const float n);
        Vector2& operator /=(const float n);
        operator Vector3();
        operator Vector4();

        void Set(float x, float y);
        void Set(const Vector2& v);
        void Reset();
        float Length() const;
        float SquaredLength() const;
        Vector2& Normalize();

        // Static functions.

        static Vector2 Zero();
        static Vector2 One();
        static Vector2 Left();
        static Vector2 Right();
        static Vector2 Down();
        static Vector2 Up();
    };

    // Vector2 nonmember functions.

    // Vector2 multiply number.
    Vector2 operator *(const float n, const Vector2& v);

    // Normalize a vector.
    Vector2& Vector2Normalize(Vector2& out, const Vector2& v);

    // The result of two vector2 dot product.
    float Vector2Dot(const Vector2& left, const Vector2& right);

    // The result of two vector2 cross product.
    float Vector2Cross(const Vector2& left, const Vector2& right);

    // The result of two vector2 distance.
    float Vector2Distance(const Vector2& left, const Vector2& right);

    // The result of two vector2 squared distance.
    float Vector2SquaredDistance(const Vector2& left, const Vector2& right);

    // Linearly interpolates between two values.
    Vector2& Vector2Lerp(Vector2& out, const Vector2& a, const Vector2& b, const float t);

    // Vector 3 class.
    struct Vector3
    {
    public:
        union
        {
            float m[3];
            struct { float x; float y; float z; };
            struct { float r; float g; float b; };
        };

    public:
        Vector3();
        Vector3(const float n);
        Vector3(const Vector3& v);
        Vector3(const float x, const float y, const float z);
        Vector3(const Vector2& v);
        Vector3(const Vector2& v, const float z);
        Vector3(const Vector4& v);

    public:
        // Member functions.

        Vector3& operator =(const float n);
        Vector3& operator =(const Vector3& v);
        Vector3& operator =(const Vector2& v);
        Vector3& operator =(const Vector4& v);
        bool operator ==(const Vector3& v) const;
        bool operator !=(const Vector3& v) const;
        Vector3 operator -() const;
        Vector3 operator +(const Vector3& v) const;
        Vector3 operator -(const Vector3& v) const;
        Vector3 operator *(const Vector3& v) const;
        Vector3 operator *(const float n) const;
        Vector3 operator *(const Matrix33& m) const;
        Vector3 operator /(const float n) const;
        Vector3& operator +=(const Vector3& v);
        Vector3& operator -=(const Vector3& v);
        Vector3& operator *=(const Vector3& v);
        Vector3& operator *=(const float n);
        Vector3& operator *=(const Matrix33& m);
        Vector3& operator /=(const float n);
        operator Vector2();
        operator Vector4();

        void Set(float x, float y, float z);
        void Set(const Vector3& v);
        void Reset();
        float Length() const;
        float SquaredLength() const;
        Vector3& Normalize();

        // Static functions.

        static Vector3 Zero();
        static Vector3 One();
        static Vector3 Left();
        static Vector3 Right();
        static Vector3 Down();
        static Vector3 Up();
        static Vector3 Back();
        static Vector3 Forward();
        static Vector3 Red();
        static Vector3 Green();
        static Vector3 Blue();
        static Vector3 Black();
        static Vector3 White();
    };

    // Vector3 nonmember functions.

    // Vector3 multiply number.
    Vector3 operator *(const float n, const Vector3& v);

    // Normalize a vector.
    Vector3& Vector3Normalize(Vector3& out, const Vector3& v);

    // The result of two vector3 dot product.
    float Vector3Dot(const Vector3& left, const Vector3& right);

    // The result of two vector3 cross product.
    Vector3& Vector3Cross(Vector3& out, const Vector3& left, const Vector3& right);

    // The result of vector a project to vector b.
    Vector3& Vector3Project(Vector3& out, const Vector3& left, const Vector3& right);

    // The result of vector a reject to vector b.
    Vector3& Vector3Reject(Vector3& out, const Vector3& left, const Vector3& right);

    // The result of two vector3 distance.
    float Vector3Distance(const Vector3& left, const Vector3& right);

    // The result of two vector3 squared distance.
    float Vector3SquaredDistance(const Vector3& left, const Vector3& right);

    // Linearly interpolates between two values.
    Vector3& Vector3Lerp(Vector3& out, const Vector3& a, const Vector3& b, const float t);

    // Transforms vector (x, y, z, 1) by a given matrix.
    // This function transforms the vector, v (x, y, z, 1), by the matrix m.
    Vector3& Vector3Transform(Vector3& out, const Vector3& v, const Matrix44& m);

    // Transforms a 3D vector by a given matrix, projecting the result back into w = 1.
    // This function transforms the vector, v (x, y, z, 1), by the matrix, m, projecting the result back into w=1.
    Vector3& Vector3TransformCoord(Vector3& out, const Vector3& v, const Matrix44& m);

    // Transforms the 3D vector normal by the given matrix.
    // This function transforms the vector (v.x, v.y, v.z, 0) by the matrix pointed to by m.
    // If you want to transform a normal, the matrix you pass to this function should be the transpose of the inverse of the matrix you would use to transform a point.
    Vector3& Vector3TransformNormal(Vector3& out, const Vector3& v, const Matrix44& m);

    // Vector 4 class.
    struct Vector4
    {
    public:
        union
        {
            float m[4];
            struct { float x; float y; float z; float w; };
            struct { float r; float g; float b; float a; };
        };

    public:
        Vector4();
        Vector4(const float n);
        Vector4(const Vector4& v);
        Vector4(const float x, const float y, const float z, const float w);
        Vector4(const Vector2& v);
        Vector4(const Vector3& v);
        Vector4(const Vector3& v, const float w);

    public:
        // Member functions.

        Vector4& operator =(const float n);
        Vector4& operator =(const Vector4& v);
        Vector4& operator =(const Vector2& v);
        Vector4& operator =(const Vector3& v);
        bool operator ==(const Vector4& v) const;
        bool operator !=(const Vector4& v) const;
        Vector4 operator -() const;
        Vector4 operator +(const Vector4& v) const;
        Vector4 operator -(const Vector4& v) const;
        Vector4 operator *(const Vector4& v) const;
        Vector4 operator *(const float n) const;
        Vector4 operator *(const Matrix44& m) const;
        Vector4 operator /(const float n) const;
        Vector4& operator +=(const Vector4& v);
        Vector4& operator -=(const Vector4& v);
        Vector4& operator *=(const Vector4& v);
        Vector4& operator *=(const float n);
        Vector4& operator *=(const Matrix44& m);
        Vector4& operator /=(const float n);
        operator Vector2();
        operator Vector3();

        void Set(float x, float y, float z, float w);
        void Set(const Vector4& v);
        void Reset();
        float Length() const;
        float SquaredLength() const;
        Vector4& Normalize();

        // Divides the vector by its w-coordinate.
        Vector4 & Homogenize();

        // Static functions.

        static Vector4 Zero();
        static Vector4 One();
        static Vector4 Red();
        static Vector4 Green();
        static Vector4 Blue();
        static Vector4 Black();
        static Vector4 White();
    };

    // Vector4 nonmember functions.

    // Vector4 multiply number.
    Vector4 operator *(const float n, const Vector4& v);

    // Normalize a vector.
    Vector4& Vector4Normalize(Vector4& out, const Vector4& v);

    // The result of two vector4 dot product.
    float Vector4Dot(const Vector4& left, const Vector4& right);

    // The result of two vector4 cross product.
    Vector4& Vector4Cross(Vector4& out, const Vector4& left, const Vector4& right);

    // The result of two vector4 distance.
    float Vector4Distance(const Vector4& left, const Vector4& right);

    // The result of two vector4 squared distance.
    float Vector4SquaredDistance(const Vector4& left, const Vector4& right);

    // Linearly interpolates between two values.
    Vector4& Vector4Lerp(Vector4& out, const Vector4& a, const Vector4& b, const float t);
}

#include "YwMathVector.inl"

#endif // !__YW_MATH_VECTOR_H__
