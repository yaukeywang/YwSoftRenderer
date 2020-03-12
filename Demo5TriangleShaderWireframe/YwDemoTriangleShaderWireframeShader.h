// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe shader class.
// Port from Nvidia SolidWireframe Sample: https://developer.download.nvidia.cn/SDK/10/direct3d/samples.html#SolidWireframe
// Know Issue: 
//   1. SolidWirePattern demo shader (DemoTriangleShaderWireframeTriangleShader and DemoTriangleShaderWireframePatternPixelShader) has problems.
//   2. Alpha Blend not supported, it is need for better effect.

#ifndef __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_SHADER_H__
#define __YW_DEMO_TRIANGLE_SHADER_WIREFRAME_SHADER_H__

#include "YwGraphics.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Wireframe shader.

    // Wireframe vertex shader.
    class DemoTriangleShaderWireframeVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Wireframe default triangle shader.
    class DemoTriangleShaderWireframeTriangleShader : public IYw3dTriangleShader
    {
    protected:
        // Shader main entry.
        bool Execute(Yw3dShaderRegister* shaderRegister0, Yw3dShaderRegister* shaderRegister1, Yw3dShaderRegister* shaderRegister2);

    private:
        // From window pixel pos to projection frame at the specified z (view frame). 
        inline float2 ProjectToWindow(const float4& viewport, float4 pos)
        {
            return float2(
                viewport.x * 0.5f * ((pos.x / pos.w) + 1) + viewport.z,
                viewport.y * 0.5f * (1.0f - (pos.y / pos.w)) + viewport.w
            );
        }
    };

    // Wireframe default pixel shader.
    class DemoTriangleShaderWireframeDefaultPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    private:
        // Get minimum edge distance.
        float EvalMinDistanceToEdges(const Yw3dShaderRegister* input);
    };

    // Wireframe pattern pixel shader.
    class DemoTriangleShaderWireframePatternPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    private:
        // Get minimum edge distance.
        float EvalMinDistanceToEdgesExt(const Yw3dShaderRegister* input, float3& edgeSqDists, float3& edgeCoords, uint32_t& edgeOrder0, uint32_t& edgeOrder1, uint32_t& edgeOrder2);
    };
}

#endif //!__YW_DEMO_TRIANGLE_SHADER_WIREFRAME_SHADER_H__
