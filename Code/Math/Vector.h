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
    };
}

#endif // !__VECTOR_H__

