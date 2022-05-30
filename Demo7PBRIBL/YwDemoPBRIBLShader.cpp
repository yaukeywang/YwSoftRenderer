// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Equirectangular map to cubemap shader.

    // Equirectangular map to cubemap vertex shader.

    // Vertex input format:
    // 0 - Vector3 position;
    void DemoPBRIBLEquirectangularMap2CubeMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Use sky mesh vertex position, in local space, as index into cubemap.
        vsShaderOutput[0] = vsShaderInput[0];
    }

    Yw3dShaderRegisterType DemoPBRIBLEquirectangularMap2CubeMapVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3; // Vertex local position.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // Equirectangular map to cubemap pixel shader.

    bool DemoPBRIBLEquirectangularMap2CubeMapPixelShader::MightKillPixels()
    {
        return false;
    }

    bool DemoPBRIBLEquirectangularMap2CubeMapPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        // Sample main texture.
        float3 texCoord = normalize(input[0]);
        texCoord = SampleSphericalMap(texCoord);

        float4 texColor;
        SampleTexture(texColor, 0, texCoord.x, texCoord.y); // SampleTexture -> texCube

        // Equirectangular map is hdr, keep the render target is also hdr.
        color = texColor;

        return true;
    }

    // Convert HDR equirectangular environment map to cubemap equivalent.
    // v must be normalized.
    Vector2 DemoPBRIBLEquirectangularMap2CubeMapPixelShader::SampleSphericalMap(const Vector3& v)
    {
        Vector2 uv = Vector2(atan2(v.z, v.x), acos(v.y)); // tan(theta) = z / x; cos(phi) = y / r;
        uv *= Vector2(-0.1591f, 0.3183f); // (1/2pi, 1/pi)
        uv += Vector2(0.5f, 0.0f);
        return uv;
    }

    // ------------------------------------------------------------------
    // Cube map to irrandiance map shader.

    // Cube map to irrandiance map vertex shader.
   
    // Shader main entry.
    void DemoPBRIBLCubeMap2IrrandianceMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Use sky mesh vertex position, in local space, as index into cube map.
        vsShaderOutput[0] = vsShaderInput[0] * (*GetWorldMatrix());
    }

    // Shader stream channel.
    Yw3dShaderRegisterType DemoPBRIBLCubeMap2IrrandianceMapVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3; // Vertex local position.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // Cube map to irrandiance map pixel shader.

    // Whether kill pixel or not.
    bool DemoPBRIBLCubeMap2IrrandianceMapPixelShader::MightKillPixels()
    {
        return false;
    }

    // Shader main entry.
    bool DemoPBRIBLCubeMap2IrrandianceMapPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        // Get world position.
        Vector3 worldPos = input[0];

        Vector3 N = normalize(worldPos);
        Vector3 irradiance = Vector3::Zero();

        // tangent space calculation from origin point
        Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
        Vector3 right = normalize(cross(up, N));
        up = normalize(cross(N, right));

        float sampleDelta = 0.025f;
        int32_t nrSamples = 0;
        for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
        {
            for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
            {
                // Spherical to cartesian (in tangent space).
                Vector3 tangentSample = Vector3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

                // Tangent space to world.
                Vector3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

                float4 envMapColor;
                SampleTexture(envMapColor, 0, sampleVec.x, sampleVec.y, sampleVec.z); // SampleTexture -> texCube

                irradiance += Vector3(envMapColor) * cos(theta) * sin(theta);
                nrSamples++;
            }
        }

        irradiance = PI * irradiance * (1.0f / float(nrSamples));

        // Final output.
        color = Vector4(irradiance, 1.0f);

        return true;
    }

    // ------------------------------------------------------------------
    // Pre-filter reflection map shader.

    // Pre-filter reflection map vertex shader.

    void DemoPBRIBLPrefilterReflectionMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Use sky mesh vertex position, in local space, as index into cube map.
        vsShaderOutput[0] = vsShaderInput[0] * (*GetWorldMatrix());
    }

    Yw3dShaderRegisterType DemoPBRIBLPrefilterReflectionMapVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3; // Vertex local position.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // Pre-filter reflection map pixel shader.

    bool DemoPBRIBLPrefilterReflectionMapPixelShader::MightKillPixels()
    {
        return false;
    }

    bool DemoPBRIBLPrefilterReflectionMapPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        return true;
    }

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
        float4 texColor;
        SampleTexture(texColor, 0, texCoord.x, texCoord.y, texCoord.z);

        // linear to srgb.
        texColor = Vector4((float)pow(texColor.x, 1.0f / 2.2f), (float)pow(texColor.y, 1.0f / 2.2f), (float)pow(texColor.z, 1.0f / 2.2f), texColor.a);

        color = texColor;

        return true;
    }

    // ------------------------------------------------------------------
    // PBR shader.

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
