// Add by Yaukey at 2018-02-22.
// YW Soft Renderer 3d base shader class.

#ifndef __YW_3D_BASE_SHADER_H__
#define __YW_3D_BASE_SHADER_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // This is the shader base-class, which implements support for float, vector4 and matrix constants-registers.
    class IYw3dBaseShader : public IBase
    {
        friend class Yw3dDevice;

    public:
        // Sets a single float-constant.
        // @param[in] index index of the constant.
        // @param[in] value value of the constant.
        void SetFloat(uint32_t index, float value);

        // Returns the value of a single float-constant.
        // @param[in] index index of the constant.
        float GetFloat(uint32_t index) const;

        // Sets a vector4-constant.
        // @param[in] index index of the constant.
        // @param[in] vector value of the constant.
        void SetVector(uint32_t index, const Vector4& vector);

        // Returns the value of a vector4-constant.
        // @param[in] index index of the constant.
        const Vector4& GetVector(uint32_t index) const;

        // Sets a matrix-constant.
        // @param[in] index index of the constant.
        // @param[in] matrix value of the constant.
        void SetMatrix(uint32_t index, const Matrix44& matrix);

        // Returns the value of a matrix-constant.
        // @param[in] index index of the constant.
        const Matrix44& GetMatrix(uint32_t index) const;

        // Returns the world matrix.
        const Matrix44* GetWorldMatrix() const;

        // Returns the view matrix.
        const Matrix44* GetViewMatrix() const;

        // Returns the projection matrix.
        const Matrix44* GetProjectionMatrix() const;

        // Returns the combined matrix of world, view and projection.
        const Matrix44* GetWVPMatrix() const;

    protected:
        // Accessible by Yw3dDevice - Sets the rendering-device.
        // @param[in] device the device.
        inline void SetDevice(class Yw3dDevice* device)
        {
            m_Device = device;
        }

        // Gets the rendering-device.
        // return the device.
        inline class Yw3dDevice* GetDevice() const
        {
            return m_Device;
        }

        // Samples the texture and returns the looked-up color. This simply functions
        // simply forwards the sampling-call to the device.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] samplerNumber number of the sampler.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        // @param[in] lod level of detail.
        // @param[in] xGradient partial derivatives of the texture coordinates with respect to the screen-space x coordinate (optional, base for mip-level calculations).
        // @param[in] yGradient partial derivatives of the texture coordinates with respect to the screen-space y coordinate (optional, base for mip-level calculations).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w = 0.0f, float lod= 0.0f, const Vector4* xGradient = nullptr, const Vector4* yGradient = nullptr);

    protected:
        /**
         * @brief Clamps a floating-point value.
         * @param[in] v value to clamp.
         * @param[in] l minimum value.
         * @param[in] u maximum value.
         * @return clamped value in [l,u].
         */
        static inline float clamp(const float v, const float l, const float u)
        {
            return yw::Clamp(v, l, u);
        }

        /**
         * @brief Clamps an integer value.
         * @param[in] v value to clamp.
         * @param[in] l minimum value.
         * @param[in] u maximum value.
         * @return clamped value in [l,u].
         */
        static inline int32_t clamp(const int32_t v, const int32_t l, const int32_t u)
        {
            return yw::Clamp(v, l, u);
        }

        /**
         * @brief Clamps a Vector2 value.
         * @param[in] v value to clamp.
         * @param[in] l minimum value.
         * @param[in] u maximum value.
         * @return clamped value in [l,u].
         */
        static inline Vector2 clamp(const Vector2& v, const float l, const float u)
        {
            return yw::Clamp(v, l, u);
        }

        /**
         * @brief Clamps a Vector3 value.
         * @param[in] v value to clamp.
         * @param[in] l minimum value.
         * @param[in] u maximum value.
         * @return clamped value in [l,u].
         */
        static inline Vector3 clamp(const Vector3& v, const float l, const float u)
        {
            return yw::Clamp(v, l, u);
        }

        /**
         * @brief Clamps a Vector4 value.
         * @param[in] v value to clamp.
         * @param[in] l minimum value.
         * @param[in] u maximum value.
         * @return clamped value in [l,u].
         */
        static inline Vector4 clamp(const Vector4& v, const float l, const float u)
        {
            return yw::Clamp(v, l, u);
        }

        /**
         * @brief Clamps a floating-point value to [0.0f,1.0f].
         * @param[in] v value to saturate.
         * @return saturated value in [0.0f,1.0f].
         */
        static inline float saturate(const float v)
        {
            return yw::Saturate(v);
        }

        /**
         * @brief Clamps a Vector2 value to [0.0f,1.0f].
         * @param[in] v value to saturate.
         * @return saturated value in [0.0f,1.0f].
         */
        static inline Vector2 saturate(const Vector2& v)
        {
            return yw::Saturate(v);
        }

        /**
         * @brief Clamps a Vector3 value to [0.0f,1.0f].
         * @param[in] v value to saturate.
         * @return saturated value in [0.0f,1.0f].
         */
        static inline Vector3 saturate(const Vector3& v)
        {
            return yw::Saturate(v);
        }

        /**
         * @brief Clamps a Vector4 value to [0.0f,1.0f].
         * @param[in] v value to saturate.
         * @return saturated value in [0.0f,1.0f].
         */
        static inline Vector4 saturate(const Vector4& v)
        {
            return yw::Saturate(v);
        }

        /**
         * @brief Linearly interpolates between two values.
         * @param[in] a first value.
         * @param[in] b second value.
         * @param [in] t interpolation factor in [0.0f,1.0f].
         * @return Linearly interpolates between two values.
         */
        static inline float lerp(const float a, const float b, const float t)
        {
            return yw::Lerp(a, b, t);
        }

        /**
         * @brief Linearly interpolates between two Vector2 values.
         * @param[in] a first value.
         * @param[in] b second value.
         * @param [in] t interpolation factor in [0.0f,1.0f].
         * @return Linearly interpolates between two values.
         */
        static inline Vector2 lerp(const Vector2& a, const Vector2& b, const float t)
        {
            return yw::Lerp(a, b, t);
        }

        /**
         * @brief Linearly interpolates between two Vector3 values.
         * @param[in] a first value.
         * @param[in] b second value.
         * @param [in] t interpolation factor in [0.0f,1.0f].
         * @return Linearly interpolates between two values.
         */
        static inline Vector3 lerp(const Vector3& a, const Vector3& b, const float t)
        {
            return yw::Lerp(a, b, t);
        }

        /**
         * @brief Linearly interpolates between two Vector4 values.
         * @param[in] a first value.
         * @param[in] b second value.
         * @param [in] t interpolation factor in [0.0f,1.0f].
         * @return Linearly interpolates between two values.
         */
        static inline Vector4 lerp(const Vector4& a, const Vector4& b, const float t)
        {
            return yw::Lerp(a, b, t);
        }

        /**
         * @brief The minimum result of two vector2. These all operate component-wise. The calculation is per component.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the minimum of two scalars or each respective component of two vectors.
         */
        static inline Vector2 minimum(const Vector2& a, const Vector2& b)
        {
            return Minimum(a, b);
        }

        /**
         * @brief The minimum result of two vector3. These all operate component-wise. The calculation is per component.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the minimum of two scalars or each respective component of two vectors.
         */
        static inline Vector3 minimum(const Vector3& a, const Vector3& b)
        {
            return Minimum(a, b);
        }

        /**
         * @brief The minimum result of two vector4. These all operate component-wise. The calculation is per component.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the minimum of two scalars or each respective component of two vectors.
         */
        static inline Vector4 minimum(const Vector4& a, const Vector4& b)
        {
            return Minimum(a, b);
        }

        /**
         * @brief The maximum result of two vector2. These all operate component-wise. The calculation is per component.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the maximum of two scalars or each respective component of two vectors.
         */
        static inline Vector2 maximum(const Vector2& a, const Vector2& b)
        {
            return Maximum(a, b);
        }

        /**
         * @brief The maximum result of two vector3. These all operate component-wise. The calculation is per component.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the maximum of two scalars or each respective component of two vectors.
         */
        static inline Vector3 maximum(const Vector3& a, const Vector3& b)
        {
            return Maximum(a, b);
        }

        /**
         * @brief The maximum result of two vector4. These all operate component-wise. The calculation is per component.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the maximum of two scalars or each respective component of two vectors.
         */
        static inline Vector4 maximum(const Vector4& a, const Vector4& b)
        {
            return Maximum(a, b);
        }

        /**
         * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
         * @param[in] v value that to be checked.
         * @return true if any component of x is not equal to 0. false otherwise.
         */
        static inline bool any(const float v)
        {
            return Any(v);
        }

        /**
         * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
         * @param[in] v value that to be checked.
         * @return true if any component of x is not equal to 0. false otherwise.
         */
        static inline bool any(const Vector2& v)
        {
            return Any(v);
        }

        /**
         * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
         * @param[in] v value that to be checked.
         * @return true if any component of x is not equal to 0. false otherwise.
         */
        static inline bool any(const Vector3& v)
        {
            return Any(v);
        }

        /**
         * @brief Returns true if any component of v is not equal to 0. Returns false otherwise.
         * @param[in] v value that to be checked.
         * @return true if any component of x is not equal to 0. false otherwise.
         */
        static inline bool any(const Vector4& v)
        {
            return Any(v);
        }

        /**
         * @brief The length of vector2.
         * @param[in] v vector to calculate length.
         * @return length of this vector.
         */
        static inline float length(const Vector2& v)
        {
            return v.Length();
        }

        /**
         * @brief The length of vector3.
         * @param[in] v vector to calculate length.
         * @return length of this vector.
         */
        static inline float length(const Vector3& v)
        {
            return v.Length();
        }

        /**
         * @brief The length of vector4.
         * @param[in] v vector to calculate length.
         * @return length of this vector.
         */
        static inline float length(const Vector4& v)
        {
            return v.Length();
        }

        /**
         * @brief The result of two vector2 dot product.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the dot product value.
         */
        static inline float dot(const Vector2& a, const Vector2& b)
        {
            return Vector2Dot(a, b);
        }

        /**
         * @brief The result of two vector3 dot product.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the dot product value.
         */
        static inline float dot(const Vector3& a, const Vector3& b)
        {
            return Vector3Dot(a, b);
        }

        /**
         * @brief The result of two vector4 dot product.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the dot product value.
         */
        static inline float dot(const Vector4& a, const Vector4& b)
        {
            return Vector4Dot(a, b);
        }

        /**
         * @brief The result of two vector3 cross product.
         * @param[in] a first value.
         * @param[in] b second value.
         * @return the cross product value.
         */
        static inline Vector3 cross(const Vector3& a, const Vector3& b)
        {
            Vector3 result;
            return Vector3Cross(result, a, b);
        }

        /**
         * @brief The normalized result of Vector2 value.
         * @param[in] v value to be normalized.
         * @return the normalized Vector2 value.
         */
        static inline Vector2 normalize(const Vector2& v)
        {
            Vector2 result;
            return Vector2Normalize(result, v);
        }

        /**
         * @brief The normalized result of Vector3 value.
         * @param[in] v value to be normalized.
         * @return the normalized Vector3 value.
         */
        static inline Vector3 normalize(const Vector3& v)
        {
            Vector3 result;
            return Vector3Normalize(result, v);
        }

        /**
         * @brief The normalized result of Vector4 value.
         * @param[in] v value to be normalized.
         * @return the normalized Vector4 value.
         */
        static inline Vector4 normalize(const Vector4& v)
        {
            Vector4 result;
            return Vector4Normalize(result, v);
        }

        /**
         * Returns the specified value raised to the specified power.
         * @param[in] v The specified value.
         * @param[in] p The specified power.
         * @return The v parameter raised to the power of the p parameter.
         */
        static inline float pow(const float v, const float p)
        {
            return Pow(v, p);
        }

        /**
         * Returns the specified value raised to the specified power.
         * @param[in] v The specified value.
         * @param[in] p The specified power.
         * @return The v parameter raised to the power of the p parameter.
         */
        static inline Vector2 pow(const Vector2& v, const Vector2& p)
        {
            return Pow(v, p);
        }

        /**
         * Returns the specified value raised to the specified power.
         * @param[in] v The specified value.
         * @param[in] p The specified power.
         * @return The v parameter raised to the power of the p parameter.
         */
        static inline Vector3 pow(const Vector3& v, const Vector3& p)
        {
            return Pow(v, p);
        }

        /**
         * Returns the specified value raised to the specified power.
         * @param[in] v The specified value.
         * @param[in] p The specified power.
         * @return The v parameter raised to the power of the p parameter.
         */
        static inline Vector4 pow(const Vector4& v, const Vector4& p)
        {
            return Pow(v, p);
        }

        /**
         * This function calculates the reflection vector using the following formula: v = i - 2 * n * dot(i n) .
         * @param[in] ii incident vector.
         * @param[in] n normal vector.
         * @return Returns a reflection vector using an incident ray and a surface normal..
         */
        static inline Vector2 reflect(const Vector2& ii, const Vector2& n)
        {
            return ii - 2.0f * n * dot(ii, n);
        }

        /**
         * This function calculates the reflection vector using the following formula: v = i - 2 * n * dot(i n) .
         * @param[in] i incident vector.
         * @param[in] n normal vector.
         * @return Returns a reflection vector using an incident ray and a surface normal..
         */
        static inline Vector3 reflect(const Vector3& i, const Vector3& n)
        {
            return i - 2.0f * n * dot(i, n);
        }

        /**
         * This function calculates the reflection vector using the following formula: v = i - 2 * n * dot(i n) .
         * @param[in] i incident vector.
         * @param[in] n normal vector.
         * @return Returns a reflection vector using an incident ray and a surface normal..
         */
        static inline Vector4 reflect(const Vector4& i, const Vector4& n)
        {
            return i - 2.0f * n * dot(i, n);
        }

    private:
        // Single float-constants.
        float m_FloatConstants[YW3D_NUM_SHADER_CONSTANTS];

        // vector4-constants.
        Vector4 m_VectorConstants[YW3D_NUM_SHADER_CONSTANTS];

        // Matrix-constants.
        Matrix44 m_MatrixConstants[YW3D_NUM_SHADER_CONSTANTS];

        // The Yw3d-device currently used for rendering.
        class Yw3dDevice* m_Device;

    protected:
        // PI.
        static const float PI;

        // Inverse of PI.
        static const float INV_PI;
    };
}

#endif // !__YW_3D_BASE_SHADER_H__
