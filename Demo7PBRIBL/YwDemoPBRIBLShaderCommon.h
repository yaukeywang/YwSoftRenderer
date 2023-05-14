// Add by Yaukey at 2022-05-13.
// YW Soft Renderer demo with PBR-IBL base shader utility class.

#ifndef __YW_DEMO_PBR_IBL_SHADER_COMMON_H__
#define __YW_DEMO_PBR_IBL_SHADER_COMMON_H__

#include "Yw3d.h"

namespace yw
{
    // PBR-IBL common shader utilities.
    class DemoPBRIBLShaderCommon
    {
    public:
        DemoPBRIBLShaderCommon() {}
        ~DemoPBRIBLShaderCommon() {}
     
    public:
        // Unpack a normal from normal map pixel.
        inline float3 UnpackNormal(const float4& packedNormal)
        {
            return packedNormal * 2.0f - float4(1.0f);
        }
     
        /*
         * Unpack a scaled normal from normal map pixel.
         */
        inline float3 UnpackScaleNormal(const float4& packedNormal, float bumpScale)
        {
            float3 normal = UnpackNormal(packedNormal);
            normal.x *= bumpScale;
            normal.y *= bumpScale;

            return normal;
        }
     
        /**
         * http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
         * Efficient VanDerCorpus calculation.
         */
        float RadicalInverseVdC(uint32_t bits);

        /**
         * Calculating "Hammersley" point set.
         */
        Vector2 Hammersley(const uint32_t i, const uint32_t N);
        
        /**
         * Calculating "GGX" distribution.
         */
        float DistributionGGX(const Vector3& N, const Vector3& H, const float roughness);

        /**
         * Importance samplng GGX value.
         */
        Vector3 ImportanceSampleGGX(const Vector2& Xi, const Vector3& N, const float roughness);

        /**
         * Geometry Schlick GGX function.
         */
        float GeometrySchlickGGX(float NdotV, float roughness);

        /**
         * Geometry Smith function.
         */
        float GeometrySmith(const Vector3& N, const Vector3& V, const Vector3& L, float roughness);

        /**
         * Calculating Fresnel Schlick value.
         */
        Vector3 FresnelSchlick(float cosTheta, Vector3 F0);
     
        /**
         * Calculating Fresnel Schlick value by roughness.
         */
        Vector3 FresnelSchlickRoughness(float cosTheta, const Vector3& F0, float roughness);
    };
}

#endif //!__YW_DEMO_PBR_IBL_SHADER_COMMON_H__
