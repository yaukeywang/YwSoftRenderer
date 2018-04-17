// Add by Yaukey at 2018-02-22.
// YW Soft Renderer 3d shader class( vertex-shader, triangle-shader and pixel shader).

#include "stdafx.h"
#include "Yw3dShader.h"

namespace yw
{
    IYw3dPixelShader::IYw3dPixelShader() : 
        m_VsOutputs(nullptr),
        m_TriangleInfo(nullptr)
    {

    }

    void IYw3dPixelShader::SetInfo(const Yw3dShaderRegisterType* vsOutputs, const Yw3dTriangleInfo* triangleInfo)
    {
        m_VsOutputs = vsOutputs;
        m_TriangleInfo = triangleInfo;
    }

    // Partial derivative equations taken from
    // "MIP-Map Level Selection for Texture Mapping",
    // Jon P. Ewins, Member, IEEE, Marcus D. Waller,
    // Martin White, and Paul F. Lister, Member, IEEE
    void IYw3dPixelShader::GetPartialDerivatives(uint32_t shaderRegister, Vector4& ddx, Vector4& ddy) const
    {
        // Set default value.
        ddx = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
        ddy = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

        // Skip if invalid shader register.
        if ((shaderRegister < 0) || (shaderRegister >= YW3D_PIXEL_SHADER_REGISTERS))
        {
            return;
        }

        // Get all formula parameters.
        const Yw3dShaderRegister& A = m_TriangleInfo->shaderOutputsDdx[shaderRegister];
        const Yw3dShaderRegister& B = m_TriangleInfo->shaderOutputsDdy[shaderRegister];
        const Yw3dShaderRegister& C = m_TriangleInfo->baseVertex->shaderOutputs[shaderRegister];

        const float& D = m_TriangleInfo->wDdx;
        const float& E = m_TriangleInfo->wDdy;
        const float& F = m_TriangleInfo->baseVertex->position.w;

        const float deltaPixelX = (float)m_TriangleInfo->curPixelX - m_TriangleInfo->baseVertex->position.x;
        const float deltaPixelY = (float)m_TriangleInfo->curPixelY - m_TriangleInfo->baseVertex->position.y;
        const float invWSquare = m_TriangleInfo->curPixelInvW * m_TriangleInfo->curPixelInvW;

        // Compute partial derivative with respect to the x-screen space coordinate.
        switch (m_VsOutputs[shaderRegister])
        {
        case Yw3d_SRT_Vector4:
            ddx.w = ((A.w * F - C.w * D) + (A.w * E - B.w * D) * deltaPixelY) * invWSquare;
        case Yw3d_SRT_Vector3:
            ddx.z = ((A.z * F - C.z * D) + (A.z * E - B.z * D) * deltaPixelY) * invWSquare;
        case Yw3d_SRT_Vector2:
            ddx.y = ((A.y * F - C.y * D) + (A.y * E - B.y * D) * deltaPixelY) * invWSquare;
        case Yw3d_SRT_Float32:
            ddx.x = ((A.x * F - C.x * D) + (A.x * E - B.x * D) * deltaPixelY) * invWSquare;
        case Yw3d_SRT_Unused:
        default:
            break;
        }

        // Compute partial derivative with respect to the y-screen space coordinate.
        switch (m_VsOutputs[shaderRegister])
        {
        case Yw3d_SRT_Vector4:
            ddy.w = ((B.w * F - C.w * E) + (B.w * D - A.w * E) * deltaPixelX) * invWSquare;
        case Yw3d_SRT_Vector3:
            ddy.z = ((B.z * F - C.z * E) + (B.z * D - A.z * E) * deltaPixelX) * invWSquare;
        case Yw3d_SRT_Vector2:
            ddy.y = ((B.y * F - C.y * E) + (B.y * D - A.y * E) * deltaPixelX) * invWSquare;
        case Yw3d_SRT_Float32:
            ddy.x = ((B.x * F - C.x * E) + (B.x * D - A.x * E) * deltaPixelX) * invWSquare;
        case Yw3d_SRT_Unused:
        default:
            break;
        }
    }
}
