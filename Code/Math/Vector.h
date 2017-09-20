// Add by yaukey at 2017-09-19.
// All vector class.

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "MathUtility.h"

namespace yw
{
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
        Vector2() : x(0.0f), y(0.0f)
        {
        }

        Vector2(const Vector2& o) : x(o.x), y(o.y)
        {
        }

        Vector2(float nx, float ny) : x(nx), y(ny)
        {
        }

    public:
        // Member functions.

        inline Vector2& operator =(const Vector2& o)
        {
            x = o.x;
            y = o.y;
        }

        inline bool operator ==(const Vector2& o) const
        {
            float deltaX = x - o.x;
            if (deltaX > YW_FLOAT_PRECISION || deltaX < -YW_FLOAT_PRECISION)
            {
                return false;
            }

            float deltaY = y - o.y;
            if (deltaY > YW_FLOAT_PRECISION || deltaY < -YW_FLOAT_PRECISION)
            {
                return false;
            }

            return true;
        }

        inline bool operator !=(const Vector2& o) const
        {
            float deltaX = x - o.x;
            if (deltaX > YW_FLOAT_PRECISION || deltaX < -YW_FLOAT_PRECISION)
            {
                return true;
            }

            float deltaY = y - o.y;
            if (deltaY > YW_FLOAT_PRECISION || deltaY < -YW_FLOAT_PRECISION)
            {
                return true;
            }

            return false;
        }

        inline Vector2 operator -() const
        {
            Vector2 value(-x, -y);
            return value;
        }

        inline Vector2 operator +(const Vector2& o) const
        {
            Vector2 value(x + o.x, y + o.y);
            return value;
        }

        inline Vector2 operator -(const Vector2& o) const
        {
            Vector2 value(x - o.x, y - o.y);
            return value;
        }

        inline Vector2 operator *(const Vector2& o) const
        {
            Vector2 value(x * o.x, y * o.y);
            return value;
        }

        inline Vector2 operator *(float o) const
        {
            Vector2 value(x * o, y * o);
            return value;
        }

        inline Vector2 operator /(float o) const
        {
            float oneOverO = 1.0f / o;
            Vector2 value(x * oneOverO, y * oneOverO);
        }

        inline Vector2& operator +=(const Vector2& o)
        {
            x += o.x;
            y += o.y;

            return *this;
        }

        inline Vector2& operator -=(const Vector2& o)
        {
            x -= o.x;
            y -= o.y;

            return *this;
        }

        inline Vector2& operator *=(const Vector2& o)
        {
            x *= o.x;
            y *= o.y;

            return *this;
        }

        inline Vector2& operator *=(float o)
        {
            x *= o;
            y *= o;

            return *this;
        }

        inline Vector2& operator /=(float o)
        {
            float oneOverO = 1.0f / o;
            x *= oneOverO;
            y *= oneOverO;

            return *this;
        }

        inline void Reset()
        {
            x = y = 0.0f;
        }

        inline float Length() const
        {
            float value = sqrt(x * x + y * y);
            return value;
        }

        inline float SquaredLength() const
        {
            float value = x * x + y * y;
            return value;
        }

        inline Vector2& Normalize()
        {
            float length = Length();
            *this /= length;

            return *this;
        }

        // Static functions.

        static inline float Dot(const Vector2& left, const Vector2& right)
        {
            float dot = left.x * right.x + left.y * right.y;
            return dot;
        }

        static inline float Cross(const Vector2& left, const Vector2& right)
        {
            float cross = left.x * right.y - left.y * right.x;
            return cross;
        }

        static inline float Distance(const Vector2& left, const Vector2& right)
        {
            float deltaX = left.x - right.x;
            float deltaY = left.y - right.y;
            float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
            return distance;
        }

        static inline float SquaredDistance(const Vector2& left, const Vector2& right)
        {
            float deltaX = left.x - right.x;
            float deltaY = left.y - right.y;
            float distance = deltaX * deltaX + deltaY * deltaY;
            return distance;
        }

        static inline Vector2 Zero()
        {
            Vector2 zero(0.0f, 0.0f);
            return zero;
        }

        static inline Vector2 One()
        {
            Vector2 one(1.0f, 1.0f);
            return one;
        }
    };

    // Vector2 nonmember functions.

    inline Vector2 operator *(float n, const Vector2& v)
    {
        Vector2 value(n * v.x, n * v.y);
        return value;
    }
}

#endif // !__VECTOR_H__

