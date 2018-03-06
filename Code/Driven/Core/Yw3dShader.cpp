// Add by Yaukey at 2018-02-22.
// YW Soft Renderer 3d shader class( vertex-shader, triangle-shader and pixel shader).

#include "stdafx.h"
#include "Yw3dShader.h"

namespace yw
{
    void IYw3dPixelShader::SetInfo(const Yw3dShaderRegisterType* vsOutputs, const Yw3dTriangleInfo* triangleInfo)
    {
        m_VSOutputs = vsOutputs;
        m_TriangleInfo = triangleInfo;
    }

    // Partial derivative equations taken from
    // "MIP-Map Level Selection for Texture Mapping",
    // Jon P. Ewins, Member, IEEE, Marcus D. Waller,
    // Martin White, and Paul F. Lister, Member, IEEE
    void IYw3dPixelShader::GetPartialDerivatives(uint32_t register, Vector4& ddx, Vector4& ddy) const
    {

    }
}
