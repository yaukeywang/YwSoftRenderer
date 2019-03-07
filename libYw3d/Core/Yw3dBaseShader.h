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
        void SetDevice(class Yw3dDevice* device);

        // Samples the texture and returns the looked-up color. This simply functions
        // simply forwards the sampling-call to the device.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] samplerNumber number of the sampler.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        // @param[in] xGradient partial derivatives of the texture coordinates with respect to the screen-space x coordinate (optional, base for mip-level calculations).
        // @param[in] yGradient partial derivatives of the texture coordinates with respect to the screen-space y coordinate (optional, base for mip-level calculations).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w = 0.0f, const Vector4* xGradient = nullptr, const Vector4* yGradient = nullptr);

    private:
        // Single float-constants.
        float m_FloatConstants[YW3D_NUM_SHADER_CONSTANTS];

        // vector4-constants.
        Vector4 m_VectorConstants[YW3D_NUM_SHADER_CONSTANTS];

        // Matrix-constants.
        Matrix44 m_MatrixConstants[YW3D_NUM_SHADER_CONSTANTS];

        // The Yw3d-device currently used for rendering.
        class Yw3dDevice* m_Device;
    };
}

#endif // !__YW_3D_BASE_SHADER_H__
