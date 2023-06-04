// Add by Yaukey at 2023-05-14.
// YW Soft Renderer demo with PBR-IBL-Textured shader class.

#ifndef __YW_DEMO_PBR_IBL_SHADER_TEXTURED_H__
#define __YW_DEMO_PBR_IBL_SHADER_TEXTURED_H__

#include "YwDemoPBRIBLShaderCommon.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR-IBL-Textured shader.
    
    // PBR vertex shader.
    class DemoPBRIBLTexturedVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // PBR pixel shader.
    class DemoPBRIBLTexturedPixelShader : public IYw3dPixelShader, public DemoPBRIBLShaderCommon
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };
}

#endif //!__YW_DEMO_PBR_IBL_SHADER_TEXTURED_H__
