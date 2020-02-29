// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR shader class.

#include "YwDemoPBRShader.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR vertex shader.

    void DemoPBRVertexShader::Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
    {
        // The projection vertex position.
        position = vsShaderInput[0] * (*GetWVPMatrix());

        // Model normal.
        //vsShaderOutput[0] = vsShaderInput[1];

        // Get light and view direction.
        const Vector4& lightDir = GetVector(0);
        const Vector4& viewDir = GetVector(1);

        // Get world inverse matrix.
        const Matrix44& worldInverse = GetMatrix(0);

        // Transform light and view direction into model space.
        Vector4 modelLightDir = lightDir * worldInverse;
        Vector4 modelViewDir = viewDir * worldInverse;

        // Other vertex attribute.
        vsShaderOutput[0] = -modelLightDir;
        vsShaderOutput[1] = -modelViewDir;

        // Other vertex attribute.
        vsShaderOutput[2] = vsShaderInput[4];
        vsShaderOutput[3] = vsShaderInput[5];

        const Vector3 normal = vsShaderInput[1];
        const Vector4 tangent = vsShaderInput[2];
        const Vector3 binormal = cross(normal, Vector3(tangent.x, tangent.y, tangent.z)) * tangent.w;
        vsShaderOutput[4] = Vector3(tangent);
        vsShaderOutput[5] = binormal;
        vsShaderOutput[6] = normal;
    }

    Yw3dShaderRegisterType DemoPBRVertexShader::GetOutputRegisters(uint32_t shaderRegister)
    {
        switch (shaderRegister)
        {
        case 0:
            return Yw3d_SRT_Vector3;    // Vertex normal.
        case 1:
            return Yw3d_SRT_Vector4;    // Vertex tangent.
        case 2:
            return Yw3d_SRT_Vector4;    // Vertex color.
        case 3:
            return Yw3d_SRT_Vector2;    // Vertex texcoord.
        case 4:
            return Yw3d_SRT_Vector2;    // Vertex texcoord2.
        case 5:
            return Yw3d_SRT_Vector3;
        case 6:
            return Yw3d_SRT_Vector3;
        case 7:
            return Yw3d_SRT_Vector3;
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
        const Vector3 tangent = Vector3(input[4]).Normalize();
        const Vector3 binormal = Vector3(input[5]).Normalize();
        const Vector3 normal = Vector3(input[6]).Normalize();
        Matrix33 TBN = Matrix33(
            tangent.x, tangent.y, tangent.z,
            binormal.x, binormal.y, binormal.z,
            normal.x, normal.y, normal.z
        );

        // Sample main texture.
        Vector4 vDdx, vDdy;
        GetPartialDerivatives(2, vDdx, vDdy);
        Vector2 texCoord = input[2];
        Vector4 texColor;
        SampleTexture(texColor, 0, texCoord.x, texCoord.y, 0.0f, &vDdx, &vDdy);

        // Sample normal texture.
        Vector4 normalTexColor;
        SampleTexture(normalTexColor, 1, texCoord.x, texCoord.y, 0.0f, &vDdx, &vDdy);

        Vector3 normalTangent = normalTexColor * 2 - Vector4(1.0f, 1.0f, 1.0f, 0.0f);
        Vector3 normalModel = normalize(normalTangent * TBN);

        Vector3 modelLightDir = Vector3(input[0]).Normalize();
        Vector3 modelViewDir = Vector3(input[1]).Normalize();

        // Get l/v/h vectors.
        Vector3 halfV = normalize(modelLightDir + modelViewDir);
        float NdotL = saturate(dot(normalModel, modelLightDir));
        float NdotH = saturate(dot(normalModel, halfV));
        float NdotV = saturate(dot(normalModel, modelViewDir));
        float VdotH = saturate(dot(modelViewDir, halfV));
        float LdotH = saturate(dot(modelLightDir, halfV));

        // Get parameters for lighting and shading.
        Vector4 lightColor = GetVector(0);
        Vector4 albedo = GetVector(1);
        Vector4 albedoColor = albedo * texColor;
        Vector4 specularColor = GetVector(2);
        float roughness = GetFloat(0);
        float subsurface = GetFloat(1);

        // BRDFs
        Vector3 diffuse = DisneyDiffuse(albedoColor, NdotL, NdotV, LdotH, roughness, subsurface);
        Vector3 specular = CookTorranceSpecular(NdotL, LdotH, NdotH, NdotV, roughness, specularColor.r);

        // Adding diffuse, specular and tins (light, specular).
        Vector3 firstLayer = diffuse * lightColor + specular * specularColor * lightColor;
        Vector4 c = firstLayer;
        c.a = 1.0f;

        color = c;
        return true;
    }

    Vector3 DemoPBRPixelShader::DisneyDiffuse(Vector3 albedo, float NdotL, float NdotV, float LdotH, float roughness, float subsurface)
    {
        // luminance approximation
        float albedoLuminosity = 0.3f * albedo.r + 0.6f * albedo.g + 0.1f * albedo.b;
        
        // normalize luminosity to isolate hue and saturation
        Vector3 albedoTint = albedoLuminosity > 0.0f ? albedo / albedoLuminosity : Vector3(1.0f, 1.0f, 1.0f);
        float fresnelL = SchlickFresnel(NdotL);
        float fresnelV = SchlickFresnel(NdotV);
        float fresnelDiffuse = 0.5f + 2.0f * sqr(LdotH) * roughness;
        Vector3 diffuse = albedoTint * Lerp(1.0f, fresnelDiffuse, fresnelL) * Lerp(1.0, fresnelDiffuse, fresnelV);
        float fresnelSubsurface90 = sqr(LdotH) * roughness;
        float fresnelSubsurface = Lerp(1.0, fresnelSubsurface90, fresnelL) * Lerp(1.0, fresnelSubsurface90, fresnelV);
        float ss = 1.25f * (fresnelSubsurface * (1.0f / (NdotL + NdotV) - 0.5f) + 0.5f);
        return saturate(lerp(diffuse, Vector3(ss, ss, ss), subsurface) * (1 / PI) * albedo);
    }

    Vector3 DemoPBRPixelShader::CookTorranceSpecular(float NdotL, float LdotH, float NdotH, float NdotV, float roughness, float specularColor)
    {
        const float F0 = specularColor;
        const float alpha = sqr(roughness);

        // D
        float alphaSqr = sqr(alpha);
        float denom = sqr(NdotH) * (alphaSqr - 1.0f) + 1.0f;
        float D = alphaSqr / (PI * sqr(denom));

        // F
        float LdotH5 = SchlickFresnel(LdotH);
        float F = F0 + (1.0f - F0) * LdotH5;

        // G
        float r = roughness + 1;
        float k = sqr(r) / 8;
        float g1L = G1(k, NdotL);
        float g1V = G1(k, NdotV);
        float G = g1L * g1V;

        float specular = (D * F * G) / (4.0f * NdotL * NdotV + 1e-4f);
        return Vector3(specular, specular, specular);
    }

    float DemoPBRPixelShader::sqr(float value)
    {
        return value * value;
    }

    float DemoPBRPixelShader::SchlickFresnel(float value)
    {
        float m = clamp(1 - value, 0.0f, 1.0f);
        return pow(m, 5);
    }

    float DemoPBRPixelShader::G1(float k, float x)
    {
        return x / (x * (1 - k) + k);
    }
}
