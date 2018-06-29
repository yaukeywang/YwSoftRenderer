// Add by yaukey at 2017-11-14.
// Matrix class.

#ifndef __YW_MATH_MATRIX_H__
#define __YW_MATH_MATRIX_H__

#include "YwMathUtility.h"

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

    // Builds a matrix that rotates around an arbitrary axis.
    Matrix44& Matrix44RotationAxis(Matrix44& out, const Vector3& axis, const float angle);

    // Construct a matrix rotate with x, y and z axis with euler theta.
    Matrix44& Matrix44RotationYawPitchRoll(Matrix44& out, const float yaw, const float pitch, const float roll);

    // Construct a matrix rotate with x, y and z axis with euler theta.
    Matrix44& Matrix44RotationYawPitchRoll(Matrix44& out, const Vector3& rotation);

    // Construct a world transform matrix formed by position, rotation and scale.
    Matrix44& Matrix44Transformation(Matrix44& out, const Vector3& scaling, const Quaternion& rotation, const Vector3& translation);

    // Construct a look at matrix at left hand axis.
    Matrix44& Matrix44LookAtLH(Matrix44& out, const Vector3& eye, const Vector3& at, const Vector3& up);

    // Construct a look at matrix at right hand axis.
    Matrix44& Matrix44LookAtRH(Matrix44& out, const Vector3& eye, const Vector3& at, const Vector3& up);

    // Construct perspective matrix at left hand axis.
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ee418867(v=vs.85).aspx
    Matrix44& Matrix44PerspectiveFovLH(Matrix44& out, const float fovy, const float aspect, const float zn, const float zf);

    // Construct perspective matrix at right hand axis.
    Matrix44& Matrix44PerspectiveFovRH(Matrix44& out, const float fovy, const float aspect, const float zn, const float zf);

    // Builds a left-handed orthographic projection matrix.
    Matrix44& Matrix44OrthoLH(Matrix44& out, const float w, const float h, const float zn, const float zf);

    // Builds a right - handed orthographic projection matrix.
    Matrix44& Matrix44OrthoRH(Matrix44& out, const float w, const float h, const float zn, const float zf);

    // Builds a left-handed perspective projection matrix
    // All the parameters of the D3DXMatrixPerspectiveLH function are distances in camera space. The parameters describe the dimensions of the view volume.
    Matrix44& Matrix44PerspectiveLH(Matrix44& out, const float w, const float h, const float zn, const float zf);

    // Builds a right-handed perspective projection matrix.
    // All the parameters of the Matrix44PerspectiveRH function are distances in camera space.The parameters describe the dimensions of the view volume.
    Matrix44& Matrix44PerspectiveRH(Matrix44& out, const float w, const float h, const float zn, const float zf);

    // Builds a customized, left-handed perspective projection matrix.
    // All the parameters of the Matrix44PerspectiveOffCenterLH function are distances in camera space. The parameters describe the dimensions of the view volume.
    Matrix44& Matrix44PerspectiveOffCenterLH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf);

    // Builds a customized, right-handed perspective projection matrix.
    // All the parameters of the Matrix44PerspectiveOffCenterRH function are distances in camera space. The parameters describe the dimensions of the view volume.
    Matrix44& Matrix44PerspectiveOffCenterRH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf);

    // Builds a customized, left-handed orthographic projection matrix.
    // The Matrix44OrthoLH function is a special case of the Matrix44OrthoOffCenterLH function. 
    // To create the same projection using Matrix44OrthoOffCenterLH, use the following values: l = -w/2, r = w/2, b = -h/2, and t = h/2.
    // Matrix44OrthoOffCenterLH(out, -w/2, w/2, -h/2, h/2, zn, zf)
    // All the parameters of the Matrix44OrthoOffCenterLH function are distances in camera space. The parameters describe the dimensions of the view volume.
    Matrix44& Matrix44OrthoOffCenterLH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf);

    // Builds a customized, right-handed orthographic projection matrix.
    // The Matrix44OrthoRH function is a special case of the Matrix44OrthoOffCenterRH function. 
    // To create the same projection using Matrix44OrthoOffCenterRH, use the following values: l = -w/2, r = w/2, b = -h/2, and t = h/2.
    // Matrix44OrthoOffCenterRH(out, -w/2, w/2, -h/2, h/2, zn, zf)
    // All the parameters of the Matrix44OrthoOffCenterRH function are distances in camera space. The parameters describe the dimensions of the view volume.
    Matrix44& Matrix44OrthoOffCenterRH(Matrix44& out, const float l, const float r, const float b, const float t, const float zn, const float zf);

    // Builds a viewport transform matrix.
    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb206341(v=vs.85).aspx
    Matrix44& Matrix44Viewport(Matrix44& out, const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height, const float zn, const float zf);
}

#include "YwMathMatrix.inl"

#endif // !__YW_MATH_MATRIX_H__
