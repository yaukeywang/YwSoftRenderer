// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL shader class.

#include "YwDemoPBRIBLShader.h"

namespace yw
{
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
        float4 texColor = texCUBElod(0, 0, Vector4(texCoord, 1));

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
        Vector3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);

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
}
