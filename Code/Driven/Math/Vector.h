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

    Vector2 operator *(float n, const Vector2& v);
    float Vector2Dot(const Vector2& left, const Vector2& right);
    float Vector2Cross(const Vector2& left, const Vector2& right);
    float Vector2Distance(const Vector2& left, const Vector2& right);
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

        static inline float Dot(const Vector3& left, const Vector3& right)
        {
            float dot = left.x * right.x + left.y * right.y + left.z * right.z;
            return dot;
        }

        static inline Vector3& Cross(Vector3& out, const Vector3& left, const Vector3& right)
        {
            out.Set(
                left.y * right.z - left.z * right.y,
                left.z * right.x - left.x * right.z,
                left.x * right.y - left.y * right.x
            );

            return out;
        }

        static inline float Distance(const Vector3& left, const Vector3& right)
        {
            float deltaX = left.x - right.x;
            float deltaY = left.y - right.y;
            float deltaZ = left.z - right.z;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
            return distance;
        }

        static inline float SquaredDistance(const Vector3& left, const Vector3& right)
        {
            float deltaX = left.x - right.x;
            float deltaY = left.y - right.y;
            float deltaZ = left.z - right.z;
            float distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
            return distance;
        }

        static inline Vector3 Zero()
        {
            Vector3 zero(0.0f, 0.0f, 0.0f);
            return zero;
        }

        static inline Vector3 One()
        {
            Vector3 one(1.0f, 1.0f, 1.0f);
            return one;
        }

        static inline Vector3 Left()
        {
            Vector3 left(-1.0f, 0.0f, 0.0f);
            return left;
        }

        static inline Vector3 Right()
        {
            Vector3 right(1.0f, 0.0f, 0.0f);
            return right;
        }

        static inline Vector3 Down()
        {
            Vector3 down(0.0f, -1.0f, 0.0f);
            return down;
        }

        static inline Vector3 Up()
        {
            Vector3 up(0.0f, 1.0f, 0.0f);
            return up;
        }

        static inline Vector3 Back()
        {
            Vector3 back(0.0f, 0.0f, -1.0f);
            return back;
        }

        static inline Vector3 Forward()
        {
            Vector3 forward(0.0f, 0.0f, 1.0f);
            return forward;
        }
    };

    // Vector3 nonmember functions.

    inline Vector3 operator *(float n, const Vector3& v)
    {
        Vector3 value(n * v.x, n * v.y, n * v.z);
        return value;
    }

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
        inline Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {
        }

        inline Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w)
        {
        }

        inline Vector4(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw)
        {
        }

        inline Vector4(const Vector2& v) : x(v.x), y(v.y), z(0.0f), w(0.0f)
        {
        }

        inline Vector4(const Vector3& v) : x(v.x), y(v.y), z(v.z), w(0.0f)
        {
        }

    public:
        // Member functions.

        inline Vector4& operator =(const Vector4& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;

            return *this;
        }

        inline bool operator ==(const Vector4& v) const
        {
            float deltaX = x - v.x;
            if (deltaX > YW_FLOAT_PRECISION || deltaX < -YW_FLOAT_PRECISION)
            {
                return false;
            }

            float deltaY = y - v.y;
            if (deltaY > YW_FLOAT_PRECISION || deltaY < -YW_FLOAT_PRECISION)
            {
                return false;
            }

            float deltaZ = z - v.z;
            if (deltaZ > YW_FLOAT_PRECISION || deltaZ < -YW_FLOAT_PRECISION)
            {
                return false;
            }

            float deltaW = w - v.w;
            if (deltaW > YW_FLOAT_PRECISION || deltaW < -YW_FLOAT_PRECISION)
            {
                return false;
            }

            return true;
        }

        inline bool operator !=(const Vector4& v) const
        {
            float deltaX = x - v.x;
            if (deltaX > YW_FLOAT_PRECISION || deltaX < -YW_FLOAT_PRECISION)
            {
                return true;
            }

            float deltaY = y - v.y;
            if (deltaY > YW_FLOAT_PRECISION || deltaY < -YW_FLOAT_PRECISION)
            {
                return true;
            }

            float deltaZ = z - v.z;
            if (deltaZ > YW_FLOAT_PRECISION || deltaZ < -YW_FLOAT_PRECISION)
            {
                return true;
            }

            float deltaW = w - v.w;
            if (deltaW > YW_FLOAT_PRECISION || deltaW < -YW_FLOAT_PRECISION)
            {
                return true;
            }

            return false;
        }

        inline Vector4 operator -() const
        {
            Vector4 value(-x, -y, -z, -w);
            return value;
        }

        inline Vector4 operator +(const Vector4& v) const
        {
            Vector4 value(x + v.x, y + v.y, z + v.z, w + v.w);
            return value;
        }

        inline Vector4 operator -(const Vector4& v) const
        {
            Vector4 value(x - v.x, y - v.y, z - v.z, w - v.w);
            return value;
        }

        inline Vector4 operator *(const Vector4& v) const
        {
            Vector4 value(x * v.x, y * v.y, z * v.z, w * v.w);
            return value;
        }

        inline Vector4 operator *(const float n) const
        {
            Vector4 value(x * n, y * n, z * n, w * n);
            return value;
        }

        inline Vector4 operator /(const float n) const
        {
            float oneOverO = 1.0f / n;
            Vector4 value(x * oneOverO, y * oneOverO, z * oneOverO, w * oneOverO);
        }

        inline Vector4& operator +=(const Vector4& n)
        {
            x += n.x;
            y += n.y;
            z += n.z;
            w += n.w;

            return *this;
        }

        inline Vector4& operator -=(const Vector4& n)
        {
            x -= n.x;
            y -= n.y;
            z -= n.z;
            w -= n.w;

            return *this;
        }

        inline Vector4& operator *=(const Vector4& n)
        {
            x *= n.x;
            y *= n.y;
            z *= n.z;
            w *= n.w;

            return *this;
        }

        inline Vector4& operator *=(const float n)
        {
            x *= n;
            y *= n;
            z *= n;
            w *= n;

            return *this;
        }

        inline Vector4& operator /=(const float n)
        {
            float oneOverO = 1.0f / n;
            x *= oneOverO;
            y *= oneOverO;
            z *= oneOverO;
            w *= oneOverO;

            return *this;
        }

        inline operator Vector2()
        {
            Vector2 value(x, y);
            return value;
        }

        inline operator Vector3()
        {
            Vector3 value(x, y, z);
            return value;
        }

        inline void Set(float nx, float ny, float nz, float nw)
        {
            x = nx;
            y = ny;
            z = nz;
            w = nw;
        }

        inline void Set(const Vector4& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
        }

        inline void Reset()
        {
            x = y = z = w = 0.0f;
        }

        inline float Length() const
        {
            float value = sqrt(x * x + y * y + z * z + w * w);
            return value;
        }

        inline float SquaredLength() const
        {
            float value = x * x + y * y + z * z + w * w;
            return value;
        }

        inline Vector4& Normalize()
        {
            float length = Length();
            *this /= length;

            return *this;
        }

        // Static functions.

        inline static float Dot(const Vector4& left, const Vector4& right)
        {
            float dot = left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
            return dot;
        }

        inline static Vector4& Cross(Vector4& out, const Vector4& left, const Vector4& right)
        {
            out.Set(
                left.y * right.z - left.z * right.y,
                left.z * right.x - left.x * right.z,
                left.x * right.y - left.y * right.x,
                0.0f
            );
            return out;
        }

        inline static float Distance(const Vector4& left, const Vector4& right)
        {
            float deltaX = left.x - right.x;
            float deltaY = left.y - right.y;
            float deltaZ = left.z - right.z;
            float deltaW = left.w - right.w;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ + deltaW * deltaW);
            return distance;
        }

        inline static float SquaredDistance(const Vector4& left, const Vector4& right)
        {
            float deltaX = left.x - right.x;
            float deltaY = left.y - right.y;
            float deltaZ = left.z - right.z;
            float deltaW = left.w - right.w;
            float distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ + deltaW * deltaW;
            return distance;
        }

        inline static Vector4 Zero()
        {
            Vector4 zero(0.0f, 0.0f, 0.0f, 0.0f);
            return zero;
        }

        inline static Vector4 One()
        {
            Vector4 one(1.0f, 1.0f, 1.0f, 1.0f);
            return one;
        }
    };

    // Vector4 nonmember functions.

    inline Vector4 operator *(const float n, const Vector4& v)
    {
        Vector4 value(n * v.x, n * v.y, n * v.z, n * v.w);
        return value;
    }
}

#include "Vector.inl"

#endif // !__VECTOR_H__
