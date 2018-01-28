// Add by yaukey at 2018-01-21.
// All vector class.

#ifndef __VECTOR_INL__
#define __VECTOR_INL__

#include "MathUtility.h"
#include "Vector.h"
#include "Matrix.h"

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

    inline Vector2 Vector2::operator *(const float n) const
    {
        Vector2 value(x * n, y * n);
        return value;
    }

    inline Vector2 Vector2::operator /(const float n) const
    {
        float oneOverO = 1.0f / n;
        Vector2 value(x * oneOverO, y * oneOverO);

        return value;
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

    inline Vector2& Vector2Normalize(Vector2& out, const Vector2& v)
    {
        float length = v.Length();
        out = v / length;

        return out;
    }

    inline float Vector2Dot(const Vector2& left, const Vector2& right)
    {
        float dot = left.x * right.x + left.y * right.y;
        return dot;
    }

    inline float Vector2Cross(const Vector2& left, const Vector2& right)
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

    inline Vector3 Vector3::operator *(const float n) const
    {
        Vector3 value(x * n, y * n, z * n);
        return value;
    }

    inline Vector3 Vector3::operator *(const Matrix33& mat) const
    {
        Vector3 value(
            x * mat._11 + y * mat._21 + z * mat._31, 
            y * mat._12 + y * mat._22 + z * mat._32, 
            z * mat._13 + y * mat._23 + z * mat._33
        );
        return value;
    }

    inline Vector3 Vector3::operator /(const float n) const
    {
        float oneOverO = 1.0f / n;
        Vector3 value(x * oneOverO, y * oneOverO, z * oneOverO);

        return value;
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

    inline Vector3& Vector3::operator *=(const Matrix33& mat)
    {
        float nx = x * mat._11 + y * mat._21 + z * mat._31;
        float ny = y * mat._12 + y * mat._22 + z * mat._32;
        float nz = z * mat._13 + y * mat._23 + z * mat._33;

        x = nx;
        y = ny;
        z = nz;

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

    // Static functions.

    inline Vector3 Vector3::Zero()
    {
        Vector3 zero(0.0f, 0.0f, 0.0f);
        return zero;
    }

    inline Vector3 Vector3::One()
    {
        Vector3 one(1.0f, 1.0f, 1.0f);
        return one;
    }

    inline Vector3 Vector3::Left()
    {
        Vector3 left(-1.0f, 0.0f, 0.0f);
        return left;
    }

    inline Vector3 Vector3::Right()
    {
        Vector3 right(1.0f, 0.0f, 0.0f);
        return right;
    }

    inline Vector3 Vector3::Down()
    {
        Vector3 down(0.0f, -1.0f, 0.0f);
        return down;
    }

    inline Vector3 Vector3::Up()
    {
        Vector3 up(0.0f, 1.0f, 0.0f);
        return up;
    }

    inline Vector3 Vector3::Back()
    {
        Vector3 back(0.0f, 0.0f, -1.0f);
        return back;
    }

    inline Vector3 Vector3::Forward()
    {
        Vector3 forward(0.0f, 0.0f, 1.0f);
        return forward;
    }

    // Nonmember functions.

    inline Vector3 operator *(float n, const Vector3& v)
    {
        Vector3 value(n * v.x, n * v.y, n * v.z);
        return value;
    }

    inline Vector3& Vector3Normalize(Vector3& out, const Vector3& v)
    {
        float length = v.Length();
        out = v / length;

        return out;
    }

    inline float Vector3Dot(const Vector3& left, const Vector3& right)
    {
        float dot = left.x * right.x + left.y * right.y + left.z * right.z;
        return dot;
    }

    inline Vector3& Vector3Cross(Vector3& out, const Vector3& left, const Vector3& right)
    {
        out.Set(
            left.y * right.z - left.z * right.y,
            left.z * right.x - left.x * right.z,
            left.x * right.y - left.y * right.x
        );

        return out;
    }

    inline float Vector3Distance(const Vector3& left, const Vector3& right)
    {
        float deltaX = left.x - right.x;
        float deltaY = left.y - right.y;
        float deltaZ = left.z - right.z;
        float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
        return distance;
    }

    inline float Vector3SquaredDistance(const Vector3& left, const Vector3& right)
    {
        float deltaX = left.x - right.x;
        float deltaY = left.y - right.y;
        float deltaZ = left.z - right.z;
        float distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
        return distance;
    }

    inline Vector3& Vector3Transform(Vector3& out, const Vector3& v, const Matrix44& m)
    {
        Vector4 value(v.x, v.y, v.z, 1.0f);
        value *= m;
        out = value;

        return out;
    }

    inline Vector3& Vector3TransformCoord(Vector3& out, const Vector3& v, const Matrix44& m)
    {
        Vector4 value(v.x, v.y, v.z, 1.0f);
        value *= m;
        value /= value.w;
        out = value;

        return out;
    }

    inline Vector3& Vector3TransformNormal(Vector3& out, const Vector3& v, const Matrix44& m)
    {
        Matrix44 matTrs;
        Matrix44Inverse(matTrs, m);
        Matrix44Transpose(matTrs, matTrs);

        Vector4 value(v.x, v.y, v.z, 0.0f);
        value *= matTrs;
        out = value;

        return out;
    }

    //
    // For Vector2 class.

    // Member functions.

    inline Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
    {
    }

    inline Vector4::Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w)
    {
    }

    inline Vector4::Vector4(float nx, float ny, float nz, float nw) : x(nx), y(ny), z(nz), w(nw)
    {
    }

    inline Vector4::Vector4(const Vector2& v) : x(v.x), y(v.y), z(0.0f), w(0.0f)
    {
    }

    inline Vector4::Vector4(const Vector3& v) : x(v.x), y(v.y), z(v.z), w(0.0f)
    {
    }

    inline Vector4& Vector4::operator =(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;

        return *this;
    }

    inline bool Vector4::operator ==(const Vector4& v) const
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

    inline bool Vector4::operator !=(const Vector4& v) const
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

    inline Vector4 Vector4::operator -() const
    {
        Vector4 value(-x, -y, -z, -w);
        return value;
    }

    inline Vector4 Vector4::operator +(const Vector4& v) const
    {
        Vector4 value(x + v.x, y + v.y, z + v.z, w + v.w);
        return value;
    }

    inline Vector4 Vector4::operator -(const Vector4& v) const
    {
        Vector4 value(x - v.x, y - v.y, z - v.z, w - v.w);
        return value;
    }

    inline Vector4 Vector4::operator *(const Vector4& v) const
    {
        Vector4 value(x * v.x, y * v.y, z * v.z, w * v.w);
        return value;
    }

    inline Vector4 Vector4::operator *(const float n) const
    {
        Vector4 value(x * n, y * n, z * n, w * n);
        return value;
    }

    inline Vector4 Vector4::operator *(const Matrix44& mat) const
    {
        Vector4 value(
            x * mat._11 + y * mat._21 + z * mat._31 + w * mat._41,
            y * mat._12 + y * mat._22 + z * mat._32 + w * mat._42,
            z * mat._13 + y * mat._23 + z * mat._33 + w * mat._43,
            w * mat._14 + y * mat._24 + z * mat._34 + w * mat._44
        );
        return value;
    }

    inline Vector4 Vector4::operator /(const float n) const
    {
        float oneOverO = 1.0f / n;
        Vector4 value(x * oneOverO, y * oneOverO, z * oneOverO, w * oneOverO);

        return value;
    }

    inline Vector4& Vector4::operator +=(const Vector4& n)
    {
        x += n.x;
        y += n.y;
        z += n.z;
        w += n.w;

        return *this;
    }

    inline Vector4& Vector4::operator -=(const Vector4& n)
    {
        x -= n.x;
        y -= n.y;
        z -= n.z;
        w -= n.w;

        return *this;
    }

    inline Vector4& Vector4::operator *=(const Vector4& n)
    {
        x *= n.x;
        y *= n.y;
        z *= n.z;
        w *= n.w;

        return *this;
    }

    inline Vector4& Vector4::operator *=(const float n)
    {
        x *= n;
        y *= n;
        z *= n;
        w *= n;

        return *this;
    }

    inline Vector4& Vector4::operator /=(const float n)
    {
        float oneOverO = 1.0f / n;
        x *= oneOverO;
        y *= oneOverO;
        z *= oneOverO;
        w *= oneOverO;

        return *this;
    }

    inline Vector4& Vector4::operator *=(const Matrix44& mat)
    {
        float nx = x * mat._11 + y * mat._21 + z * mat._31 + w * mat._41;
        float ny = y * mat._12 + y * mat._22 + z * mat._32 + w * mat._42;
        float nz = z * mat._13 + y * mat._23 + z * mat._33 + w * mat._43;
        float nw = w * mat._14 + y * mat._24 + z * mat._34 + w * mat._44;

        x = nx;
        y = ny;
        z = nz;
        w = nw;

        return *this;
    }

    inline Vector4::operator Vector2()
    {
        Vector2 value(x, y);
        return value;
    }

    inline Vector4::operator Vector3()
    {
        Vector3 value(x, y, z);
        return value;
    }

    inline void Vector4::Set(float nx, float ny, float nz, float nw)
    {
        x = nx;
        y = ny;
        z = nz;
        w = nw;
    }

    inline void Vector4::Set(const Vector4& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }

    inline void Vector4::Reset()
    {
        x = y = z = w = 0.0f;
    }

    inline float Vector4::Length() const
    {
        float value = sqrt(x * x + y * y + z * z + w * w);
        return value;
    }

    inline float Vector4::SquaredLength() const
    {
        float value = x * x + y * y + z * z + w * w;
        return value;
    }

    inline Vector4& Vector4::Normalize()
    {
        float length = Length();
        *this /= length;

        return *this;
    }

    // Static functions.

    inline Vector4 Vector4::Zero()
    {
        Vector4 zero(0.0f, 0.0f, 0.0f, 0.0f);
        return zero;
    }

    inline Vector4 Vector4::One()
    {
        Vector4 one(1.0f, 1.0f, 1.0f, 1.0f);
        return one;
    }

    // Nonmember functions.

    inline Vector4 operator *(const float n, const Vector4& v)
    {
        Vector4 value(n * v.x, n * v.y, n * v.z, n * v.w);
        return value;
    }

    inline Vector4& Vector4Normalize(Vector4& out, const Vector4& v)
    {
        float length = v.Length();
        out = v / length;

        return out;
    }

    inline float Vector4Dot(const Vector4& left, const Vector4& right)
    {
        float dot = left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
        return dot;
    }

    inline Vector4& Vector4Cross(Vector4& out, const Vector4& left, const Vector4& right)
    {
        out.Set(
            left.y * right.z - left.z * right.y,
            left.z * right.x - left.x * right.z,
            left.x * right.y - left.y * right.x,
            0.0f
        );
        return out;
    }

    inline float Vector4Distance(const Vector4& left, const Vector4& right)
    {
        float deltaX = left.x - right.x;
        float deltaY = left.y - right.y;
        float deltaZ = left.z - right.z;
        float deltaW = left.w - right.w;
        float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ + deltaW * deltaW);
        return distance;
    }

    inline float Vector4SquaredDistance(const Vector4& left, const Vector4& right)
    {
        float deltaX = left.x - right.x;
        float deltaY = left.y - right.y;
        float deltaZ = left.z - right.z;
        float deltaW = left.w - right.w;
        float distance = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ + deltaW * deltaW;
        return distance;
    }
}

#endif // !__VECTOR_INL__
