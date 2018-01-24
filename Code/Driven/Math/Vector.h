// Add by yaukey at 2017-09-19.
// All vector class.

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "MathUtility.h"

namespace yw
{
    // Predefine.
    struct Vector3;
    struct Vector4;

    // Vector 2 class.
    struct Vector2
    {
    public:
        union
        {
            float m[2];
            struct { float x; float y; };
        };

    public:
        Vector2();
        Vector2(const Vector2& v);
        Vector2(float nx, float ny);
        Vector2(const Vector3& v);
        Vector2(const Vector4& v);

    public:
        // Member functions.

        Vector2& operator =(const Vector2& v);
        bool operator ==(const Vector2& v) const;
        bool operator !=(const Vector2& v) const;
        Vector2 operator -() const;
        Vector2 operator +(const Vector2& v) const;
        Vector2 operator -(const Vector2& v) const;
        Vector2 operator *(const Vector2& v) const;
        Vector2 operator *(float n) const;
        Vector2 operator /(float n) const;
        Vector2& operator +=(const Vector2& v);
        Vector2& operator -=(const Vector2& v);
        Vector2& operator *=(const Vector2& v);
        Vector2& operator *=(float n);
        Vector2& operator /=(float n);
        operator Vector3();
        operator Vector4();

        void Set(float nx, float ny);
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
    Vector2 operator *(float n, const Vector2& v);

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

    // Vector 3 class.
    struct Vector3
    {
    public:
        union
        {
            float m[3];
            struct { float x; float y; float z; };
        };

    public:
        Vector3();
        Vector3(const Vector3& v);
        Vector3(float nx, float ny, float nz);
        Vector3(const Vector2& v);
        Vector3(const Vector4& v);

    public:
        // Member functions.

        Vector3& operator =(const Vector3& v);
        bool operator ==(const Vector3& v) const;
        bool operator !=(const Vector3& v) const;
        Vector3 operator -() const;
        Vector3 operator +(const Vector3& v) const;
        Vector3 operator -(const Vector3& v) const;
        Vector3 operator *(const Vector3& v) const;
        Vector3 operator *(float n) const;
        Vector3 operator /(float n) const;
        Vector3& operator +=(const Vector3& v);
        Vector3& operator -=(const Vector3& v);
        Vector3& operator *=(const Vector3& v);
        Vector3& operator *=(float n);
        Vector3& operator /=(float n);
        operator Vector2();
        operator Vector4();

        void Set(float nx, float ny, float nz);
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
    };

    // Vector3 nonmember functions.

    // Vector3 multiply number.
    inline Vector3 operator *(float n, const Vector3& v);

    // Normalize a vector.
    Vector3& Vector3Normalize(Vector3& out, const Vector3& v);

    // The result of two vector3 dot product.
    float Vector3Dot(const Vector3& left, const Vector3& right);

    // The result of two vector3 cross product.
    Vector3& Vector3Cross(Vector3& out, const Vector3& left, const Vector3& right);

    // The result of two vector3 distance.
    float Vector3Distance(const Vector3& left, const Vector3& right);

    // The result of two vector3 squared distance.
    float Vector3SquaredDistance(const Vector3& left, const Vector3& right);

    // Vector 4 class.
    struct Vector4
    {
    public:
        union
        {
            float m[4];
            struct { float x; float y; float z; float w; };
        };

    public:
        Vector4();
        Vector4(const Vector4& v);
        Vector4(float nx, float ny, float nz, float nw);
        Vector4(const Vector2& v);
        Vector4(const Vector3& v);

    public:
        // Member functions.

        Vector4& operator =(const Vector4& v);
        bool operator ==(const Vector4& v) const;
        bool operator !=(const Vector4& v) const;
        Vector4 operator -() const;
        Vector4 operator +(const Vector4& v) const;
        Vector4 operator -(const Vector4& v) const;
        Vector4 operator *(const Vector4& v) const;
        Vector4 operator *(const float n) const;
        Vector4 operator /(const float n) const;
        Vector4& operator +=(const Vector4& n);
        Vector4& operator -=(const Vector4& n);
        Vector4& operator *=(const Vector4& n);
        Vector4& operator *=(const float n);
        Vector4& operator /=(const float n);
        operator Vector2();
        operator Vector3();

        void Set(float nx, float ny, float nz, float nw);
        void Set(const Vector4& v);
        void Reset();
        float Length() const;
        float SquaredLength() const;
        Vector4& Normalize();

        // Static functions.

        inline static Vector4 Zero();
        inline static Vector4 One();
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
}

#include "Vector.inl"

#endif // !__VECTOR_H__
