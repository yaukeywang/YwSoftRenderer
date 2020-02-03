// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe shader class.

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
    };

    // Wireframe default pixel shader.
    class DemoTriangleShaderWireframeDefaultPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Get minimum edge distance.
        float EvalMinDistanceToEdges(const Yw3dShaderRegister* input);

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };

    // Wireframe pattern pixel shader.
    class DemoTriangleShaderWireframePatternPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Get minimum edge distance.
        float EvalMinDistanceToEdgesExt(const Yw3dShaderRegister* input, Vector3& edgeSqDists, Vector3& edgeCoords, uint32_t& edgeOrder0, uint32_t& edgeOrder1, uint32_t& edgeOrder2);

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };
}

#endif //!__YW_DEMO_TRIANGLE_SHADER_WIREFRAME_SHADER_H__
