// Add by yaukey at 2018-01-21.
// All vector class.

#ifndef __VECTOR_INL__
#define __VECTOR_INL__

#include "MathUtility.h"
#include "Vector.h"

namespace yw
{
    //
    // For Vector2 class.

    // Member functions.

    inline Vector2::Vector2() : x(0.0f), y(0.0f)
    {

    }

    inline Vector2::Vector2(const Vector2& v) : x(v.x), y(v.y)
    {
    }

    inline Vector2::Vector2(float nx, float ny) : x(nx), y(ny)
    {
    }

    inline Vector2::Vector2(const Vector3& v) : x(v.x), y(v.y)
    {
    }

    inline Vector2::Vector2(const Vector4& v) : x(v.x), y(v.y)
    {
    }

    inline Vector2& Vector2::operator =(const Vector2& v)
    {
        x = v.x;
        y = v.y;

        return *this;
    }

    inline bool Vector2::operator ==(const Vector2& v) const
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

        return true;
    }

    inline bool Vector2::operator !=(const Vector2& v) const
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

        return false;
    }

    inline Vector2 Vector2::operator -() const
    {
        Vector2 value(-x, -y);
        return value;
    }

    inline Vector2 Vector2::operator +(const Vector2& v) const
    {
        Vector2 value(x + v.x, y + v.y);
        return value;
    }

    inline Vector2 Vector2::operator -(const Vector2& v) const
    {
        Vector2 value(x - v.x, y - v.y);
        return value;
    }

    inline Vector2 Vector2::operator *(const Vector2& v) const
    {
        Vector2 value(x * v.x, y * v.y);
        return value;
    }

    inline Vector2 Vector2::operator *(float n) const
    {
        Vector2 value(x * n, y * n);
        return value;
    }

    inline Vector2 Vector2::operator /(float n) const
    {
        float oneOverO = 1.0f / n;
        Vector2 value(x * oneOverO, y * oneOverO);
    }

    inline Vector2& Vector2::operator +=(const Vector2& v)
    {
        x += v.x;
        y += v.y;

        return *this;
    }

    inline Vector2& Vector2::operator -=(const Vector2& v)
    {
        x -= v.x;
        y -= v.y;

        return *this;
    }

    inline Vector2& Vector2::operator *=(const Vector2& v)
    {
        x *= v.x;
        y *= v.y;

        return *this;
    }

    inline Vector2& Vector2::operator *=(float n)
    {
        x *= n;
        y *= n;

        return *this;
    }

    inline Vector2& Vector2::operator /=(float n)
    {
        float oneOverO = 1.0f / n;
        x *= oneOverO;
        y *= oneOverO;

        return *this;
    }

    inline Vector2::operator Vector3()
    {
        Vector3 value(x, y, 0.0f);
        return value;
    }

    inline Vector2::operator Vector4()
    {
        Vector4 value(x, y, 0.0f, 0.0f);
        return value;
    }

    inline void Vector2::Set(float nx, float ny)
    {
        x = nx;
        y = ny;
    }

    inline void Vector2::Set(const Vector2& v)
    {
        x = v.x;
        y = v.y;
    }

    inline void Vector2::Reset()
    {
        x = y = 0.0f;
    }

    inline float Vector2::Length() const
    {
        float value = sqrt(x * x + y * y);
        return value;
    }

    inline float Vector2::SquaredLength() const
    {
        float value = x * x + y * y;
        return value;
    }

    inline Vector2& Vector2::Normalize()
    {
        float length = Length();
        *this /= length;

        return *this;
    }

    // Static functions.

    inline Vector2 Vector2::Zero()
    {
        Vector2 zero(0.0f, 0.0f);
        return zero;
    }

    inline Vector2 Vector2::One()
    {
        Vector2 one(1.0f, 1.0f);
        return one;
    }

    inline Vector2 Vector2::Left()
    {
        Vector2 left(-1.0f, 0.0f);
        return left;
    }

    inline Vector2 Vector2::Right()
    {
        Vector2 right(1.0f, 0.0f);
        return right;
    }

    inline Vector2 Vector2::Down()
    {
        Vector2 down(0.0f, -1.0f);
        return down;
    }

    inline Vector2 Vector2::Up()
    {
        Vector2 up(0.0f, 1.0f);
        return up;
    }

    // Non-Member functions.

    inline Vector2 operator *(float n, const Vector2& v)
    {
        Vector2 value(n * v.x, n * v.y);
        return value;
    }

    inline float Vector2Dot(const Vector2& left, const Vector2& right)
    {
        float dot = left.x * right.x + left.y * right.y;
        return dot;
    }

    static inline float Vector2Cross(const Vector2& left, const Vector2& right)
    {
        // The area of quad formed by vector left and right.
        // a*b*sina.
        float cross = left.x * right.y - left.y * right.x;
        return cross;
    }

    inline float Vector2Distance(const Vector2& left, const Vector2& right)
    {
        float deltaX = left.x - right.x;
        float deltaY = left.y - right.y;
        float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
        return distance;
    }

    inline float Vector2SquaredDistance(const Vector2& left, const Vector2& right)
    {
        float deltaX = left.x - right.x;
        float deltaY = left.y - right.y;
        float distance = deltaX * deltaX + deltaY * deltaY;
        return distance;
    }

    //
    // For Vector3 class.

    // Member functions.

    inline Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
    {
    }

    inline Vector3::Vector3(const Vector3& v) : x(v.x), y(v.y), z(v.z)
    {
    }

    inline Vector3::Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz)
    {
    }

    inline Vector3::Vector3(const Vector2& v) : x(v.x), y(v.y), z(0.0f)
    {
    }

    inline Vector3::Vector3(const Vector4& v) : x(v.x), y(v.y), z(v.z)
    {
    }

    inline Vector3& Vector3::operator =(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    inline bool Vector3::operator ==(const Vector3& v) const
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

        return true;
    }

    inline bool Vector3::operator !=(const Vector3& v) const
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

        return false;
    }

    inline Vector3 Vector3::operator -() const
    {
        Vector3 value(-x, -y, -z);
        return value;
    }

    inline Vector3 Vector3::operator +(const Vector3& v) const
    {
        Vector3 value(x + v.x, y + v.y, z + v.z);
        return value;
    }

    inline Vector3 Vector3::operator -(const Vector3& v) const
    {
        Vector3 value(x - v.x, y - v.y, z - v.z);
        return value;
    }

    inline Vector3 Vector3::operator *(const Vector3& v) const
    {
        Vector3 value(x * v.x, y * v.y, z * v.z);
        return value;
    }

    inline Vector3 Vector3::operator *(float n) const
    {
        Vector3 value(x * n, y * n, z * n);
        return value;
    }

    inline Vector3 Vector3::operator /(float n) const
    {
        float oneOverO = 1.0f / n;
        Vector3 value(x * oneOverO, y * oneOverO, z * oneOverO);
    }

    inline Vector3& Vector3::operator +=(const Vector3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    inline Vector3& Vector3::operator -=(const Vector3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    inline Vector3& Vector3::operator *=(const Vector3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;

        return *this;
    }

    inline Vector3& Vector3::operator *=(float n)
    {
        x *= n;
        y *= n;
        z *= n;

        return *this;
    }

    inline Vector3& Vector3::operator /=(float n)
    {
        float oneOverO = 1.0f / n;
        x *= oneOverO;
        y *= oneOverO;
        z *= oneOverO;

        return *this;
    }

    inline Vector3::operator Vector2()
    {
        Vector2 value(x, y);
        return value;
    }

    inline Vector3::operator Vector4()
    {
        Vector4 value(x, y, z, 0.0f);
        return value;
    }

    inline void Vector3::Set(float nx, float ny, float nz)
    {
        x = nx;
        y = ny;
        z = nz;
    }

    inline void Vector3::Set(const Vector3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    inline void Vector3::Reset()
    {
        x = y = z = 0.0f;
    }

    inline float Vector3::Length() const
    {
        float value = sqrt(x * x + y * y + z * z);
        return value;
    }

    inline float Vector3::SquaredLength() const
    {
        float value = x * x + y * y + z * z;
        return value;
    }

    inline Vector3& Vector3::Normalize()
    {
        float length = Length();
        *this /= length;

        return *this;
    }
}

#endif // !__VECTOR_INL__
