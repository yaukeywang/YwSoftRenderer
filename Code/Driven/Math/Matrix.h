// Add by yaukey at 2017-11-14.
// Matrix class.

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "System/Support/Base.h"
#include "MathUtility.h"

namespace yw
{
    // Predefine.
    struct Vector2;
    struct Vector3;
    struct Vector4;
    struct Quaternion;

    // Matrix common functions.

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

    // Matrix 3x3 class.
    struct Matrix33
    {
    public:
        union
        {
            float mm[3][3];
            float m[9];
            struct
            {
                float _11, _12, _13;
                float _21, _22, _23;
                float _31, _32, _33;
            };
        };

    public:
        Matrix33();
        Matrix33(const Matrix33& mat);
        Matrix33(
            const float a11, const float a12, const float a13,
            const float a21, const float a22, const float a23,
            const float a31, const float a32, const float a33
        );

    public:
        operator float*();
        operator const float*() const;
        float operator()(int32_t row, int32_t col) const;
        Matrix33 operator +() const;
        Matrix33 operator -() const; //< Returns the inverse of the matrix.
        Matrix33& operator =(const Matrix33& mat);
        Matrix33 operator +(const Matrix33 &mat) const;
        Matrix33 operator -(const Matrix33 &mat) const;
        Matrix33 operator *(const Matrix33 &mat) const;
        Matrix33 operator *(const float n) const;
        Matrix33 operator /(const float n) const;
        Matrix33& operator +=(const Matrix33& mat);
        Matrix33& operator -=(const Matrix33& mat);
        Matrix33& operator *=(const Matrix33& mat);
        Matrix33& operator *=(const float n);
        Matrix33& operator /=(const float n);

        float GetDeterminant() const;
        Matrix33 GetInverse() const;
        void SetIdentity();
    };

    // Matrix33 nonmember functions.

    // Get the determinant of matrix 4x4.
    float Matrix33Determinant(const Matrix33& mat);

    // Get the determinant of matrix exclude row and col.
    float Matrix33MinorDeterminant(const Matrix33& mat, const int32_t row, const int32_t col);

    // Get Adjoint of matrix.
    Matrix33 Matrix33Adjoint(const Matrix33& mat);

    // Get inverse of matrix.
    bool Matrix33Inverse(Matrix33& out, const Matrix33& mat);

    // Get transpose of matrix.
    Matrix33& Matrix33Transpose(Matrix33& out, const Matrix33& mat);

    // Get identity of matrix 4x4.
    Matrix33& Matrix33Identity(Matrix33& out);

    // Get matrix from quaternion.
    Matrix33& Matrix33FromQuaternion(Matrix33& out, const Quaternion& q);

    // Matrix 4x4 class.
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
        Matrix44(const Matrix44& mat);
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
        Matrix44& operator =(const Matrix44& mat);
        Matrix44 operator +(const Matrix44 &mat) const;
        Matrix44 operator -(const Matrix44 &mat) const;
        Matrix44 operator *(const Matrix44 &mat) const;
        Matrix44 operator *(const float n) const;
        Matrix44 operator /(const float n) const;
        Matrix44& operator +=(const Matrix44& mat);
        Matrix44& operator -=(const Matrix44& mat);
        Matrix44& operator *=(const Matrix44& mat);
        Matrix44& operator *=(const float n);
        Matrix44& operator /=(const float n);

        float GetDeterminant() const;
        Matrix44 GetInverse() const;
        void SetIdentity();
    };

    // Matrix44 nonmember functions.

    // Get the determinant of matrix 4x4.
    float Matrix44Determinant(const Matrix44& mat);

    // Get the determinant of matrix exclude row and col.
    float Matrix44MinorDeterminant(const Matrix44& mat, const int32_t row, const int32_t col);

    // Get Adjoint of matrix.
    Matrix44 Matrix44Adjoint(const Matrix44& mat);

    // Get inverse of matrix.
    bool Matrix44Inverse(Matrix44& out, const Matrix44& mat);

    // Get transpose of matrix.
    Matrix44& Matrix44Transpose(Matrix44& out, const Matrix44& mat);

    // Get identity of matrix 4x4.
    Matrix44& Matrix44Identity(Matrix44& out);

    // Get matrix from quaternion.
    Matrix44& Matrix44FromQuaternion(Matrix44& out, const Quaternion& q);

    // Construct a matrix with translation.
    Matrix44& Matrix44Translation(Matrix44& out, const float x, const float y, const float z);

    // Construct a matrix with translation.
    Matrix44& Matrix44Translation(Matrix44& out, const Vector3& translation);

    // Construct a matrix scale with x, y and z.
    Matrix44& Matrix44Scaling(Matrix44& out, const float x, const float y, const float z);

    // Construct a matrix scale with x, y and z.
    Matrix44& Matrix44Scaling(Matrix44& out, const Vector3& scale);

    // Construct a matrix rotate with x axis with euler theta.
    Matrix44& Matrix44RotationX(Matrix44& out, const float thetaX);

    // Construct a matrix rotate with y axis with euler theta.
    Matrix44& Matrix44RotationY(Matrix44& out, const float thetaY);

    // Construct a matrix rotate with z axis with euler theta.
    Matrix44& Matrix44RotationZ(Matrix44& out, const float thetaZ);

    // Construct a matrix rotate with x, y and z axis with euler theta.
    Matrix44& Matrix44RotationYawPitchRoll(Matrix44& out, const float yaw, const float pitch, const float roll);

    // Construct a matrix rotate with x, y and z axis with euler theta.
    Matrix44& Matrix44RotationYawPitchRoll(Matrix44& out, const Vector3& rotation);

    // Construct a world transform matrix formed by position, rotation and scale.
    Matrix44& Matrix44Transform(Matrix44& out, const Vector3& scale, const Quaternion& rotation, const Vector3& position);
}

#include "Matrix.inl"

#endif // !__MATRIX_H__
