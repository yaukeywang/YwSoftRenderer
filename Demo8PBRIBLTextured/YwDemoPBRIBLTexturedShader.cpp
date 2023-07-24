// Add by Yaukey at 2023-05-14.
// YW Soft Renderer demo with PBR-IBL-Textured shader class.

#include "YwDemoPBRIBLTexturedShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR-IBL-Textured shader.

    // PBR vertex shader.

    // Vertex input format:
    // 0 - float3 position;
    void DemoPBRIBLTexturedVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, float4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Texcoord.
        vsShaderOutput[0] = vsShaderInput[4];

        // Inverse of transpose of world matrix.
        const float44 matInvTrs = GetMatrix(0);

        // TBN.
        const float3 normal = vsShaderInput[1] * matInvTrs;
        const float4 tangent = vsShaderInput[2] * matInvTrs;
        const float3 binormal = cross(normal, float3(tangent)) * tangent.w;
        vsShaderOutput[1] = float3(tangent);
        vsShaderOutput[2] = binormal;
        vsShaderOutput[3] = normal;

        // Vertex world position.
        vsShaderOutput[4] = vsShaderInput[0] * (*GetWorldMatrix());
    }

    Yw3dShaderRegisterType DemoPBRIBLTexturedVertexShader::GetOutputRegisters(uint32_t shaderRegister)
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
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // PBR default pixel shader.

    bool DemoPBRIBLTexturedPixelShader::MightKillPixels()
    {
        return false;
    }

    // Shader main entry.
    bool DemoPBRIBLTexturedPixelShader::Execute(const Yw3dShaderRegister* input, float4& color, float& depth)
    {
        const float2 texCoord = float2(input[0]);

        // Form TBN matrix from tangent space to model space.
        const float3 tangent = normalize(float3(input[1]));
        const float3 binormal = normalize(float3(input[2]));
        const float3 normal = normalize(float3(input[3]));
        const float3 worldPos = float3(input[4]);

        float33 TBN = float33(
            tangent.x, tangent.y, tangent.z,
            binormal.x, binormal.y, binormal.z,
            normal.x, normal.y, normal.z
        );

        // Material properties.
        const float3 albedo = pow(float3(tex2D(0, 3, texCoord)), 2.2f);
        const float3 worldNormal = normalize(UnpackScaleNormal(tex2D(0, 4, texCoord), 1.0f) * TBN);
        const float metallic = tex2D(0, 5, texCoord).r;
        const float roughness = tex2D(0, 6, texCoord).r;
        const float ao = tex2D(0, 7, texCoord).r;

        const float3 cameraPos = GetVector(0);
        const float3 lightColor = GetVector(1);
        const float3 lightPositions[] = { GetVector(2), GetVector(3), GetVector(4), GetVector(5) };
 
        float3 N = worldNormal;
        float3 V = normalize(cameraPos - worldPos);
        float3 R = reflect(-V, N);

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        float3 F0 = float3(0.04f);
        F0 = lerp(F0, albedo, metallic);

        // Reflectance equation.
        float3 Lo = float3(0.0f);
        int32_t lightCount = sizeof(lightPositions) / sizeof(lightPositions[0]);
        for (int32_t i = 0; i < lightCount; i++)
        {
            float3 lightPos = lightPositions[i];

            // calculate per-light radiance
            float3 L = normalize(lightPos - worldPos);
            float3 H = normalize(V + L);
            float distance = length(lightPos - worldPos);
            float attenuation = 1.0f / (distance * distance);
            float3 radiance = lightColor * attenuation;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);   
            float G = GeometrySmith(N, V, L, roughness);    
            float3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);        
            
            float3 numerator = NDF * G * F;
            float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // + 0.0001 to prevent divide by zero
            float3 specular = numerator / denominator;
            
             // kS is equal to Fresnel
            float3 kS = F;
            // for energy conservation, the diffuse and specular light can't
            // be above 1.0 (unless the surface emits light); to preserve this
            // relationship the diffuse component (kD) should equal 1.0 - kS.
            float3 kD = float3(1.0f) - kS;
            // multiply kD by the inverse metalness such that only non-metals 
            // have diffuse lighting, or a linear blend if partly metal (pure metals
            // have no diffuse light).
            kD *= 1.0f - metallic;	                
                
            // scale light by NdotL
            float NdotL = max(dot(N, L), 0.0f);        

            // add to outgoing radiance Lo
            Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        }

        // ambient lighting (we now use IBL as the ambient term)
        float3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0f), F0, roughness);

        float3 kS = F;
        float3 kD = float3(1.0f) - kS;
        kD *= 1.0f - metallic;

        float3 irradiance = texCUBE(0, 0, N);
        float3 diffuse = irradiance * albedo;

        // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0f;
        float3 prefilteredColor = texCUBElod(0, 1, float4(R, roughness * MAX_REFLECTION_LOD));
        float2 brdf = tex2D(0, 2, float2(max(dot(N, V), 0.0f), roughness));
        float3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        float3 ambient = (kD * diffuse + specular) * ao;

        float3 finalColor = ambient + Lo;

        // HDR tonemapping
        finalColor = finalColor / (finalColor + float3(1.0f));

        // gamma correct
        finalColor = pow(finalColor, float3(1.0f / 2.2f));

        color = finalColor;

        return true;
    }
}
