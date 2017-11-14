// Add by yaukey at 2017-11-14.
// Matrix class.

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "System/Support/Base.h"
#include "MathUtility.h"

namespace yw
{
    struct Matrix44
    {
    public:
        union
        {
            struct
            {
                float _11, _12, _13, _14;
                float _21, _22, _23, _24;
                float _31, _32, _33, _34;
                float _41, _42, _43, _44;
            };

            float m[4][4];
        };

    public:
        inline Matrix44()
        {
        }

        inline Matrix44(const Matrix44& m) :
            _11(m._11), _12(m._12), _13(m._13), _14(m._14),
            _21(m._21), _22(m._22), _23(m._23), _24(m._24),
            _31(m._31), _32(m._32), _33(m._33), _34(m._34),
            _41(m._41), _42(m._42), _43(m._43), _44(m._44)
        {
        }

        inline Matrix44(
            const float i11, const float i12, const float i13, const float i14,
            const float i21, const float i22, const float i23, const float i24,
            const float i31, const float i32, const float i33, const float i34,
            const float i41, const float i42, const float i43, const float i44
        ):
            _11(i11), _12(i12), _13(i13), _14(i14),
            _21(i21), _22(i22), _23(i23), _24(i24),
            _31(i31), _32(i32), _33(i33), _34(i34),
            _41(i41), _42(i42), _43(i43), _44(i44)
        {
        }

    public:
        inline operator float*()
        {
            return &_11;
        }

        operator const float*() const
        {
            return &_11;
        }

        float operator()(int32_t row, int32_t col) const
        {
            return m[row][col];
        }

        inline Matrix44 operator +() const
        {
            return *this;
        }

        //< Returns the inverse of the matrix.
        inline Matrix44 operator -() const
        {

        }

        inline Matrix44& operator =(const Matrix44& m)
        {
            _11 = m._11; _12 = m._12; _13 = m._13; _14 = m._14;
            _21 = m._21; _22 = m._22; _23 = m._23; _24 = m._24;
            _31 = m._31; _32 = m._32; _33 = m._33; _34 = m._34;
            _41 = m._41; _42 = m._42; _43 = m._43; _44 = m._44;

            return *this;
        }

        inline Matrix44 operator +(const Matrix44 &m) const
        {
            Matrix44 value(
                _11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
                _21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
                _31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
                _41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44
            );
            return value;
        }

        inline Matrix44 operator -(const Matrix44 &m) const
        {
            Matrix44 value(
                _11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
                _21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
                _31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
                _41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44
            );
            return value;
        }

        inline Matrix44 operator *(const Matrix44 &m) const
        {
            Matrix44 value(
                _11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41,
                _11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42,
                _11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43,
                _11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44,
                _21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41,
                _21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42,
                _21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43,
                _21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44,
                _31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41,
                _31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42,
                _31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43,
                _31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44,
                _41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41,
                _41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42,
                _41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43,
                _41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44
            );
            return value;
        }

        inline Matrix44 operator *(const float n) const
        {
            Matrix44 value(
                _11 * n, _12 * n, _13 * n, _14 * n,
                _21 * n, _22 * n, _23 * n, _24 * n,
                _31 * n, _32 * n, _33 * n, _34 * n,
                _41 * n, _42 * n, _43 * n, _44 * n
            );
            return value;
        }

        inline Matrix44 operator /(const float n) const
        {
            const float oneOverN = 1.0f / n;
            Matrix44 value(
                _11 * oneOverN, _12 * oneOverN, _13 * oneOverN, _14 * oneOverN,
                _21 * oneOverN, _22 * oneOverN, _23 * oneOverN, _24 * oneOverN,
                _31 * oneOverN, _32 * oneOverN, _33 * oneOverN, _34 * oneOverN,
                _41 * oneOverN, _42 * oneOverN, _43 * oneOverN, _44 * oneOverN
            );

            return value;
        }

        inline Matrix44& operator +=(const Matrix44& m)
        {
            _11 += m._11; _12 += m._12; _13 += m._13; _14 += m._14;
            _21 += m._21; _22 += m._22; _23 += m._23; _24 += m._24;
            _31 += m._31; _32 += m._32; _33 += m._33; _34 += m._34;
            _41 += m._41; _42 += m._42; _43 += m._43; _44 += m._44;

            return *this;
        }

        inline Matrix44& operator -=(const Matrix44& m)
        {
            _11 -= m._11; _12 -= m._12; _13 -= m._13; _14 -= m._14;
            _21 -= m._21; _22 -= m._22; _23 -= m._23; _24 -= m._24;
            _31 -= m._31; _32 -= m._32; _33 -= m._33; _34 -= m._34;
            _41 -= m._41; _42 -= m._42; _43 -= m._43; _44 -= m._44;

            return *this;
        }

        inline Matrix44& operator *=(const Matrix44& m)
        {
            const float f11 = _11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41;
            const float f12 = _11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42;
            const float f13 = _11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43;
            const float f14 = _11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44;
            const float f21 = _21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41;
            const float f22 = _21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42;
            const float f23 = _21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43;
            const float f24 = _21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44;
            const float f31 = _31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41;
            const float f32 = _31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42;
            const float f33 = _31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43;
            const float f34 = _31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44;
            const float f41 = _41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41;
            const float f42 = _41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42;
            const float f43 = _41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43;
            const float f44 = _41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44;

            _11 = f11; _12 = f12; _13 = f13; _14 = f14;
            _21 = f21; _22 = f22; _23 = f23; _24 = f24;
            _31 = f31; _32 = f32; _33 = f33; _34 = f34;
            _41 = f41; _42 = f42; _43 = f43; _44 = f44;

            return *this;
        }

        inline Matrix44& operator *=(const float n)
        {
            _11 *= n; _12 *= n; _13 *= n; _14 *= n;
            _21 *= n; _22 *= n; _23 *= n; _24 *= n;
            _31 *= n; _32 *= n; _33 *= n; _34 *= n;
            _41 *= n; _42 *= n; _43 *= n; _44 *= n;

            return *this;
        }

        inline Matrix44& operator /=(const float n)
        {
            const float oneOverN = 1.0f / n;
            _11 *= oneOverN; _12 *= oneOverN; _13 *= oneOverN; _14 *= oneOverN;
            _21 *= oneOverN; _22 *= oneOverN; _23 *= oneOverN; _24 *= oneOverN;
            _31 *= oneOverN; _32 *= oneOverN; _33 *= oneOverN; _34 *= oneOverN;
            _41 *= oneOverN; _42 *= oneOverN; _43 *= oneOverN; _44 *= oneOverN;

            return *this;
        }
    };
}

#endif // !__MATRIX_H__
