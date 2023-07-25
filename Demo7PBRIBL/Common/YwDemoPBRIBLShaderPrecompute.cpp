// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLShaderPrecompute.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Equirectangular map to cubemap shader.

    // Equirectangular map to cubemap vertex shader.

    // Vertex input format:
    // 0 - float3 position;
    void DemoPBRIBLEquirectangularMap2CubeMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, float4& position, Yw3dShaderRegister* vsShaderOutput)
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

    bool DemoPBRIBLEquirectangularMap2CubeMapPixelShader::Execute(const Yw3dShaderRegister* input, float4& color, float& depth)
    {
        // Sample main texture.
        float3 texCoord = normalize(input[0]);
        texCoord = SampleSphericalMap(texCoord);

        float4 texColor = texCUBElod(0, 0, float4(texCoord, 0.0f));

        // Equirectangular map is hdr, keep the render target is also hdr.
        color = texColor;

        return true;
    }

    // Convert HDR equirectangular environment map to cubemap equivalent.
    // v must be normalized.
    float2 DemoPBRIBLEquirectangularMap2CubeMapPixelShader::SampleSphericalMap(const float3& v)
    {
        float2 uv = float2(atan2(v.z, v.x), acos(v.y)); // tan(theta) = z / x; cos(phi) = y / r;
        uv *= float2(-0.1591f, 0.3183f); // (1/2pi, 1/pi)
        uv += float2(0.5f, 0.0f);
        return uv;
    }

    // ------------------------------------------------------------------
    // Cube map to irrandiance map shader.

    // Cube map to irrandiance map vertex shader.

    // Shader main entry.
    void DemoPBRIBLCubeMap2IrrandianceMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, float4& position, Yw3dShaderRegister* vsShaderOutput)
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
    bool DemoPBRIBLCubeMap2IrrandianceMapPixelShader::Execute(const Yw3dShaderRegister* input, float4& color, float& depth)
    {
        // Get world position.
        float3 worldPos = input[0];

        float3 N = normalize(worldPos);
        float3 irradiance = float3::Zero();

        // tangent space calculation from origin point
        float3 up = float3(0.0f, 1.0f, 0.0f);
        float3 right = normalize(cross(up, N));
        up = normalize(cross(N, right));

        float sampleDelta = 0.025f;
        int32_t nrSamples = 0;
        for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
        {
            for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
            {
                // Spherical to cartesian (in tangent space).
                float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

                // Tangent space to world.
                float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

                // Sample and increase irradiance.
                float4 envMapColor = texCUBElod(0, 0, float4(sampleVec, 0.0f));

                irradiance += float3(envMapColor) * cos(theta) * sin(theta);
                nrSamples++;
            }
        }

        irradiance = PI * irradiance * (1.0f / float(nrSamples));

        // Final output.
        color = float4(irradiance, 1.0f);

        return true;
    }

    // ------------------------------------------------------------------
    // Pre-filter reflection map shader.

    // Pre-filter reflection map vertex shader.

    void DemoPBRIBLPrefilterReflectionMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, float4& position, Yw3dShaderRegister* vsShaderOutput)
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

    bool DemoPBRIBLPrefilterReflectionMapPixelShader::Execute(const Yw3dShaderRegister* input, float4& color, float& depth)
    {
        // Get world position.
        const float3& worldPos = input[0];
        const float roughness = GetFloat(0);

        float3 N = normalize(worldPos);

        // make the simplyfying assumption that V equals R equals the normal 
        float3 R = N;
        float3 V = R;

        const uint32_t SAMPLE_COUNT = 1024u;
        float3 prefilteredColor = float3(0.0f);
        float totalWeight = 0.0f;

        for (uint32_t i = 0u; i < SAMPLE_COUNT; ++i)
        {
            // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
            float2 Xi = Hammersley(i, SAMPLE_COUNT);
            float3 H = ImportanceSampleGGX(Xi, N, roughness);
            float3 L = normalize(2.0f * dot(V, H) * H - V);

            float NdotL = max(dot(N, L), 0.0f);
            if (NdotL > 0.0f)
            {
                // sample from the environment's mip level based on roughness/pdf
                float D = DistributionGGX(N, H, roughness);
                float NdotH = max(dot(N, H), 0.0f);
                float HdotV = max(dot(H, V), 0.0f);
                float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;

                float resolution = 512.0f; // resolution of source cubemap (per face)
                float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
                float saSample = 1.0f / (float(SAMPLE_COUNT) * pdf + 0.0001f);

                float mipLevel = (roughness == 0.0f) ? 0.0f : (0.5f * log2(saSample / saTexel));

                prefilteredColor += texCUBElod(0, 0, float4(L, mipLevel)) * NdotL;
                totalWeight += NdotL;
            }
        }

        prefilteredColor = prefilteredColor / totalWeight;
        color = float4(prefilteredColor, 1.0f);

        return true;
    }

    // ------------------------------------------------------------------
    // Pre-integrate brdf map shader.

    // Pre-integrate brdf map vertex shader.

    void DemoPBRIBLPreintegrateBRDFMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, float4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The NDC vertex position.
        position = float4(vsShaderInput[0], 1.0f);

        // Vertex texcoord.
        vsShaderOutput[0] = vsShaderInput[1];
    }

    Yw3dShaderRegisterType DemoPBRIBLPreintegrateBRDFMapVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector2; // Vertex texcoord.
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // Pre-integrate brdf map pixel shader.

    bool DemoPBRIBLPreintegrateBRDFMapPixelShader::MightKillPixels()
    {
        return false;
    }

    bool DemoPBRIBLPreintegrateBRDFMapPixelShader::Execute(const Yw3dShaderRegister* input, float4& color, float& depth)
    {
        float2 texCoords = input[0];
        float2 integratedBRDF = IntegrateBRDF(texCoords.x, texCoords.y);
        color = integratedBRDF;

        return true;
    }

    float2 DemoPBRIBLPreintegrateBRDFMapPixelShader::IntegrateBRDF(float NdotV, float roughness)
    {
        float3 V;
        V.x = sqrt(1.0f - NdotV * NdotV);
        V.y = 0.0f;
        V.z = NdotV;

        float A = 0.0f;
        float B = 0.0f;

        float3 N = float3(0.0f, 0.0f, 1.0f);

        const uint32_t SAMPLE_COUNT = 1024u;
        for (uint32_t i = 0u; i < SAMPLE_COUNT; ++i)
        {
            // generates a sample vector that's biased towards the
            // preferred alignment direction (importance sampling).
            float2 Xi = Hammersley(i, SAMPLE_COUNT);
            float3 H = ImportanceSampleGGX(Xi, N, roughness);
            float3 L = normalize(2.0f * dot(V, H) * H - V);

            float NdotL = max(L.z, 0.0f);
            float NdotH = max(H.z, 0.0f);
            float VdotH = max(dot(V, H), 0.0f);

            if (NdotL > 0.0f)
            {
                float G = GeometrySmith(N, V, L, roughness);
                float G_Vis = (G * VdotH) / (NdotH * NdotV);
                float Fc = pow(1.0f - VdotH, 5.0f);

                A += (1.0f - Fc) * G_Vis;
                B += Fc * G_Vis;
            }
        }

        A /= float(SAMPLE_COUNT);
        B /= float(SAMPLE_COUNT);
        return float2(A, B);
    }
}
