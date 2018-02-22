// Add by Yaukey at 2018-02-22.
// YW Soft Renderer 3d base shader class.

#include "stdafx.h"
#include "Yw3dBaseShader.h"
#include "Yw3dDevice.h"

namespace yw
{
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

        return m_VectorConstants[index];
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

    void IYw3dBaseShader::SetDevice(Yw3dDevice* device)
    {
        m_Device = device;
    }

	Yw3dResult IYw3dBaseShader::SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient)
    {
        if (nullptr == m_Device)
        {
            return Yw3d_E_Unknown;
        }

        return m_Device->SampleTexture(color, samplerNumber, u, v, w, xGradient, yGradient);
    }
}
