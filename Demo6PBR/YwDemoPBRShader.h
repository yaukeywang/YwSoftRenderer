// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR shader class.

#ifndef __YW_DEMO_PBR_SHADER_H__
#define __YW_DEMO_PBR_SHADER_H__

#include "YwGraphics.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR shader.

    // PBR vertex shader.
    class DemoPBRVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // PBR pixel shader.
    class DemoPBRPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    private:
        // Disney implementation of PBR diffuse part.
        Vector3 DisneyDiffuse(Vector3 albedo, float NdotL, float NdotV, float LdotH, float roughness, float subsurface);

        // Cook-Torrance implementation of PBR specular part.
        Vector3 CookTorranceSpecular(float NdotL, float LdotH, float NdotH, float NdotV, float roughness, float specularColor);

        // Utility functions for calculating diffuse and specular.
        float sqr(float value);
        float SchlickFresnel(float value);
        float G1(float k, float x);
    };
}

#endif //!__YW_DEMO_PBR_SHADER_H__
