// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Sky vertex shader.

    // Vertex input format:
    // 0 - Vector3 position;
    void DemoPBRIBLSkyVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Use sky mesh vertex position, in local space, as index into cubemap.
        vsShaderOutput[0] = vsShaderInput[0];
    }

    Yw3dShaderRegisterType DemoPBRIBLSkyVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3; // Vertex local position.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // ------------------------------------------------------------------
    // Sky pixel shader.

    bool DemoPBRIBLSkyPixelShader::MightKillPixels()
    {
        return false;
    }

    bool DemoPBRIBLSkyPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        // Sample main texture.
        float3 texCoord = normalize(input[0]);
        texCoord = SampleSphericalMap(texCoord);

        float4 texColor;
        SampleTexture(texColor, 0, texCoord.x, texCoord.y);

        // linear to srgb.
        texColor = Vector4((float)pow(texColor.x, 1.0f / 2.2f), (float)pow(texColor.y, 1.0f / 2.2f), (float)pow(texColor.z, 1.0f / 2.2f), texColor.a);

        color = texColor;

        return true;
    }

    // Convert HDR equirectangular environment map to cubemap equivalent.
    // v must be normalized.
    Vector2 DemoPBRIBLSkyPixelShader::SampleSphericalMap(const Vector3& v)
    {
        Vector2 uv = Vector2(atan2(v.z, v.x), acos(v.y)); // tan(theta) = z / x; cos(phi) = y / r;
        uv *= Vector2(-0.1591f, 0.3183f); // (1/2pi, 1/pi)
        uv += Vector2(0.5f, 0.0f);
        return uv;
    }

    // ------------------------------------------------------------------
    // PBR vertex shader.

    // Vertex input format:
    // 0 - Vector3 position;
    void DemoPBRIBLVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        vsShaderOutput[0] = vsShaderInput[4];

        //const float3 normal = vsShaderInput[1];
        //const float4 tangent = vsShaderInput[2];
        //const float3 binormal = cross(normal, float3(tangent)) * tangent.w;
        //vsShaderOutput[1] = float3(tangent);
        //vsShaderOutput[2] = binormal;
        //vsShaderOutput[3] = normal;
    }

    Yw3dShaderRegisterType DemoPBRIBLVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector2; // Texcoord.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // ------------------------------------------------------------------
    // PBR default pixel shader.

    bool DemoPBRIBLPixelShader::MightKillPixels()
    {
        return false;
    }

    // Shader main entry.
    bool DemoPBRIBLPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        color = float4(0.0f, 1.0f, 0.0f, 1.0f);

        return true;
    }
}
