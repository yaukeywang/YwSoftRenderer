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
            float mm[4][4];
            float m[16];
            struct
            {
                float _11, _12, _13, _14;
                float _21, _22, _23, _24;
                float _31, _32, _33, _34;
                float _41, _42, _43, _44;
            };
        };

    public:
        inline Matrix44()
        {
            SetIdentity();
        }

        inline Matrix44(const Matrix44& m) :
            _11(m._11), _12(m._12), _13(m._13), _14(m._14),
            _21(m._21), _22(m._22), _23(m._23), _24(m._24),
            _31(m._31), _32(m._32), _33(m._33), _34(m._34),
            _41(m._41), _42(m._42), _43(m._43), _44(m._44)
        {
        }

        inline Matrix44(
            const float a11, const float a12, const float a13, const float a14,
            const float a21, const float a22, const float a23, const float a24,
            const float a31, const float a32, const float a33, const float a34,
            const float a41, const float a42, const float a43, const float a44
        ):
            _11(a11), _12(a12), _13(a13), _14(a14),
            _21(a21), _22(a22), _23(a23), _24(a24),
            _31(a31), _32(a32), _33(a33), _34(a34),
            _41(a41), _42(a42), _43(a43), _44(a44)
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
            return mm[row][col];
        }

        inline Matrix44 operator +() const
        {
            return *this;
        }

        //< Returns the inverse of the matrix.
        inline Matrix44 operator -() const
        {
            return GetInverse();
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

        inline float GetDeterminant() const
        {
            return Determinant4x4(*this);
        }

        inline Matrix44 GetInverse() const
        {
            const float determinant = Determinant4x4(*this);
            if (fabsf(determinant) < FLT_EPSILON)
            {
                return *this;
            }

            Matrix44 mat = Adjoint(*this) / determinant;
            return mat;
        }

        inline void SetIdentity()
        {
            _11 = 1; _12 = 0; _13 = 0; _14 = 0;
            _21 = 0; _22 = 1; _23 = 0; _24 = 0;
            _31 = 0; _32 = 0; _33 = 1; _34 = 0;
            _41 = 0; _42 = 0; _43 = 0; _44 = 1;
        }

    public:
        // Static functions.

        // Get the determinant of matrix 2x2.
        inline static float Determinant2x2(
            const float a11, const float a12,
            const float a21, const float a22
        )
        {
            return a11 * a22 - a12 * a21;
        }

        // Get the determinant of matrix 3x3.
        inline static float Determinant3x3(
            const float a11, const float a12, const float a13,
            const float a21, const float a22, const float a23,
            const float a31, const float a32, const float a33
        )
        {
            return a11 * Determinant2x2(a22, a23, a32, a33) - a12 * Determinant2x2(a21, a23, a31, a33) + a13 * Determinant2x2(a21, a22, a31, a32);
        }

        // Get the determinant of matrix 4x4.
        inline static float Determinant4x4(
            const float a11, const float a12, const float a13, const float a14,
            const float a21, const float a22, const float a23, const float a24,
            const float a31, const float a32, const float a33, const float a34,
            const float a41, const float a42, const float a43, const float a44
        )
        {
            return a11 * Determinant3x3(a22, a23, a24, a32, a33, a34, a42, a43, a44)
                - a12 * Determinant3x3(a21, a23, a24, a31, a33, a34, a41, a43, a44)
                + a13 * Determinant3x3(a21, a22, a24, a31, a32, a34, a41, a42, a44)
                - a14 * Determinant3x3(a21, a22, a23, a31, a32, a33, a41, a42, a43);
        }

        // Get the determinant of matrix 4x4.
        inline static float Determinant4x4(const Matrix44& mat)
        {
            return Determinant4x4(
                mat._11, mat._12, mat._13, mat._14,
                mat._21, mat._22, mat._23, mat._24,
                mat._31, mat._32, mat._33, mat._34,
                mat._41, mat._42, mat._43, mat._44
            );
        }

        // Get the determinant of matrix exclude row and col.
        inline static float MinorDeterminant(const Matrix44& mat, const int32_t row, const int32_t col)
        {
            float mat3x3[3][3];
            for (int32_t r = 0, rn = 0; r < 4; r++)
            {
                if (row == r)
                {
                    continue;
                }

                for (int32_t c = 0, cn = 0; c < 4; c++)
                {
                    if (col == c)
                    {
                        continue;
                    }

                    mat3x3[rn][cn] = mat.mm[r][c];

                    cn++;
                }

                rn++;
            }

            return Determinant3x3(
                mat3x3[0][0], mat3x3[0][1], mat3x3[0][2],
                mat3x3[1][0], mat3x3[1][1], mat3x3[1][2],
                mat3x3[2][0], mat3x3[2][1], mat3x3[2][2]
            );
        }

        // Get Adjoint of matrix.
        inline static Matrix44 Adjoint(const Matrix44& mat)
        {
            Matrix44 matAdjoint;
            for (int32_t r = 0; r < 4; r++)
            {
                for (int32_t c = 0; c < 4; c++)
                {
                    matAdjoint.mm[c][r] = (float)pow(-1, r + c) * MinorDeterminant(mat, r, c);
                }
            }

            return matAdjoint;
        }

        // Get inverse of matrix.
        inline static bool Inverse(Matrix44& out, const Matrix44& mat)
        {
            const float determinant = Determinant4x4(mat);
            if (fabsf(determinant) < FLT_EPSILON)
            {
                return false;
            }

            out = Adjoint(mat) / determinant;
            return true;
        }

        // Get transpose of matrix.
        inline static Matrix44& Transpose(Matrix44& out, const Matrix44& mat)
        {
            out._11 = mat._11; out._12 = mat._21; out._13 = mat._31; out._14 = mat._41;
            out._21 = mat._12; out._22 = mat._22; out._23 = mat._32; out._24 = mat._42;
            out._31 = mat._13; out._32 = mat._23; out._33 = mat._33; out._34 = mat._43;
            out._41 = mat._14; out._42 = mat._24; out._43 = mat._34; out._44 = mat._44;

            return out;
        }

        // Get identity of matrix 4x4.
        inline static Matrix44& Identity(Matrix44& out)
        {
            out._11 = 1; out._12 = 0; out._13 = 0; out._14 = 0;
            out._21 = 0; out._22 = 1; out._23 = 0; out._24 = 0;
            out._31 = 0; out._32 = 0; out._33 = 1; out._34 = 0;
            out._41 = 0; out._42 = 0; out._43 = 0; out._44 = 1;

            return out;
        }
    };
}

#endif // !__MATRIX_H__
