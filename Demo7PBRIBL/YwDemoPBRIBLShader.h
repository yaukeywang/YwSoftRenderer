// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#ifndef __YW_DEMO_PBR_IBL_SHADER_H__
#define __YW_DEMO_PBR_IBL_SHADER_H__

#include "YwGraphics.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Equirectangular map to cubemap shader.

    // Equirectangular map to cubemap vertex shader.
    class DemoPBRIBLEquirectangularMapVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Equirectangular map to cubemap pixel shader.
    class DemoPBRIBLEquirectangularMapPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    private:
        // Convert HDR equirectangular environment map to cubemap equivalent.
        Vector2 SampleSphericalMap(const Vector3& v);
    };

    // ------------------------------------------------------------------
    // Sky shader.

    // Sky vertex shader.
    class DemoPBRIBLSkyVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Sky pixel shader.
    class DemoPBRIBLSkyPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    private:
        // Convert HDR equirectangular environment map to cubemap equivalent.
        Vector2 SampleSphericalMap(const Vector3& v);
    };

    // ------------------------------------------------------------------
    // PBR shader.

    // ------------------------------------------------------------------

    // PBR vertex shader.
    class DemoPBRIBLVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // PBR pixel shader.
    class DemoPBRIBLPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };
}

#endif //!__YW_DEMO_PBR_IBL_SHADER_H__
