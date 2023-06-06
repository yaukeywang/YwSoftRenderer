// Add by Yaukey at 2023-05-14.
// YW Soft Renderer demo with PBR-IBL-Textured shader class.

#include "YwDemoPBRIBLTexturedShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR-IBL-Textured shader.

    // PBR vertex shader.

    // Vertex input format:
    // 0 - Vector3 position;
    void DemoPBRIBLTexturedVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Texcoord.
        vsShaderOutput[0] = vsShaderInput[4];

        // Inverse of transpose of world matrix.
        const float44 matInvTrs = GetMatrix(0);

        // TBN.
        const Vector3 normal = vsShaderInput[1] * matInvTrs;
        const float4 tangent = vsShaderInput[2] * matInvTrs;
        const Vector3 binormal = cross(normal, Vector3(tangent)) * tangent.w;
        vsShaderOutput[1] = Vector3(tangent);
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
    bool DemoPBRIBLTexturedPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        const float2 texCoord = float2(input[0]);

        // Form TBN matrix from tangent space to model space.
        const Vector3 tangent = normalize(Vector3(input[1]));
        const Vector3 binormal = normalize(Vector3(input[2]));
        const Vector3 normal = normalize(Vector3(input[3]));
        const Vector3 worldPos = Vector3(input[4]);

        float33 TBN = float33(
            tangent.x, tangent.y, tangent.z,
            binormal.x, binormal.y, binormal.z,
            normal.x, normal.y, normal.z
        );

        // Material properties.
        const Vector3 albedo = pow(Vector3(tex2D(0, 3, texCoord)), 2.2f);
        const Vector3 worldNormal = normalize(UnpackScaleNormal(tex2D(0, 4, texCoord), 1.0f) * TBN);
        const float metallic = tex2D(0, 5, texCoord).r;
        const float roughness = tex2D(0, 6, texCoord).r;
        const float ao = tex2D(0, 7, texCoord).r;

        const Vector3 cameraPos = GetVector(0);
        const Vector3 lightColor = GetVector(1);
        const Vector3 lightPositions[] = { GetVector(2), GetVector(3), GetVector(4), GetVector(5) };
 
        Vector3 N = worldNormal;
        Vector3 V = normalize(cameraPos - worldPos);
        Vector3 R = reflect(-V, N);

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        Vector3 F0 = Vector3(0.04f);
        F0 = lerp(F0, albedo, metallic);

        // Reflectance equation.
        Vector3 Lo = Vector3(0.0f);
        int32_t lightCount = sizeof(lightPositions) / sizeof(lightPositions[0]);
        for (int32_t i = 0; i < lightCount; i++)
        {
            Vector3 lightPos = lightPositions[i];

            // calculate per-light radiance
            Vector3 L = normalize(lightPos - worldPos);
            Vector3 H = normalize(V + L);
            float distance = length(lightPos - worldPos);
            float attenuation = 1.0f / (distance * distance);
            Vector3 radiance = lightColor * attenuation;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);   
            float G = GeometrySmith(N, V, L, roughness);    
            Vector3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);        
            
            Vector3 numerator = NDF * G * F;
            float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // + 0.0001 to prevent divide by zero
            Vector3 specular = numerator / denominator;
            
             // kS is equal to Fresnel
            Vector3 kS = F;
            // for energy conservation, the diffuse and specular light can't
            // be above 1.0 (unless the surface emits light); to preserve this
            // relationship the diffuse component (kD) should equal 1.0 - kS.
            Vector3 kD = Vector3(1.0f) - kS;
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

        Vector3 ambient = (kD * diffuse + specular) * ao;

        Vector3 finalColor = ambient + Lo;

        // HDR tonemapping
        finalColor = finalColor / (finalColor + Vector3(1.0f));

        // gamma correct
        finalColor = pow(finalColor, Vector3(1.0f / 2.2f));

        color = finalColor;

        return true;
    }
}
