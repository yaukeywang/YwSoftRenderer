// Add by Yaukey at 2022-05-13.
// YW Soft Renderer demo with PBR-IBL base shader utility class.

#include "YwDemoPBRIBLShaderCommon.h"

namespace yw
{

    // ------------------------------------------------------------------
    // Internal utility functions.

    const float PI = (float)YW_PI;
    const float INV_PI = (float)YW_INV_PI;

    // Clamp value between lower and upper bounds of float3.
    float3 clamp(const float3& value, const float lower, const float upper)
    {
        return float3(yw::Clamp(value.x, lower, upper), yw::Clamp(value.y, lower, upper), yw::Clamp(value.z, lower, upper));
    }

    // Clamp value between lower and upper bounds of float3.
    float3 maximum(const float3& a, const float3& b)
    {
        return float3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
    }

    // Calculate power of float3.
    float3 pow(const float3& v, const float3& p)
    {
        return float3((float)::pow(v.x, p.x), (float)::pow(v.y, p.y), (float)::pow(v.z, p.z));
    }

    // Normalize float3.
    float3 normalize(const float3& v)
    {
        float3 result;
        return Vector3Normalize(result, v);
    }

    // Dot product of float3.
    float dot(const float3& a, const float3& b)
    {
        return Vector3Dot(a, b);
    }

    // Cross product of float3.
    float3 cross(const float3& a, const float3& b)
    {
        float3 result;
        return Vector3Cross(result, a, b);
    }
    
    // ------------------------------------------------------------------
    // PBR and IBL common base library.

    float DemoPBRIBLShaderCommon::RadicalInverseVdC(uint32_t bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return (float)(float(bits) * 2.3283064365386963e-10); // 0x100000000
    }

    float2 DemoPBRIBLShaderCommon::Hammersley(const uint32_t i, const uint32_t N)
    {
        return float2(float(i) / float(N), RadicalInverseVdC(i));
    }

    float3 DemoPBRIBLShaderCommon::ImportanceSampleGGX(const float2& Xi, const float3& N, const float roughness)
    {
        float a = roughness * roughness;

        float phi = 2.0f * (float)YW_PI * Xi.x;
        float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

        // from spherical coordinates to cartesian coordinates - halfway vector
        float3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;

        // from tangent-space H vector to world-space sample vector
        float3 up = abs(N.z) < 0.999f ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
        float3 tangent = normalize(cross(up, N));
        float3 bitangent = cross(N, tangent);

        float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return normalize(sampleVec);
    }

    float DemoPBRIBLShaderCommon::DistributionGGX(const float3& N, const float3& H, const float roughness)
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
    
    float DemoPBRIBLShaderCommon::GeometrySchlickGGX(float NdotV, float roughness)
    {
        // note that we use a different k for IBL
        float a = roughness;
        float k = (a * a) / 2.0f;

        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;

        return nom / denom;
    }
    
    float DemoPBRIBLShaderCommon::GeometrySmith(const float3& N, const float3& V, const float3& L, float roughness)
    {
        float NdotV = max(Vector3Dot(N, V), 0.0f);
        float NdotL = max(Vector3Dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    }

    float3 DemoPBRIBLShaderCommon::FresnelSchlick(float cosTheta, float3 F0)
    {
        return F0 + (float3(1.0f) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }

    float3 DemoPBRIBLShaderCommon::FresnelSchlickRoughness(float cosTheta, const float3& F0, float roughness)
    {
        const float smoothness = 1.0f - roughness;
        return F0 + (maximum(float3(smoothness), F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }
}
