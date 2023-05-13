// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLShaderSky.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Cube map rendering shader.

    // Cube map pixel shader.

    // Vertex input format:
    // 0 - Vector3 position;
    void DemoPBRIBLCubeMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Use sky mesh vertex position, in local space, as index into cubemap.
        vsShaderOutput[0] = vsShaderInput[0];
    }

    Yw3dShaderRegisterType DemoPBRIBLCubeMapVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3; // Vertex local position.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // Cube map pixel shader.

    bool DemoPBRIBLCubeMapPixelShader::MightKillPixels()
    {
        return false;
    }

    bool DemoPBRIBLCubeMapPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        // Sample main texture.
        float3 texCoord = input[0];
        float4 texColor = texCUBElod(0, 0, Vector4(texCoord, 1));

        // We can bake into cube texture.
        // linear to srgb.
        texColor = Vector4((float)pow(texColor.x, 1.0f / 2.2f), (float)pow(texColor.y, 1.0f / 2.2f), (float)pow(texColor.z, 1.0f / 2.2f), texColor.a);

        color = texColor;

        return true;
    }
}
