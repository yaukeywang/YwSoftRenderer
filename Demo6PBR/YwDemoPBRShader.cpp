// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR shader class.

#include "YwDemoPBRShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR vertex shader.

    // Vertex input format:
    // 0 - Vector3 position;
    // 1 - Vector3 normal;
    // 2 - Vector4 tangent;
    // 3 - Vector4 color;
    // 4 - Vector2 texcoord;
    // 5 - Vector2 texcoord2;
    void DemoPBRVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        vsShaderOutput[0] = vsShaderInput[4];

        const float3 normal = vsShaderInput[1];
        const float4 tangent = vsShaderInput[2];
        const float3 binormal = cross(normal, float3(tangent)) * tangent.w;
        vsShaderOutput[1] = float3(tangent);
        vsShaderOutput[2] = binormal;
        vsShaderOutput[3] = normal;
    }

    Yw3dShaderRegisterType DemoPBRVertexShader::GetOutputRegisters(uint32_t shaderRegister)
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
        default:
            return Yw3d_SRT_Unused;
        }
    }

    // ------------------------------------------------------------------
    // PBR default pixel shader.

    bool DemoPBRPixelShader::MightKillPixels()
    {
        return false;
    }

    // Shader main entry.
    bool DemoPBRPixelShader::Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
    {
        // Form TBN matrix from tangent space to model space.
        const float3 tangent = normalize(float3(input[1]));
        const float3 binormal = normalize(float3(input[2]));
        const float3 normal = normalize(float3(input[3]));
        float33 TBN = float33(
            tangent.x, tangent.y, tangent.z,
            binormal.x, binormal.y, binormal.z,
            normal.x, normal.y, normal.z
        );

        // Sample main texture.
        float2 texCoord = input[0];
        float4 texColor = tex2D(0, 0, texCoord);

        // Sample normal texture.
        float4 normalTexColor = tex2D(0, 1, texCoord);

        // Sample metallic map color.
        float4 metallicGlossMap;
        bool hasMetallicGlossMap = tex2DSafe(metallicGlossMap, 0, 2, texCoord);

        // Get all parameters.
        float3 tangentNormal = normalize(UnpackScaleNormal(normalTexColor, 1.0f));
        float3 modelNormal = normalize(tangentNormal * TBN);
        float3 worldNormal = normalize(float4(modelNormal, 0.0f) * *GetWorldMatrix());
        float3 lightDir = normalize(GetVector(0));
        float4 lightColor = GetVector(1);
        float4 albedo = GetVector(2);
        float4 specularColor = GetVector(3);
        float3 viewDir = normalize(GetVector(4));
        float metallic = GetFloat(0); // _Metallic/"Metallic"
        float smoothness = GetFloat(1); // _Glossiness/"Smoothness"
        float smoothnessScale = GetFloat(2); // _GlossMapScale/"Smoothness Scale"

        // Calculating the final brdf.
        FragmentCommonData s = FragmentSetup(albedo, texColor, hasMetallicGlossMap, metallicGlossMap, specularColor, metallic, smoothness, smoothnessScale);
        float4 c = UNITY_BRDF_PBS(s.diffColor, s.specColor, lightColor, s.oneMinusReflectivity, s.smoothness, worldNormal, -viewDir, -lightDir);
        color = c;

        return true;
    }

    float4 DemoPBRPixelShader::UNITY_BRDF_PBS(float3 diffColor, float3 specColor, float3 lightColor, float oneMinusReflectivity, float smoothness, float3 normal, float3 viewDir, float3 lightDir)
    {
        float perceptualRoughness = SmoothnessToPerceptualRoughness(smoothness);
        float3 halfDir = Unity_SafeNormalize(lightDir + viewDir);

        // NdotV should not be negative for visible pixels, but it can happen due to perspective projection and normal mapping
        // In this case normal should be modified to become valid (i.e facing camera) and not cause weird artifacts.
        // but this operation adds few ALU and users may not want it. Alternative is to simply take the abs of NdotV (less correct but works too).
        // Following define allow to control this. Set it to 0 if ALU is critical on your platform.
        // This correction is interesting for GGX with SmithJoint visibility function because artifacts are more visible in this case due to highlight edge of rough surface
        // Edit: Disable this code by default for now as it is not compatible with two sided lighting used in SpeedTree.
#if UNITY_HANDLE_CORRECTLY_NEGATIVE_NDOTV
    // The amount we shift the normal toward the view vector is defined by the dot product.
        float shiftAmount = dot(normal, viewDir);
        normal = shiftAmount < 0.0f ? normal + viewDir * (-shiftAmount + 1e-5f) : normal;
        // A re-normalization should be applied here but as the shift is small we don't do it to save ALU.
        //normal = normalize(normal);

        float nv = saturate(dot(normal, viewDir)); // TODO: this saturate should no be necessary here
#else
        float nv = abs(dot(normal, viewDir));    // This abs allow to limit artifact
#endif

        float nl = saturate(dot(normal, lightDir));
        float nh = saturate(dot(normal, halfDir));

        float lv = saturate(dot(lightDir, viewDir));
        float lh = saturate(dot(lightDir, halfDir));

        // Diffuse term
        float diffuseTerm = DisneyDiffuse(nv, nl, lh, perceptualRoughness) * nl;

        // Specular term
        // HACK: theoretically we should divide diffuseTerm by Pi and not multiply specularTerm!
        // BUT 1) that will make shader look significantly darker than Legacy ones
        // and 2) on engine side "Non-important" lights have to be divided by Pi too in cases when they are injected into ambient SH
        float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);

        // GGX with roughtness to 0 would mean no specular at all, using max(roughness, 0.002) here to match HDrenderloop roughtness remapping.
        roughness = max(roughness, 0.002f);
        float V = SmithJointGGXVisibilityTerm(nl, nv, roughness);
        float D = GGXTerm(nh, roughness);

        float specularTerm = V * D * PI; // Torrance-Sparrow model, Fresnel is applied later

#ifdef UNITY_COLORSPACE_GAMMA
        specularTerm = sqrt(max(1e-4f, specularTerm));
#endif

        // specularTerm * nl can be NaN on Metal in some cases, use max() to make sure it's a sane value
        specularTerm = max(0.0f, specularTerm * nl);
#if defined(_SPECULARHIGHLIGHTS_OFF)
        specularTerm = 0.0f;
#endif

        // surfaceReduction = Int D(NdotH) * NdotH * Id(NdotL>0) dH = 1/(roughness^2+1)
        float surfaceReduction = 0.0f;
#   ifdef UNITY_COLORSPACE_GAMMA
        surfaceReduction = 1.0f - 0.28f * roughness * perceptualRoughness; // 1-0.28*x^3 as approximation for (1/(x^4+1))^(1/2.2) on the domain [0;1]
#   else
        surfaceReduction = 1.0 / (roughness * roughness + 1.0); // fade \in [0.5;1]
#   endif

        // To provide true Lambert lighting, we need to be able to kill specular completely.
        specularTerm *= any(specColor) ? 1.0f : 0.0f;

        float grazingTerm = saturate(smoothness + (1.0f - oneMinusReflectivity));
        float3 indirectDiffuse = float3(0.0f, 0.0f, 0.0f);
        float3 indirectSpecular = float3(0.0f, 0.0f, 0.0f);
        float3 color = diffColor * (indirectDiffuse + lightColor * diffuseTerm)
            + specularTerm * lightColor * FresnelTerm(specColor, lh)
            + surfaceReduction * indirectSpecular * FresnelLerp(specColor, grazingTerm, nv);

        return float4(color, 1.0f);
    }

    float DemoPBRPixelShader::DisneyDiffuse(float NdotV, float NdotL, float LdotH, float perceptualRoughness)
    {
        float fd90 = 0.5f + 2.0f * LdotH * LdotH * (perceptualRoughness * 1.35f); // Darker than unity, temporary add a factor 1.35 for perceptualRoughness.
        // Two schlick fresnel term
        float lightScatter = (1.0f + (fd90 - 1.0f) * Pow5(1.0f - NdotL));
        float viewScatter = (1.0f + (fd90 - 1.0f) * Pow5(1.0f - NdotV));

        return lightScatter * viewScatter;

        // Note: Disney diffuse must be multiply by diffuseAlbedo / PI. This is done outside of this function.
        //return lightScatter * viewScatter * INV_PI;
    }
}
