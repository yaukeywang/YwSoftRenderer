// Add by yaukey at 2018-01-22.
// All matrix class.

#ifndef __MATRIX_INL__
#define __MATRIX_INL__

#include "YwMathUtility.h"
#include "YwMathMatrix.h"
#include "YwMathQuaternion.h"
#include "YwMathVector.h"

namespace yw
{
    //
    // Matrix common functions.

    // Get the determinant of matrix 2x2.
    inline float Matrix22Determinant(
        const float a11, const float a12,
        const float a21, const float a22
    )
    {
        return a11 * a22 - a12 * a21;
    }

    // Get the determinant of matrix 3x3.
    inline float Matrix33Determinant(
        const float a11, const float a12, const float a13,
        const float a21, const float a22, const float a23,
        const float a31, const float a32, const float a33
    )
    {
        return a11 * Matrix22Determinant(a22, a23, a32, a33) - a12 * Matrix22Determinant(a21, a23, a31, a33) + a13 * Matrix22Determinant(a21, a22, a31, a32);
    }

    // Get the determinant of matrix 4x4.
    inline float Matrix44Determinant(
        const float a11, const float a12, const float a13, const float a14,
        const float a21, const float a22, const float a23, const float a24,
        const float a31, const float a32, const float a33, const float a34,
        const float a41, const float a42, const float a43, const float a44
    )
    {
        return a11 * Matrix33Determinant(a22, a23, a24, a32, a33, a34, a42, a43, a44)
            - a12 * Matrix33Determinant(a21, a23, a24, a31, a33, a34, a41, a43, a44)
            + a13 * Matrix33Determinant(a21, a22, a24, a31, a32, a34, a41, a42, a44)
            - a14 * Matrix33Determinant(a21, a22, a23, a31, a32, a33, a41, a42, a43);
    }

    //
    // For Matrix33 class.
    inline Matrix33::Matrix33()
    {
        SetIdentity();
    }

    inline Matrix33::Matrix33(const Matrix33& mat) :
        _11(mat._11), _12(mat._12), _13(mat._13),
        _21(mat._21), _22(mat._22), _23(mat._23),
        _31(mat._31), _32(mat._32), _33(mat._33)
    {
    }

    inline Matrix33::Matrix33(
        const float a11, const float a12, const float a13,
        const float a21, const float a22, const float a23,
        const float a31, const float a32, const float a33
    ) :
        _11(a11), _12(a12), _13(a13),
        _21(a21), _22(a22), _23(a23),
        _31(a31), _32(a32), _33(a33)
    {
    }

    // Member functions.

    inline Matrix33::operator float*()
    {
        return &_11;
    }

    inline Matrix33::operator const float*() const
    {
        return &_11;
    }

    inline float Matrix33::operator()(int32_t row, int32_t col) const
    {
        return mm[row][col];
    }

    inline Matrix33 Matrix33::operator +() const
    {
        return *this;
    }

    inline Matrix33 Matrix33::operator -() const
    {
        return GetInverse();
    }

    inline Matrix33& Matrix33::operator =(const Matrix33& mat)
    {
        _11 = mat._11; _12 = mat._12; _13 = mat._13;
        _21 = mat._21; _22 = mat._22; _23 = mat._23;
        _31 = mat._31; _32 = mat._32; _33 = mat._33;

        return *this;
    }

    inline Matrix33 Matrix33::operator +(const Matrix33 &mat) const
    {
        Matrix33 value(
            _11 + mat._11, _12 + mat._12, _13 + mat._13,
            _21 + mat._21, _22 + mat._22, _23 + mat._23,
            _31 + mat._31, _32 + mat._32, _33 + mat._33
        );
        return value;
    }

    inline Matrix33 Matrix33::operator -(const Matrix33 &mat) const
    {
        Matrix33 value(
            _11 - mat._11, _12 - mat._12, _13 - mat._13,
            _21 - mat._21, _22 - mat._22, _23 - mat._23,
            _31 - mat._31, _32 - mat._32, _33 - mat._33
        );
        return value;
    }

    inline Matrix33 Matrix33::operator *(const Matrix33 &mat) const
    {
        Matrix33 value(
            _11 * mat._11 + _12 * mat._21 + _13 * mat._31,
            _11 * mat._12 + _12 * mat._22 + _13 * mat._32,
            _11 * mat._13 + _12 * mat._23 + _13 * mat._33,

            _21 * mat._11 + _22 * mat._21 + _23 * mat._31,
            _21 * mat._12 + _22 * mat._22 + _23 * mat._32,
            _21 * mat._13 + _22 * mat._23 + _23 * mat._33,

            _31 * mat._11 + _32 * mat._21 + _33 * mat._31,
            _31 * mat._12 + _32 * mat._22 + _33 * mat._32,
            _31 * mat._13 + _32 * mat._23 + _33 * mat._33
        );
        return value;
    }

    inline Matrix33 Matrix33::operator *(const float n) const
    {
        Matrix33 value(
            _11 * n, _12 * n, _13 * n,
            _21 * n, _22 * n, _23 * n,
            _31 * n, _32 * n, _33 * n
        );
        return value;
    }

    inline Matrix33 Matrix33::operator /(const float n) const
    {
        const float oneOverN = 1.0f / n;
        Matrix33 value(
            _11 * oneOverN, _12 * oneOverN, _13 * oneOverN,
            _21 * oneOverN, _22 * oneOverN, _23 * oneOverN,
            _31 * oneOverN, _32 * oneOverN, _33 * oneOverN
        );

        return value;
    }

    inline Matrix33& Matrix33::operator +=(const Matrix33& mat)
    {
        _11 += mat._11; _12 += mat._12; _13 += mat._13;
        _21 += mat._21; _22 += mat._22; _23 += mat._23;
        _31 += mat._31; _32 += mat._32; _33 += mat._33;

        return *this;
    }

    inline Matrix33& Matrix33::operator -=(const Matrix33& mat)
    {
        _11 -= mat._11; _12 -= mat._12; _13 -= mat._13;
        _21 -= mat._21; _22 -= mat._22; _23 -= mat._23;
        _31 -= mat._31; _32 -= mat._32; _33 -= mat._33;

        return *this;
    }

    inline Matrix33& Matrix33::operator *=(const Matrix33& mat)
    {
        const float f11 = _11 * mat._11 + _12 * mat._21 + _13 * mat._31;
        const float f12 = _11 * mat._12 + _12 * mat._22 + _13 * mat._32;
        const float f13 = _11 * mat._13 + _12 * mat._23 + _13 * mat._33;

        const float f21 = _21 * mat._11 + _22 * mat._21 + _23 * mat._31;
        const float f22 = _21 * mat._12 + _22 * mat._22 + _23 * mat._32;
        const float f23 = _21 * mat._13 + _22 * mat._23 + _23 * mat._33;

        const float f31 = _31 * mat._11 + _32 * mat._21 + _33 * mat._31;
        const float f32 = _31 * mat._12 + _32 * mat._22 + _33 * mat._32;
        const float f33 = _31 * mat._13 + _32 * mat._23 + _33 * mat._33;

        _11 = f11; _12 = f12; _13 = f13;
        _21 = f21; _22 = f22; _23 = f23;
        _31 = f31; _32 = f32; _33 = f33;

        return *this;
    }

    inline Matrix33& Matrix33::operator *=(const float n)
    {
        _11 *= n; _12 *= n; _13 *= n;
        _21 *= n; _22 *= n; _23 *= n;
        _31 *= n; _32 *= n; _33 *= n;

        return *this;
    }

    inline Matrix33& Matrix33::operator /=(const float n)
    {
        const float oneOverN = 1.0f / n;
        _11 *= oneOverN; _12 *= oneOverN; _13 *= oneOverN;
        _21 *= oneOverN; _22 *= oneOverN; _23 *= oneOverN;
        _31 *= oneOverN; _32 *= oneOverN; _33 *= oneOverN;

        return *this;
    }

    inline float Matrix33::GetDeterminant() const
    {
        return Matrix33Determinant(*this);
    }

    inline Matrix33 Matrix33::GetInverse() const
    {
        Matrix33 out;
        if (!Matrix33Inverse(out, *this))
        {
            return *this;
        }

        return out;
    }

    inline void Matrix33::SetIdentity()
    {
        Matrix33Identity(*this);
    }

    // Matrix33 nonmember functions.

    // Get the determinant of matrix 4x4.
    inline float Matrix33Determinant(const Matrix33& mat)
    {
        return Matrix33Determinant(
            mat._11, mat._12, mat._13,
            mat._21, mat._22, mat._23,
            mat._31, mat._32, mat._33
        );
    }

    inline float Matrix33MinorDeterminant(const Matrix33& mat, const int32_t row, const int32_t col)
    {
        float mat2x2[2][2];
        for (int32_t r = 0, rn = 0; r < 3; r++)
        {
            if (row == r)
            {
                continue;
            }

            for (int32_t c = 0, cn = 0; c < 3; c++)
            {
                if (col == c)
                {
                    continue;
                }

                mat2x2[rn][cn] = mat.mm[r][c];

                cn++;
            }

            rn++;
        }

        return Matrix22Determinant(
            mat2x2[0][0], mat2x2[0][1],
            mat2x2[1][0], mat2x2[1][1]
        );
    }

    inline Matrix33 Matrix33Adjoint(const Matrix33& mat)
    {
        Matrix33 matAdjoint;
        for (int32_t r = 0; r < 3; r++)
        {
            for (int32_t c = 0; c < 3; c++)
            {
                matAdjoint.mm[c][r] = (float)pow(-1, r + c) * Matrix33MinorDeterminant(mat, r, c);
            }
        }

        return matAdjoint;
    }

    inline bool Matrix33Inverse(Matrix33& out, const Matrix33& mat)
    {
        const float determinant = Matrix33Determinant(mat);
        if (fabsf(determinant) < FLT_EPSILON)
        {
            return false;
        }

        out = Matrix33Adjoint(mat) / determinant;
        return true;
    }

    inline Matrix33& Matrix33Transpose(Matrix33& out, const Matrix33& mat)
    {
        out._11 = mat._11; out._12 = mat._21; out._13 = mat._31;
        out._21 = mat._12; out._22 = mat._22; out._23 = mat._32;
        out._31 = mat._13; out._32 = mat._23; out._33 = mat._33;

        return out;
    }

    inline Matrix33& Matrix33Identity(Matrix33& out)
    {
        out._11 = 1; out._12 = 0; out._13 = 0;
        out._21 = 0; out._22 = 1; out._23 = 0;
        out._31 = 0; out._32 = 0; out._33 = 1;

        return out;
    }

    inline Matrix33& Matrix33FromQuaternion(Matrix33& out, const Quaternion& q)
    {
        const float SquaredX = q.x * q.x;
        const float SquaredY = q.y * q.y;
        const float SquaredZ = q.z * q.z;

        out._11 = 1.0f - 2.0f * (SquaredY + SquaredZ);
        out._12 = 2.0f * (q.x * q.y + q.z * q.w);
        out._13 = 2.0f * (q.x * q.z - q.y * q.w);

        out._21 = 2.0f * (q.x * q.y - q.z * q.w);
        out._22 = 1.0f - 2.0f * (SquaredX + SquaredZ);
        out._23 = 2.0f * (q.y * q.z + q.x * q.w);

        out._31 = 2.0f * (q.x * q.z + q.y * q.w);
        out._32 = 2.0f * (q.y * q.z - q.x * q.w);
        out._33 = 1.0f - 2.0f * (SquaredX + SquaredY);

        return out;
    }

    //
    // For Matrix44 class.

    // Member functions.

    inline Matrix44::Matrix44()
    {
        SetIdentity();
    }

    inline Matrix44::Matrix44(const Matrix44& mat) :
        _11(mat._11), _12(mat._12), _13(mat._13), _14(mat._14),
        _21(mat._21), _22(mat._22), _23(mat._23), _24(mat._24),
        _31(mat._31), _32(mat._32), _33(mat._33), _34(mat._34),
        _41(mat._41), _42(mat._42), _43(mat._43), _44(mat._44)
    {
    }

    inline Matrix44::Matrix44(
        const float a11, const float a12, const float a13, const float a14,
        const float a21, const float a22, const float a23, const float a24,
        const float a31, const float a32, const float a33, const float a34,
        const float a41, const float a42, const float a43, const float a44
    ) :
        _11(a11), _12(a12), _13(a13), _14(a14),
        _21(a21), _22(a22), _23(a23), _24(a24),
        _31(a31), _32(a32), _33(a33), _34(a34),
        _41(a41), _42(a42), _43(a43), _44(a44)
    {
    }

    inline Matrix44::operator float*()
    {
        return &_11;
    }

    inline Matrix44::operator const float*() const
    {
        return &_11;
    }

    inline float Matrix44::operator()(int32_t row, int32_t col) const
    {
        return mm[row][col];
    }

    inline Matrix44 Matrix44::operator +() const
    {
        return *this;
    }

    //< Returns the inverse of the matrix.
    inline Matrix44 Matrix44::operator -() const
    {
        return GetInverse();
    }

    inline Matrix44& Matrix44::operator =(const Matrix44& mat)
    {
        _11 = mat._11; _12 = mat._12; _13 = mat._13; _14 = mat._14;
        _21 = mat._21; _22 = mat._22; _23 = mat._23; _24 = mat._24;
        _31 = mat._31; _32 = mat._32; _33 = mat._33; _34 = mat._34;
        _41 = mat._41; _42 = mat._42; _43 = mat._43; _44 = mat._44;

        return *this;
    }

    inline Matrix44 Matrix44::operator +(const Matrix44 &mat) const
    {
        Matrix44 value(
            _11 + mat._11, _12 + mat._12, _13 + mat._13, _14 + mat._14,
            _21 + mat._21, _22 + mat._22, _23 + mat._23, _24 + mat._24,
            _31 + mat._31, _32 + mat._32, _33 + mat._33, _34 + mat._34,
            _41 + mat._41, _42 + mat._42, _43 + mat._43, _44 + mat._44
        );
        return value;
    }

    inline Matrix44 Matrix44::operator -(const Matrix44 &mat) const
    {
        Matrix44 value(
            _11 - mat._11, _12 - mat._12, _13 - mat._13, _14 - mat._14,
            _21 - mat._21, _22 - mat._22, _23 - mat._23, _24 - mat._24,
            _31 - mat._31, _32 - mat._32, _33 - mat._33, _34 - mat._34,
            _41 - mat._41, _42 - mat._42, _43 - mat._43, _44 - mat._44
        );
        return value;
    }

    inline Matrix44 Matrix44::operator *(const Matrix44 &mat) const
    {
        Matrix44 value(
            _11 * mat._11 + _12 * mat._21 + _13 * mat._31 + _14 * mat._41,
            _11 * mat._12 + _12 * mat._22 + _13 * mat._32 + _14 * mat._42,
            _11 * mat._13 + _12 * mat._23 + _13 * mat._33 + _14 * mat._43,
            _11 * mat._14 + _12 * mat._24 + _13 * mat._34 + _14 * mat._44,

            _21 * mat._11 + _22 * mat._21 + _23 * mat._31 + _24 * mat._41,
            _21 * mat._12 + _22 * mat._22 + _23 * mat._32 + _24 * mat._42,
            _21 * mat._13 + _22 * mat._23 + _23 * mat._33 + _24 * mat._43,
            _21 * mat._14 + _22 * mat._24 + _23 * mat._34 + _24 * mat._44,

            _31 * mat._11 + _32 * mat._21 + _33 * mat._31 + _34 * mat._41,
            _31 * mat._12 + _32 * mat._22 + _33 * mat._32 + _34 * mat._42,
            _31 * mat._13 + _32 * mat._23 + _33 * mat._33 + _34 * mat._43,
            _31 * mat._14 + _32 * mat._24 + _33 * mat._34 + _34 * mat._44,

            _41 * mat._11 + _42 * mat._21 + _43 * mat._31 + _44 * mat._41,
            _41 * mat._12 + _42 * mat._22 + _43 * mat._32 + _44 * mat._42,
            _41 * mat._13 + _42 * mat._23 + _43 * mat._33 + _44 * mat._43,
            _41 * mat._14 + _42 * mat._24 + _43 * mat._34 + _44 * mat._44
        );
        return value;
    }

    inline Matrix44 Matrix44::operator *(const float n) const
    {
        Matrix44 value(
            _11 * n, _12 * n, _13 * n, _14 * n,
            _21 * n, _22 * n, _23 * n, _24 * n,
            _31 * n, _32 * n, _33 * n, _34 * n,
            _41 * n, _42 * n, _43 * n, _44 * n
        );
        return value;
    }

    inline Matrix44 Matrix44::operator /(const float n) const
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

    inline Matrix44& Matrix44::operator +=(const Matrix44& mat)
    {
        _11 += mat._11; _12 += mat._12; _13 += mat._13; _14 += mat._14;
        _21 += mat._21; _22 += mat._22; _23 += mat._23; _24 += mat._24;
        _31 += mat._31; _32 += mat._32; _33 += mat._33; _34 += mat._34;
        _41 += mat._41; _42 += mat._42; _43 += mat._43; _44 += mat._44;

        return *this;
    }

    inline Matrix44& Matrix44::operator -=(const Matrix44& mat)
    {
        _11 -= mat._11; _12 -= mat._12; _13 -= mat._13; _14 -= mat._14;
        _21 -= mat._21; _22 -= mat._22; _23 -= mat._23; _24 -= mat._24;
        _31 -= mat._31; _32 -= mat._32; _33 -= mat._33; _34 -= mat._34;
        _41 -= mat._41; _42 -= mat._42; _43 -= mat._43; _44 -= mat._44;

        return *this;
    }

    inline Matrix44& Matrix44::operator *=(const Matrix44& mat)
    {
        const float f11 = _11 * mat._11 + _12 * mat._21 + _13 * mat._31 + _14 * mat._41;
        const float f12 = _11 * mat._12 + _12 * mat._22 + _13 * mat._32 + _14 * mat._42;
        const float f13 = _11 * mat._13 + _12 * mat._23 + _13 * mat._33 + _14 * mat._43;
        const float f14 = _11 * mat._14 + _12 * mat._24 + _13 * mat._34 + _14 * mat._44;

        const float f21 = _21 * mat._11 + _22 * mat._21 + _23 * mat._31 + _24 * mat._41;
        const float f22 = _21 * mat._12 + _22 * mat._22 + _23 * mat._32 + _24 * mat._42;
        const float f23 = _21 * mat._13 + _22 * mat._23 + _23 * mat._33 + _24 * mat._43;
        const float f24 = _21 * mat._14 + _22 * mat._24 + _23 * mat._34 + _24 * mat._44;

        const float f31 = _31 * mat._11 + _32 * mat._21 + _33 * mat._31 + _34 * mat._41;
        const float f32 = _31 * mat._12 + _32 * mat._22 + _33 * mat._32 + _34 * mat._42;
        const float f33 = _31 * mat._13 + _32 * mat._23 + _33 * mat._33 + _34 * mat._43;
        const float f34 = _31 * mat._14 + _32 * mat._24 + _33 * mat._34 + _34 * mat._44;

        const float f41 = _41 * mat._11 + _42 * mat._21 + _43 * mat._31 + _44 * mat._41;
        const float f42 = _41 * mat._12 + _42 * mat._22 + _43 * mat._32 + _44 * mat._42;
        const float f43 = _41 * mat._13 + _42 * mat._23 + _43 * mat._33 + _44 * mat._43;
        const float f44 = _41 * mat._14 + _42 * mat._24 + _43 * mat._34 + _44 * mat._44;

        _11 = f11; _12 = f12; _13 = f13; _14 = f14;
        _21 = f21; _22 = f22; _23 = f23; _24 = f24;
        _31 = f31; _32 = f32; _33 = f33; _34 = f34;
        _41 = f41; _42 = f42; _43 = f43; _44 = f44;

        return *this;
    }

    inline Matrix44& Matrix44::operator *=(const float n)
    {
        _11 *= n; _12 *= n; _13 *= n; _14 *= n;
        _21 *= n; _22 *= n; _23 *= n; _24 *= n;
        _31 *= n; _32 *= n; _33 *= n; _34 *= n;
        _41 *= n; _42 *= n; _43 *= n; _44 *= n;

        return *this;
    }

    inline Matrix44& Matrix44::operator /=(const float n)
    {
        const float oneOverN = 1.0f / n;
        _11 *= oneOverN; _12 *= oneOverN; _13 *= oneOverN; _14 *= oneOverN;
        _21 *= oneOverN; _22 *= oneOverN; _23 *= oneOverN; _24 *= oneOverN;
        _31 *= oneOverN; _32 *= oneOverN; _33 *= oneOverN; _34 *= oneOverN;
        _41 *= oneOverN; _42 *= oneOverN; _43 *= oneOverN; _44 *= oneOverN;

        return *this;
    }

    inline float Matrix44::GetDeterminant() const
    {
        return Matrix44Determinant(*this);
    }

    inline Matrix44 Matrix44::GetInverse() const
    {
        Matrix44 out;
        if (!Matrix44Inverse(out, *this))
        {
            return *this;
        }

        return out;
    }

    inline void Matrix44::SetIdentity()
    {
        Matrix44Identity(*this);
    }

    // Nonmember functions.

    // Get the determinant of matrix 4x4.
    inline float Matrix44Determinant(const Matrix44& mat)
    {
        return Matrix44Determinant(
            mat._11, mat._12, mat._13, mat._14,
            mat._21, mat._22, mat._23, mat._24,
            mat._31, mat._32, mat._33, mat._34,
            mat._41, mat._42, mat._43, mat._44
        );
    }

    // Get the determinant of matrix exclude row and col.
    inline float Matrix44MinorDeterminant(const Matrix44& mat, const int32_t row, const int32_t col)
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

        return Matrix33Determinant(
            mat3x3[0][0], mat3x3[0][1], mat3x3[0][2],
            mat3x3[1][0], mat3x3[1][1], mat3x3[1][2],
            mat3x3[2][0], mat3x3[2][1], mat3x3[2][2]
        );
    }

    // Get Adjoint of matrix.
    inline Matrix44 Matrix44Adjoint(const Matrix44& mat)
    {
        Matrix44 matAdjoint;
        for (int32_t r = 0; r < 4; r++)
        {
            for (int32_t c = 0; c < 4; c++)
            {
                matAdjoint.mm[c][r] = (float)pow(-1, r + c) * Matrix44MinorDeterminant(mat, r, c);
            }
        }

        return matAdjoint;
    }

    // Get inverse of matrix.
    inline bool Matrix44Inverse(Matrix44& out, const Matrix44& mat)
    {
        const float determinant = Matrix44Determinant(mat);
        if (fabsf(determinant) < FLT_EPSILON)
        {
            return false;
        }

        out = Matrix44Adjoint(mat) / determinant;
        return true;
    }

    // Get transpose of matrix.
    inline Matrix44& Matrix44Transpose(Matrix44& out, const Matrix44& mat)
    {
        Matrix44 transpose(
            mat._11, mat._21, mat._31, mat._41,
            mat._12 ,mat._22, mat._32, mat._42,
            mat._13, mat._23, mat._33, mat._43,
            mat._14, mat._24, mat._34, mat._44
        );
        out = transpose;

        return out;
    }

    // Get identity of matrix 4x4.
    inline Matrix44& Matrix44Identity(Matrix44& out)
    {
        out._11 = 1; out._12 = 0; out._13 = 0; out._14 = 0;
        out._21 = 0; out._22 = 1; out._23 = 0; out._24 = 0;
        out._31 = 0; out._32 = 0; out._33 = 1; out._34 = 0;
        out._41 = 0; out._42 = 0; out._43 = 0; out._44 = 1;

        return out;
    }

    inline Matrix44& Matrix44FromQuaternion(Matrix44& out, const Quaternion& q)
    {
        const float SquaredX = q.x * q.x;
        const float SquaredY = q.y * q.y;
        const float SquaredZ = q.z * q.z;
        //const float SquaredW = q.w * q.w;

        out._11 = 1.0f - 2.0f * (SquaredY + SquaredZ);
        out._12 = 2.0f * (q.x * q.y + q.z * q.w);
        out._13 = 2.0f * (q.x * q.z - q.y * q.w);
        out._14 = 0.0f;

        out._21 = 2.0f * (q.x * q.y - q.z * q.w);
        out._22 = 1.0f - 2.0f * (SquaredX + SquaredZ);
        out._23 = 2.0f * (q.y * q.z + q.x * q.w);
        out._24 = 0.0f;

        out._31 = 2.0f * (q.x * q.z + q.y * q.w);
        out._32 = 2.0f * (q.y * q.z - q.x * q.w);
        out._33 = 1.0f - 2.0f * (SquaredX + SquaredY);
        out._34 = 0.0f;

        out._41 = 0.0f;
        out._42 = 0.0f;
        out._43 = 0.0f;
        out._44 = 1.0f; //SquaredX + SquaredY + SquaredZ + SquaredW;

        return out;
    }

    inline Matrix44& Matrix44Translation(Matrix44& out, const float x, const float y, const float z)
    {
        out._11 = 1.0f; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 1.0f; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = 1.0f; out._34 = 0.0f;
        out._41 = x; out._42 = y; out._43 = z; out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44Translation(Matrix44& out, const Vector3& translation)
    {
        return Matrix44Translation(out, translation.x, translation.y, translation.z);
    }

    inline Matrix44& Matrix44Scaling(Matrix44& out, const float x, const float y, const float z)
    {
        out._11 = x; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = y; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = z; out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = 0.0f; out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44Scaling(Matrix44& out, const Vector3& scale)
    {
        return Matrix44Scaling(out, scale.x, scale.y, scale.z);
    }

    inline Matrix44& Matrix44RotationX(Matrix44& out, const float thetaX)
    {
        const float fSin = sinf(thetaX);
        const float fCos = cosf(thetaX);

        out._11 = 1.0f; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = fCos; out._23 = fSin; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = -fSin; out._33 = fCos; out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = 0.0f; out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44RotationY(Matrix44& out, const float thetaY)
    {
        const float fSin = sinf(thetaY);
        const float fCos = cosf(thetaY);

        out._11 = fCos; out._12 = 0.0f; out._13 = -fSin; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 1.0f; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = fSin; out._32 = 0.0f; out._33 = fCos; out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = 0.0f; out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44RotationZ(Matrix44& out, const float thetaZ)
    {
        const float fSin = sinf(thetaZ);
        const float fCos = cosf(thetaZ);

        out._11 = fCos; out._12 = fSin; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = -fSin; out._22 = fCos; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = 1.0f; out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = 0.0f; out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44RotationAxis(Matrix44& out, const Vector3& axis, const float angle)
    {
        const float sine = sinf(angle);
        const float cosine = cosf(angle);
        const float oneInvCosine = 1 - cosine;

        out._11 = axis.x * axis.x * oneInvCosine + cosine; out._12 = axis.x * axis.y * oneInvCosine + axis.z * sine; out._13 = axis.x * axis.z * oneInvCosine - axis.y * sine; out._14 = 0.0f;
        out._21 = axis.x * axis.y * oneInvCosine - axis.z * sine; out._22 = axis.y * axis.y * oneInvCosine + cosine; out._23 = axis.y * axis.z * oneInvCosine + axis.x * sine; out._24 = 0.0f;
        out._31 = axis.x * axis.z * oneInvCosine + axis.y * sine; out._32 = axis.y * axis.z * oneInvCosine - axis.x * sine; out._33 = axis.z * axis.z * oneInvCosine + cosine; out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = 0.0f; out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44RotationYawPitchRoll(Matrix44& out, const float yaw, const float pitch, const float roll)
    {
        Matrix44 matYaw;
        Matrix44 matPitch;
        Matrix44 matRoll;

        Matrix44RotationY(matYaw, yaw);
        Matrix44RotationX(matPitch, pitch);
        Matrix44RotationZ(matRoll, roll);

        out = matRoll * matPitch * matYaw;
        return out;
    }

    inline Matrix44& Matrix44RotationYawPitchRoll(Matrix44& out, const Vector3& rotation)
    {
        return Matrix44RotationYawPitchRoll(out, rotation.y, rotation.x, rotation.z);
    }

    inline Matrix44& Matrix44Transformation(Matrix44& out, const Vector3& scaling, const Quaternion& rotation, const Vector3& translation)
    {
        Matrix44FromQuaternion(out, rotation);

        out._11 *= scaling.x; out._12 *= scaling.x; out._13 *= scaling.x;
        out._21 *= scaling.y; out._22 *= scaling.y; out._23 *= scaling.z;
        out._31 *= scaling.z; out._32 *= scaling.z; out._33 *= scaling.z;
        out._41 = translation.x; out._42 = translation.y; out._43 = translation.z;

        return out;
    }

    inline Matrix44& Matrix44LookAtLH(Matrix44& out, const Vector3& eye, const Vector3& at, const Vector3& up)
    {
        Vector3 zAxis = (at - eye).Normalize();

        Vector3 xAxis;
        Vector3Cross(xAxis, up, zAxis).Normalize();

        Vector3 yAxis;
        Vector3Cross(yAxis, zAxis, xAxis);

        out._11 = xAxis.x; out._12 = yAxis.x; out._13 = zAxis.x; out._14 = 0.0f;
        out._21 = xAxis.y; out._22 = yAxis.y; out._23 = zAxis.y; out._24 = 0.0f;
        out._31 = xAxis.z; out._32 = yAxis.z; out._33 = zAxis.z; out._34 = 0.0f;
        out._41 = -Vector3Dot(xAxis, eye); out._42 = -Vector3Dot(yAxis, eye); out._43 = -Vector3Dot(zAxis, eye); out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44LookAtRH(Matrix44& out, const Vector3& eye, const Vector3& at, const Vector3& up)
    {
        Vector3 zAxis = (eye - at).Normalize();

        Vector3 xAxis;
        Vector3Cross(xAxis, up, zAxis).Normalize();

        Vector3 yAxis;
        Vector3Cross(yAxis, zAxis, xAxis);

        out._11 = xAxis.x; out._12 = yAxis.x; out._13 = zAxis.x; out._14 = 0.0f;
        out._21 = xAxis.y; out._22 = yAxis.y; out._23 = zAxis.y; out._24 = 0.0f;
        out._31 = xAxis.z; out._32 = yAxis.z; out._33 = zAxis.z; out._34 = 0.0f;
        out._41 = Vector3Dot(xAxis, eye); out._42 = Vector3Dot(yAxis, eye); out._43 = Vector3Dot(zAxis, eye); out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44PerspectiveFovLH(Matrix44& out, const float fovy, const float aspect, const float zn, const float zf)
    {
        const float yScale = 1.0f / tanf(fovy * 0.5f);
        const float xScale = yScale / aspect;

        out._11 = xScale; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = yScale; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = zf / (zf - zn); out._34 = 1.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = -zn * zf / (zf - zn); out._44 = 0.0f;

        return out;
    }

    inline Matrix44& Matrix44PerspectiveFovRH(Matrix44& out, const float fovy, const float aspect, const float zn, const float zf)
    {
        const float yScale = 1.0f / tanf(fovy * 0.5f);
        const float xScale = yScale / aspect;

        out._11 = xScale; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = yScale; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = zf / (zn - zf); out._34 = -1.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = zn * zf / (zn - zf); out._44 = 0.0f;

        return out;
    }

    inline Matrix44& Matrix44OrthoLH(Matrix44& out, const float w, const float h, const float zn, const float zf)
    {
        //return Matrix44OrthoOffCenterLH(out, -w / 2, w / 2, -h / 2, h / 2, zn, zf);

        out._11 = 2.0f / w; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f / h; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = 1.0f / (zf - zn); out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = -zn / (zf - zn); out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44OrthoRH(Matrix44& out, const float w, const float h, const float zn, const float zf)
    {
        //return Matrix44OrthoOffCenterRH(out, -w / 2, w / 2, -h / 2, h / 2, zn, zf);

        out._11 = 2.0f / w; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f / h; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = 1.0f / (zn - zf); out._34 = 0.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = zn / (zn - zf); out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44PerspectiveLH(Matrix44& out, const float w, const float h, const float zn, const float zf)
    {
        out._11 = 2.0f * zn / w; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f * zn / h; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = zf / (zf - zn); out._34 = 1.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = zn * zf / (zn - zf); out._44 = 0.0f;

        return out;
    }

    inline Matrix44& Matrix44PerspectiveRH(Matrix44& out, const float w, const float h, const float zn, const float zf)
    {
        out._11 = 2.0f * zn / w; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f * zn / h; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = zf / (zn - zf); out._34 = -1.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = zn * zf / (zn - zf); out._44 = 0.0f;

        return out;
    }

    inline Matrix44& Matrix44PerspectiveOffCenterLH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf)
    {
        out._11 = 2.0f * zn / (r - l); out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f * zn / (t - b); out._23 = 0.0f; out._24 = 0.0f;
        out._31 = (l + r) / (l - r); out._32 = (t + b) / (b - t); out._33 = zf / (zf - zn); out._34 = 1.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = zn * zf / (zn - zf); out._44 = 0.0f;

        return out;
    }

    inline Matrix44& Matrix44PerspectiveOffCenterRH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf)
    {
        out._11 = 2.0f * zn / (r - l); out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f * zn / (t - b); out._23 = 0.0f; out._24 = 0.0f;
        out._31 = (l + r) / (r - l); out._32 = (t + b) / (t - b); out._33 = zf / (zn - zf); out._34 = -1.0f;
        out._41 = 0.0f; out._42 = 0.0f; out._43 = zn * zf / (zn - zf); out._44 = 0.0f;

        return out;
    }

    inline Matrix44& Matrix44OrthoOffCenterLH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf)
    {
        out._11 = 2.0f / (r - l); out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f / (t - b); out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = 1.0f / (zf - zn); out._34 = 0.0f;
        out._41 = (l + r) / (l - r); out._42 = (t + b) / (b - t); out._43 = zn / (zn - zf); out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44OrthoOffCenterRH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf)
    {
        out._11 = 2.0f / (r - l); out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = 2.0f / (t - b); out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = 1.0f / (zn - zf); out._34 = 0.0f;
        out._41 = (l + r) / (l - r); out._42 = (t + b) / (b - t); out._43 = zn / (zn - zf); out._44 = 1.0f;

        return out;
    }

    inline Matrix44& Matrix44Viewport(Matrix44& out, const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height, const float zn, const float zf)
    {
        out._11 = (float)width * 0.5f; out._12 = 0.0f; out._13 = 0.0f; out._14 = 0.0f;
        out._21 = 0.0f; out._22 = (float)height * -0.5f; out._23 = 0.0f; out._24 = 0.0f;
        out._31 = 0.0f; out._32 = 0.0f; out._33 = zf - zn; out._34 = 0.0f;
        out._41 = x + (float)width * 0.5f; out._42 = y + (float)height * 0.5f; out._43 = zn; out._44 = 1.0f;

        return out;
    }
}

#endif // !__MATRIX_INL__
