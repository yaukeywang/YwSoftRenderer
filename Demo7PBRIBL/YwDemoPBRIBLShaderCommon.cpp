// Add by Yaukey at 2022-05-13.
// YW Soft Renderer demo with PBR-IBL base shader utility class.

#include "YwDemoPBRIBLShaderCommon.h"

namespace yw
{

    // ------------------------------------------------------------------
    // Internal utility functions.

    const float PI = (float)YW_PI;
    const float INV_PI = (float)YW_INV_PI;

    // Clamp value between lower and upper bounds of Vector3.
    Vector3 clamp(const Vector3& value, const float lower, const float upper)
    {
        return Vector3(yw::Clamp(value.x, lower, upper), yw::Clamp(value.y, lower, upper), yw::Clamp(value.z, lower, upper));
    }

    // Clamp value between lower and upper bounds of Vector3.
    Vector3 maximum(const Vector3& a, const Vector3& b)
    {
        return Vector3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
    }

    // Calculate power of Vector3.
    Vector3 pow(const Vector3& v, const Vector3& p)
    {
        return Vector3((float)::pow(v.x, p.x), (float)::pow(v.y, p.y), (float)::pow(v.z, p.z));
    }

    // Normalize Vector3.
    Vector3 normalize(const Vector3& v)
    {
        Vector3 result;
        return Vector3Normalize(result, v);
    }

    // Dot product of Vector3.
    float dot(const Vector3& a, const Vector3& b)
    {
        return Vector3Dot(a, b);
    }

    // Cross product of Vector3.
    Vector3 cross(const Vector3& a, const Vector3& b)
    {
        Vector3 result;
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

    Vector2 DemoPBRIBLShaderCommon::Hammersley(const uint32_t i, const uint32_t N)
    {
        return Vector2(float(i) / float(N), RadicalInverseVdC(i));
    }

    Vector3 DemoPBRIBLShaderCommon::ImportanceSampleGGX(const Vector2& Xi, const Vector3& N, const float roughness)
    {
        float a = roughness * roughness;

        float phi = 2.0f * (float)YW_PI * Xi.x;
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

    float DemoPBRIBLShaderCommon::DistributionGGX(const Vector3& N, const Vector3& H, const float roughness)
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
    
    float DemoPBRIBLShaderCommon::GeometrySmith(const Vector3& N, const Vector3& V, const Vector3& L, float roughness)
    {
        float NdotV = max(Vector3Dot(N, V), 0.0f);
        float NdotL = max(Vector3Dot(N, L), 0.0f);
        float ggx2 = GeometrySchlickGGX(NdotV, roughness);
        float ggx1 = GeometrySchlickGGX(NdotL, roughness);

        return ggx1 * ggx2;
    }

    Vector3 DemoPBRIBLShaderCommon::FresnelSchlickRoughness(float cosTheta, const Vector3& F0, float roughness)
    {
        const float smoothness = 1.0f - roughness;
        return F0 + (maximum(Vector3(smoothness), F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
    }
}
