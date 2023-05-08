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
        // Get world position.
        const Vector3& worldPos = input[0];
        const float roughness = GetFloat(0);

        Vector3 N = normalize(worldPos);

        // make the simplyfying assumption that V equals R equals the normal 
        Vector3 R = N;
        Vector3 V = R;

        const uint32_t SAMPLE_COUNT = 1024u;
        Vector3 prefilteredColor = Vector3(0.0f);
        float totalWeight = 0.0f;

        for (uint32_t i = 0u; i < SAMPLE_COUNT; ++i)
        {
            // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
            Vector2 Xi = Hammersley(i, SAMPLE_COUNT);
            Vector3 H = ImportanceSampleGGX(Xi, N, roughness);
            Vector3 L = normalize(2.0f * dot(V, H) * H - V);

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

                prefilteredColor += texCUBElod(0, 0, Vector4(L, mipLevel)) * NdotL;
                totalWeight += NdotL;
            }
        }

        prefilteredColor = prefilteredColor / totalWeight;
        color = Vector4(prefilteredColor, 1.0f);

        return true;
    }

    float DemoPBRIBLPrefilterReflectionMapPixelShader::DistributionGGX(const Vector3& N, const Vector3& H, const float roughness)
    {
        float a = roughness * roughness;
        float a2 = a * a;
        float NdotH = (float)max(dot(N, H), 0.0f);
        float NdotH2 = NdotH * NdotH;

        float nom = a2;
        float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
        denom = PI * denom * denom;

        return nom / denom;
    }

    float DemoPBRIBLPrefilterReflectionMapPixelShader::RadicalInverse_VdC(uint32_t bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return (float)(float(bits) * 2.3283064365386963e-10); // / 0x100000000
    }

    Vector2 DemoPBRIBLPrefilterReflectionMapPixelShader::Hammersley(const uint32_t i, const uint32_t N)
    {
        return Vector2(float(i) / float(N), RadicalInverse_VdC(i));
    }

    Vector3 DemoPBRIBLPrefilterReflectionMapPixelShader::ImportanceSampleGGX(const Vector2& Xi, const Vector3& N, const float roughness)
    {
        float a = roughness * roughness;

        float phi = 2.0f * PI * Xi.x;
        float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

        // from spherical coordinates to cartesian coordinates - halfway vector
        Vector3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;

        // from tangent-space H vector to world-space sample vector
        Vector3 up = abs(N.z) < 0.999f ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(1.0f, 0.0f, 0.0f);
        Vector3 tangent = normalize(cross(up, N));
        Vector3 bitangent = cross(N, tangent);

        Vector3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return normalize(sampleVec);
    }

    // ------------------------------------------------------------------
    // Pre-integrate brdf map shader.

    // Pre-integrate brdf map vertex shader.

    void DemoPBRIBLPreintegrateBRDFMapVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The NDC vertex position.
        position = Vector4(vsShaderInput[0], 1.0f);

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

    bool DemoPBRIBLPreintegrateBRDFMapPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        Vector2 texCoords = input[0];
        Vector2 integratedBRDF = IntegrateBRDF(texCoords.x, texCoords.y);
        color = integratedBRDF;

        return true;
    }

    float DemoPBRIBLPreintegrateBRDFMapPixelShader::RadicalInverse_VdC(uint32_t bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return (float)(float(bits) * 2.3283064365386963e-10); // / 0x100000000
    }

    Vector2 DemoPBRIBLPreintegrateBRDFMapPixelShader::Hammersley(const uint32_t i, const uint32_t N)
    {
        return Vector2(float(i) / float(N), RadicalInverse_VdC(i));
    }

    Vector3 DemoPBRIBLPreintegrateBRDFMapPixelShader::ImportanceSampleGGX(const Vector2& Xi, const Vector3& N, const float roughness)
    {
        float a = roughness * roughness;

        float phi = 2.0f * PI * Xi.x;
        float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

        // from spherical coordinates to cartesian coordinates - halfway vector
        Vector3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;

        // from tangent-space H vector to world-space sample vector
        Vector3 up = abs(N.z) < 0.999f ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(1.0f, 0.0f, 0.0f);
        Vector3 tangent = normalize(cross(up, N));
        Vector3 bitangent = cross(N, tangent);

        Vector3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return normalize(sampleVec);
    }

    float DemoPBRIBLPreintegrateBRDFMapPixelShader::GeometrySchlickGGX(float NdotV, float roughness)
    {
        // note that we use a different k for IBL
        float a = roughness;
        float k = (a * a) / 2.0f;

        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;

        return nom / denom;
    }

    float DemoPBRIBLPreintegrateBRDFMapPixelShader::GeometrySmith(const Vector3& N, const Vector3& V, const Vector3& L, float roughness)
    {
        float NdotV = max(dot(N, V), 0.0f);
        float NdotL = max(dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    }

    Vector2 DemoPBRIBLPreintegrateBRDFMapPixelShader::IntegrateBRDF(float NdotV, float roughness)
    {
        Vector3 V;
        V.x = sqrt(1.0f - NdotV * NdotV);
        V.y = 0.0f;
        V.z = NdotV;

        float A = 0.0f;
        float B = 0.0f;

        Vector3 N = Vector3(0.0f, 0.0f, 1.0f);

        const uint32_t SAMPLE_COUNT = 1024u;
        for (uint32_t i = 0u; i < SAMPLE_COUNT; ++i)
        {
            // generates a sample vector that's biased towards the
            // preferred alignment direction (importance sampling).
            Vector2 Xi = Hammersley(i, SAMPLE_COUNT);
            Vector3 H = ImportanceSampleGGX(Xi, N, roughness);
            Vector3 L = normalize(2.0f * dot(V, H) * H - V);

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
        return Vector2(A, B);
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
        //float4 texColor = texCUBE(0, 0, texCoord);
        float4 texColor = texCUBElod(0, 0, float4(texCoord, 2.0f));

        // We can bake into cube texture.
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

        // Texcoord.
        vsShaderOutput[0] = vsShaderInput[4];

        // TBN.
        const float3 normal = vsShaderInput[1];
        const float4 tangent = vsShaderInput[2];
        const float3 binormal = cross(normal, float3(tangent)) * tangent.w;
        vsShaderOutput[1] = float3(tangent);
        vsShaderOutput[2] = binormal;
        vsShaderOutput[3] = normal;

        // Vertex world position.
        vsShaderOutput[4] = vsShaderInput[0] * (*GetWorldMatrix());

        // Vertex world normal.
        vsShaderOutput[5] = float3(float4(normal, 0.0f) * (*GetWorldMatrix()));
    }

    Yw3dShaderRegisterType DemoPBRIBLVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector2; // Texcoord.
        case 1:
            return Yw3d_SRT_Vector3; // Tangent of TBN.
        case 2:
            return Yw3d_SRT_Vector3; // Bi-Normal of TBN.
        case 3:
            return Yw3d_SRT_Vector3; // Normal of TBN.
        case 4:
            return Yw3d_SRT_Vector3; // Vertex world position.
        case 5:
            return Yw3d_SRT_Vector3; // Vertex world normal.
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
        const float2 texCoord = float2(input[0]);

        // Form TBN matrix from tangent space to model space.
        const float3 tangent = normalize(float3(input[1]));
        const float3 binormal = normalize(float3(input[2]));
        const float3 normal = normalize(float3(input[3]));
        const float3 worldPos = float3(input[4]);
        const float3 worldNormal = normalize(float3(input[5]));

        const float metallic = GetFloat(0);
        const float roughness = GetFloat(1);
        const float3 albedo = GetVector(0);
        const float3 cameraPos = GetVector(1);

        Vector3 N = worldNormal;
        Vector3 V = normalize(cameraPos - worldPos);
        Vector3 R = reflect(-V, N);

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        Vector3 F0 = Vector3(0.04f);
        F0 = lerp(F0, albedo, metallic);

        // ambient lighting (we now use IBL as the ambient term)
        Vector3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);

        Vector3 kS = F;
        Vector3 kD = Vector3::One() - kS;
        kD *= 1.0f - metallic;

        Vector3 irradiance = texCUBE(0, 0, N);
        Vector3 diffuse = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0f;
        Vector3 prefilteredColor = texCUBElod(0, 1, Vector4(R, roughness * MAX_REFLECTION_LOD));
        Vector2 brdf = tex2D(0, 2, Vector2(max(dot(N, V), 0.0f), roughness));
        Vector3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        //Vector3 ambient = (kD * diffuse + specular) * ao;
        Vector3 ambient = (kD * diffuse + specular);

        //Vector3 finalColor = ambient + Lo;
        Vector3 finalColor = ambient;

        // HDR tonemapping
        finalColor = finalColor / (finalColor + Vector3(1.0f));

        // gamma correct
        finalColor = pow(finalColor, Vector3(1.0f / 2.2f));

        color = finalColor;

        return true;
    }

    Vector3 DemoPBRIBLPixelShader::fresnelSchlickRoughness(float cosTheta, const Vector3& F0, float roughness)
    {
        const float smoothness = 1.0f - roughness;
        return F0 + (maximum(Vector3(smoothness), F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }
}
