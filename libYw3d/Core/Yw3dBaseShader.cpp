// Add by Yaukey at 2018-02-22.
// YW Soft Renderer 3d base shader class.

#include "Yw3dBaseShader.h"
#include "Yw3dDevice.h"

namespace yw
{
    const float IYw3dBaseShader::PI = (float)YW_PI;
    const float IYw3dBaseShader::INV_PI = (float)YW_INV_PI;

    void IYw3dBaseShader::SetFloat(uint32_t index, float value)
    {
        if (index >= YW3D_NUM_SHADER_CONSTANTS)
        {
            return;
        }

        m_FloatConstants[index] = value;
    }

    float IYw3dBaseShader::GetFloat(uint32_t index) const
    {
        if (index >= YW3D_NUM_SHADER_CONSTANTS)
        {
            return 0.0f;
        }

        return m_FloatConstants[index];
    }

    void IYw3dBaseShader::SetVector(uint32_t index, const Vector4& vector)
    {
        if (index >= YW3D_NUM_SHADER_CONSTANTS)
        {
            return;
        }

        m_VectorConstants[index] = vector;
    }

    const Vector4& IYw3dBaseShader::GetVector(uint32_t index) const
    {
        if (index >= YW3D_NUM_SHADER_CONSTANTS)
        {
            return m_VectorConstants[YW3D_NUM_SHADER_CONSTANTS - 1];
        }

        return m_VectorConstants[index];
    }

    void IYw3dBaseShader::SetMatrix(uint32_t index, const Matrix44& matrix)
    {
        if (index >= YW3D_NUM_SHADER_CONSTANTS)
        {
            return;
        }

        m_MatrixConstants[index] = matrix;
    }

    const Matrix44& IYw3dBaseShader::GetMatrix(uint32_t index) const
    {
        if (index >= YW3D_NUM_SHADER_CONSTANTS)
        {
            return m_MatrixConstants[YW3D_NUM_SHADER_CONSTANTS - 1];
        }

        return m_MatrixConstants[index];
    }

    const Matrix44* IYw3dBaseShader::GetWorldMatrix() const
    {
        const Matrix44* matWorld = nullptr;
        if (YW3D_FAILED(m_Device->GetTransform(Yw3d_TS_World, matWorld)))
        {
            return nullptr;
        }

        return matWorld;
    }

    const Matrix44* IYw3dBaseShader::GetViewMatrix() const
    {
        const Matrix44* matView = nullptr;
        if (YW3D_FAILED(m_Device->GetTransform(Yw3d_TS_View, matView)))
        {
            return nullptr;
        }

        return matView;
    }

    const Matrix44* IYw3dBaseShader::GetProjectionMatrix() const
    {
        const Matrix44* matProj = nullptr;
        if (YW3D_FAILED(m_Device->GetTransform(Yw3d_TS_Projection, matProj)))
        {
            return nullptr;
        }

        return matProj;
    }

    const Matrix44* IYw3dBaseShader::GetWVPMatrix() const
    {
        const Matrix44* matWVP = nullptr;
        if (YW3D_FAILED(m_Device->GetTransform(Yw3d_TS_WVP, matWVP)))
        {
            return nullptr;
        }

        return matWVP;
    }

    Yw3dResult IYw3dBaseShader::SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w, float lod, const Vector4* xGradient, const Vector4* yGradient)
    {
        if (nullptr == m_Device)
        {
            return Yw3d_E_Unknown;
        }

        return m_Device->SampleTexture(color, samplerNumber, u, v, w, lod, xGradient, yGradient);
    }
}
