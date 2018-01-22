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
        Matrix44();
        Matrix44(const Matrix44& m);
        Matrix44(
            const float a11, const float a12, const float a13, const float a14,
            const float a21, const float a22, const float a23, const float a24,
            const float a31, const float a32, const float a33, const float a34,
            const float a41, const float a42, const float a43, const float a44
        );

    public:
        operator float*();
        operator const float*() const;
        float operator()(int32_t row, int32_t col) const;
        Matrix44 operator +() const;
        Matrix44 operator -() const; //< Returns the inverse of the matrix.
        Matrix44& operator =(const Matrix44& m);
        Matrix44 operator +(const Matrix44 &m) const;
        Matrix44 operator -(const Matrix44 &m) const;
        Matrix44 operator *(const Matrix44 &m) const;
        Matrix44 operator *(const float n) const;
        Matrix44 operator /(const float n) const;
        Matrix44& operator +=(const Matrix44& m);
        Matrix44& operator -=(const Matrix44& m);
        Matrix44& operator *=(const Matrix44& m);
        Matrix44& operator *=(const float n);
        Matrix44& operator /=(const float n);

        float GetDeterminant() const;
        Matrix44 GetInverse() const;
        void SetIdentity();
    };

    // Get the determinant of matrix 2x2.
    float Matrix22Determinant(
        const float a11, const float a12,
        const float a21, const float a22
    );

    // Get the determinant of matrix 3x3.
    float Matrix33Determinant(
        const float a11, const float a12, const float a13,
        const float a21, const float a22, const float a23,
        const float a31, const float a32, const float a33
    );

    // Get the determinant of matrix 4x4.
     float Matrix44Determinant(
        const float a11, const float a12, const float a13, const float a14,
        const float a21, const float a22, const float a23, const float a24,
        const float a31, const float a32, const float a33, const float a34,
        const float a41, const float a42, const float a43, const float a44
    );

    // Get the determinant of matrix 4x4.
    float Matrix44Determinant(const Matrix44& mat);

    // Get the determinant of matrix exclude row and col.
    float MinorDeterminant(const Matrix44& mat, const int32_t row, const int32_t col);

    // Get Adjoint of matrix.
    Matrix44 Matrix44Adjoint(const Matrix44& mat);

    // Get inverse of matrix.
    bool Matrix44Inverse(Matrix44& out, const Matrix44& mat);

    // Get transpose of matrix.
    Matrix44& Matrix44Transpose(Matrix44& out, const Matrix44& mat);

    // Get identity of matrix 4x4.
    Matrix44& Matrix44Identity(Matrix44& out);
}

#include "Matrix.inl"

#endif // !__MATRIX_H__
