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

    float IYw3dBaseShader::clamp(const float value, const float lower, const float upper)
    {
        return yw::Clamp(value, lower, upper);
    }

    int32_t IYw3dBaseShader::clamp(const int32_t value, const int32_t lower, const int32_t upper)
    {
        return yw::Clamp(value, lower, upper);
    }

    Vector2 IYw3dBaseShader::clamp(const Vector2& value, const float lower, const float upper)
    {
        return Vector2(yw::Clamp(value.x, lower, upper), yw::Clamp(value.y, lower, upper));
    }

    Vector3 IYw3dBaseShader::clamp(const Vector3& value, const float lower, const float upper)
    {
        return Vector3(yw::Clamp(value.x, lower, upper), yw::Clamp(value.y, lower, upper), yw::Clamp(value.z, lower, upper));
    }

    Vector4 IYw3dBaseShader::clamp(const Vector4& value, const float lower, const float upper)
    {
        return Vector4(yw::Clamp(value.x, lower, upper), yw::Clamp(value.y, lower, upper), yw::Clamp(value.z, lower, upper), yw::Clamp(value.w, lower, upper));
    }

    float IYw3dBaseShader::saturate(const float value)
    {
        return yw::Saturate(value);
    }

    Vector2 IYw3dBaseShader::saturate(const Vector2& value)
    {
        return Vector2(yw::Saturate(value.x), yw::Saturate(value.y));
    }

    Vector3 IYw3dBaseShader::saturate(const Vector3& value)
    {
        return Vector3(yw::Saturate(value.x), yw::Saturate(value.y), yw::Saturate(value.z));
    }

    Vector4 IYw3dBaseShader::saturate(const Vector4& value)
    {
        return Vector4(yw::Saturate(value.x), yw::Saturate(value.y), yw::Saturate(value.z), yw::Saturate(value.w));
    }

    float IYw3dBaseShader::lerp(const float a, const float b, const float t)
    {
        return yw::Lerp(a, b, t);
    }

    Vector2 IYw3dBaseShader::lerp(const Vector2& a, const Vector2& b, const float t)
    {
        return Vector2(yw::Lerp(a.x, b.x, t), yw::Lerp(a.y, b.y, t));
    }

    Vector3 IYw3dBaseShader::lerp(const Vector3& a, const Vector3& b, const float t)
    {
        Vector3 result;
        return Vector3Lerp(result, a, b, t);
    }

    Vector4 IYw3dBaseShader::lerp(const Vector4& a, const Vector4& b, const float t)
    {
        return Vector4(yw::Lerp(a.x, b.x, t), yw::Lerp(a.y, b.y, t), yw::Lerp(a.z, b.z, t), yw::Lerp(a.w, b.w, t));
    }

    float IYw3dBaseShader::dot(const Vector3& a, const Vector3& b)
    {
        return Vector3Dot(a, b);
    }

    Vector3 IYw3dBaseShader::cross(const Vector3& a, const Vector3& b)
    {
        Vector3 result;
        return Vector3Cross(result, a, b);
    }

    Vector3 IYw3dBaseShader::normalize(const Vector3& v)
    {
        Vector3 result;
        return Vector3Normalize(result, v);
    }
}
