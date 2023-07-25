// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLShaderSky.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Cube map rendering shader.

    // Cube map pixel shader.

    // Vertex input format:
    // 0 - float3 position;
    void DemoPBRIBLCubeMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, float4& position, Yw3dShaderRegister* vsShaderOutput)
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

    bool DemoPBRIBLCubeMapPixelShader::Execute(const Yw3dShaderRegister* input, float4& color, float& depth)
    {
        // Sample main texture.
        float3 texCoord = input[0];
        //float4 texColor = texCUBE(0, 0, texCoord);
        float4 texColor = texCUBElod(0, 0, float4(texCoord, 0)); // Highest mipmap level.

        // linear to srgb.
        float gammaFactor = 1.0f / 2.2f;
        texColor = float4((float)pow(texColor.x, gammaFactor), (float)pow(texColor.y, gammaFactor), (float)pow(texColor.z, gammaFactor), texColor.a);

        color = texColor;

        return true;
    }
}
