// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#ifndef __YW_DEMO_PBR_IBL_SHADER_H__
#define __YW_DEMO_PBR_IBL_SHADER_H__

#include "YwGraphics.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Equirectangular map to cube map shader.

    // Equirectangular map to cube map vertex shader.
    class DemoPBRIBLEquirectangularMap2CubeMapVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Equirectangular map to cube map pixel shader.
    class DemoPBRIBLEquirectangularMap2CubeMapPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    private:
        // Convert HDR equirectangular environment map to cube map equivalent.
        Vector2 SampleSphericalMap(const Vector3& v);
    };

    // ------------------------------------------------------------------
    // Cube map to irrandiance map shader.

    // Cube map to irrandiance map vertex shader.
    class DemoPBRIBLCubeMap2IrrandianceMapVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Cube map to irrandiance map pixel shader.
    class DemoPBRIBLCubeMap2IrrandianceMapPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };

    // ------------------------------------------------------------------
    // Pre-filter reflection map shader.

    // Pre-filter reflection map vertex shader.
    class DemoPBRIBLPrefilterReflectionMapVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Pre-filter reflection map pixel shader.
    class DemoPBRIBLPrefilterReflectionMapPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };

    // ------------------------------------------------------------------
    // Cube map rendering shader.

    // Cube map vertex shader.
    class DemoPBRIBLCubeMapVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Cube map pixel shader.
    class DemoPBRIBLCubeMapPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
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
