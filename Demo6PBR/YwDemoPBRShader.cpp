// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR shader class.

#include "YwDemoPBRShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR vertex shader.

    void DemoPBRVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // Get current position.
        Vector4 curPos = vsShaderInput[0];
        curPos.w = 1.0f;

        // The projection vertex position.
        position = curPos * (*GetWVPMatrix());

        // The projection pos.
        vsShaderOutput[0] = position;
    }

    Yw3dShaderRegisterType DemoPBRVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector4; // Projective pos.
        case 1:
            return Yw3d_SRT_Vector4; // EdgeA.
        case 2:
            return Yw3d_SRT_Vector4; // EdgeB.
        case 3:
            return Yw3d_SRT_Float32; // Case.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // ------------------------------------------------------------------
    // PBR default pixel shader.

    bool DemoPBRPixelShader::MightKillPixels()
    {
        return false;
    }

    // Shader main entry.
    bool DemoPBRPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        return true;
    }
}
