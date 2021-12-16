// Add by Yaukey at 2020-02-24.
// YW Soft Renderer demo with PBR shader class.

#ifndef __YW_DEMO_PBR_SHADER_H__
#define __YW_DEMO_PBR_SHADER_H__

#include "YwGraphics.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Sky shader.

    // Sky vertex shader.
    class DemoPBRSkyVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // Sky pixel shader.
    class DemoPBRSkyPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);
    };

    // ------------------------------------------------------------------
    // PBR shader.

    // ------------------------------------------------------------------

    // PBR vertex shader.
    class DemoPBRVertexShader : public IYw3dVertexShader
    {
    protected:
        // Shader main entry.
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput);

        // Shader stream channel.
        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister);
    };

    // ------------------------------------------------------------------

    // Define default shader mode.
    #define SHADER_TARGET 30

    // We donot use handle correctly negative NdotV.
    #define UNITY_HANDLE_CORRECTLY_NEGATIVE_NDOTV 0

    // We use gamma color space default.
    #define UNITY_COLORSPACE_GAMMA 1

    // Energy conservation for Specular workflow is Monochrome. For instance: Red metal will make diffuse Black not Cyan
    #ifndef UNITY_CONSERVE_ENERGY
        #define UNITY_CONSERVE_ENERGY 1
    #endif

    #ifndef UNITY_CONSERVE_ENERGY_MONOCHROME
        #define UNITY_CONSERVE_ENERGY_MONOCHROME 1
    #endif

    // Color space constants.
    #ifdef UNITY_COLORSPACE_GAMMA
        #define unity_ColorSpaceGrey float4(0.5f, 0.5f, 0.5f, 0.5f)
        #define unity_ColorSpaceDouble float4(2.0f, 2.0f, 2.0f, 2.0f)
        #define unity_ColorSpaceDielectricSpec float4(0.220916301f, 0.220916301f, 0.220916301f, 1.0f - 0.220916301f)
        #define unity_ColorSpaceLuminance float4(0.22f, 0.707f, 0.071f, 0.0) // Legacy: alpha is set to 0.0 to specify gamma mode
    #else // Linear values
        #define unity_ColorSpaceGrey float4(0.214041144f, 0.214041144f, 0.214041144f, 0.5f)
        #define unity_ColorSpaceDouble float4(4.59479380f, 4.59479380f, 4.59479380f, 2.0f)
        #define unity_ColorSpaceDielectricSpec float4(0.04f, 0.04f, 0.04f, 1.0f - 0.04f) // standard dielectric reflectivity coef at incident angle (= 4%)
        #define unity_ColorSpaceLuminance float4(0.0396819152f, 0.458021790f, 0.00609653955f, 1.0f) // Legacy: alpha is set to 1.0 to specify linear mode
    #endif

    // PBR pixel shader.
    class DemoPBRPixelShader : public IYw3dPixelShader
    {
    protected:
        // Whether kill pixel or not.
        bool MightKillPixels();

        // Shader main entry.
        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth);

    protected:
        // Fragment common data used for pbr.
        struct FragmentCommonData
        {
            float3 diffColor;
            float3 specColor;
            // Note: smoothness & oneMinusReflectivity for optimization purposes, mostly for DX9 SM2.0 level.
            // Most of the math is being done on these (1-x) values, and that saves a few precious ALU slots.
            float oneMinusReflectivity;
            float smoothness;
            float alpha;

            FragmentCommonData() : oneMinusReflectivity(0.0f), smoothness(0.0f), alpha(0.0f) {}
        };

        // Using Metallic Texture Alpha as smoothness source.
        // albedoTexture - Metallic Texture parameter.
        // glossMapScale - smoothness parameter.
        inline FragmentCommonData FragmentSetup(const float4& albedoTint, const float4& albedoColor, bool hasMetallicOrSpecularGlossMap, const float4& metallicOrSpecularGlossMap, const float4& specularColor, float metallic, float glossiness, float glossMapScale)
        {
            // Get alpha.
            float alpha = albedoTint.a * albedoColor.a;

            // Get fragment from metallic setup.
            FragmentCommonData o = OnFragmentSetup(albedoTint, albedoColor, hasMetallicOrSpecularGlossMap, metallicOrSpecularGlossMap, specularColor, metallic, glossiness, glossMapScale);

            // NOTE: shader relies on pre-multiply alpha-blend (_SrcBlend = One, _DstBlend = OneMinusSrcAlpha)
            o.diffColor = PreMultiplyAlpha(o.diffColor, alpha, o.oneMinusReflectivity, /*out*/ o.alpha);

            return o;
        }

        inline FragmentCommonData MetallicSetup(const float4& albedoTint, const float4& albedoColor, bool hasMetallicGlossMap, const float4& metallicGlossMap, float metallic, float glossiness, float glossMapScale)
        {
            float2 metallicGloss = MetallicGloss(hasMetallicGlossMap, metallicGlossMap, metallic, glossiness, glossMapScale);
            float metallicFinal = metallicGloss.x;
            float smoothness = metallicGloss.y; // this is 1 minus the square root of real roughness m.

            float oneMinusReflectivity = 0.5f;
            float3 specColor;
            float3 diffColor = DiffuseAndSpecularFromMetallic(Albedo(albedoTint, albedoColor), metallicFinal, /*out*/ specColor, /*out*/ oneMinusReflectivity);

            FragmentCommonData o = FragmentCommonData();
            o.diffColor = diffColor;
            o.specColor = specColor;
            o.oneMinusReflectivity = oneMinusReflectivity;
            o.smoothness = smoothness;
            return o;
        }

        inline FragmentCommonData SpecularSetup(const float4& albedoTint, const float4& albedoColor, bool hasSpecularGlossMap, const float4& specularGlossMap, const float4& specularColor, float metallic, float glossiness, float glossMapScale)
        {
            half4 specGloss = SpecularGloss(hasSpecularGlossMap, specularGlossMap, specularColor, metallic, glossiness, glossMapScale);
            half3 specColor = specGloss;
            half smoothness = specGloss.a;

            half oneMinusReflectivity;
            half3 diffColor = EnergyConservationBetweenDiffuseAndSpecular(Albedo(albedoTint, albedoColor), specColor, /*out*/ oneMinusReflectivity);

            FragmentCommonData o = FragmentCommonData();
            o.diffColor = diffColor;
            o.specColor = specColor;
            o.oneMinusReflectivity = oneMinusReflectivity;
            o.smoothness = smoothness;
            return o;
        }

        inline float2 MetallicGloss(bool hasMetallicGlossMap, const float4& metallicGlossMap, float metallic, float glossiness, float glossMapScale)
        {
            float2 mg;
            if (hasMetallicGlossMap)
            {
                mg = float2(metallicGlossMap.r, metallicGlossMap.a);
                mg.g *= glossMapScale;
            }
            else
            {
                mg.r = metallic;
                mg.g = glossiness;
            }

            return mg;
        }

        inline half4 SpecularGloss(bool hasSpecularGlossMap, const float4& specularGlossMap, const float4& specularColor, float metallic, float glossiness, float glossMapScale)
        {
            half4 sg;
            if (hasSpecularGlossMap)
            {
                sg = specularGlossMap;
                sg.a *= glossMapScale;
            }
            else {
                sg = specularColor;
                sg.a = glossiness;
            }

            return sg;
        }

        inline float3 DiffuseAndSpecularFromMetallic(const float3& albedo, float metallic, /*out*/ float3& specColor, /*out*/ float& oneMinusReflectivity)
        {
            specColor = lerp((float3)unity_ColorSpaceDielectricSpec, albedo, metallic);
            oneMinusReflectivity = OneMinusReflectivityFromMetallic(metallic);
            return albedo * oneMinusReflectivity;
        }

        // Diffuse/Spec Energy conservation
        inline half3 EnergyConservationBetweenDiffuseAndSpecular(const half3& albedo, const half3& specColor, /*out*/ half& oneMinusReflectivity)
        {
            oneMinusReflectivity = 1 - SpecularStrength(specColor);
        #if !UNITY_CONSERVE_ENERGY
            return albedo;
        #elif UNITY_CONSERVE_ENERGY_MONOCHROME
            return albedo * oneMinusReflectivity;
        #else
            return albedo * (half3(1, 1, 1) - specColor);
        #endif
        }

        inline float OneMinusReflectivityFromMetallic(float metallic)
        {
            // We'll need oneMinusReflectivity, so
            //   1-reflectivity = 1-lerp(dielectricSpec, 1, metallic) = lerp(1-dielectricSpec, 0, metallic)
            // store (1-dielectricSpec) in unity_ColorSpaceDielectricSpec.a, then
            //   1-reflectivity = lerp(alpha, 0, metallic) = alpha + metallic*(0 - alpha) =
            //                  = alpha - metallic * alpha
            float oneMinusDielectricSpec = unity_ColorSpaceDielectricSpec.a;
            return oneMinusDielectricSpec - metallic * oneMinusDielectricSpec;
        }

        inline half SpecularStrength(const half3& specular)
        {
        #if (SHADER_TARGET < 30)
            // SM2.0: instruction count limitation
            // SM2.0: simplified SpecularStrength
            return specular.r; // Red channel - because most metals are either monocrhome or with redish/yellowish tint
        #else
            return max(max(specular.r, specular.g), specular.b);
        #endif
        }

        inline float3 Albedo(const float4& albedoTint, const float4& albedoColor)
        {
            //float3 albedo = (float3)albedoTint * (float3)albedoColor;
            float3 albedo = float3(albedoTint.r * albedoColor.r, albedoTint.g * albedoColor.g, albedoTint.b * albedoColor.b);
            return albedo;
        }

        inline float3 PreMultiplyAlpha(const float3& diffColor, float alpha, float oneMinusReflectivity, /*out*/ float outModifiedAlpha)
        {
        #if defined(_ALPHAPREMULTIPLY_ON)
            // NOTE: shader relies on pre-multiply alpha-blend (_SrcBlend = One, _DstBlend = OneMinusSrcAlpha)

            // Transparency 'removes' from Diffuse component
            diffColor *= alpha;

            #if (SHADER_TARGET < 30)
                // SM2.0: instruction count limitation
                // Instead will sacrifice part of physically based transparency where amount Reflectivity is affecting Transparency
                // SM2.0: uses unmodified alpha
                outModifiedAlpha = alpha;
            #else
                // Reflectivity 'removes' from the rest of components, including Transparency
                // outAlpha = 1-(1-alpha)*(1-reflectivity) = 1-(oneMinusReflectivity - alpha*oneMinusReflectivity) =
                //          = 1-oneMinusReflectivity + alpha*oneMinusReflectivity
                outModifiedAlpha = 1 - oneMinusReflectivity + alpha * oneMinusReflectivity;
            #endif
        #else
            outModifiedAlpha = alpha;
        #endif
            return diffColor;
        }

        //-------------------------------------------------------------------------------------
        // Unity Standard BRDF.
        // Note: BRDF entry points use smoothness and oneMinusReflectivity for optimization
        // purposes, mostly for DX9 SM2.0 level. Most of the math is being done on these (1-x) values, and that saves
        // a few precious ALU slots.

        // Main Physically Based BRDF
        // Derived from Disney work and based on Torrance-Sparrow micro-facet model
        //
        //   BRDF = kD / pi + kS * (D * V * F) / 4
        //   I = BRDF * NdotL
        //
        // * GGX for NDF
        // * Smith for Visiblity term
        // * Schlick approximation for Fresnel
        float4 UNITY_BRDF_PBS(const float3& diffColor, const float3& specColor, const float3& lightColor, float oneMinusReflectivity, float smoothness, const float3& normal, const float3& viewDir, const float3& lightDir);

        // Note: Disney diffuse must be multiply by diffuseAlbedo / PI. This is done outside of this function.
        float DisneyDiffuse(float NdotV, float NdotL, float LdotH, float perceptualRoughness);

        // Ref: http://jcgt.org/published/0003/02/03/paper.pdf
        inline float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
        {
        #if 0
            // Original formulation:
            //  lambda_v    = (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
            //  lambda_l    = (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
            //  G           = 1 / (1 + lambda_v + lambda_l);

            // Reorder code to be more optimal
            half a = roughness;
            half a2 = a * a;

            half lambdaV = NdotL * sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
            half lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

            // Simplify visibility term: (2.0f * NdotL * NdotV) /  ((4.0f * NdotL * NdotV) * (lambda_v + lambda_l + 1e-5f));
            return 0.5f / (lambdaV + lambdaL + 1e-5f);  // This function is not intended to be running on Mobile,
                                                        // therefore epsilon is smaller than can be represented by half
        #else
            // Approximation of the above formulation (simplify the sqrt, not mathematically correct but close enough)
            float a = roughness;
            float lambdaV = NdotL * (NdotV * (1 - a) + a);
            float lambdaL = NdotV * (NdotL * (1 - a) + a);

            #if defined(SHADER_API_SWITCH)
                return 0.5f / (lambdaV + lambdaL + 1e-4f); // work-around against hlslcc rounding error
            #else
                return 0.5f / (lambdaV + lambdaL + 1e-5f);
            #endif

        #endif
        }

        inline float GGXTerm(float NdotH, float roughness)
        {
            float a2 = roughness * roughness;
            float d = (NdotH * a2 - NdotH) * NdotH + 1.0f; // 2 mad
            return INV_PI * a2 / (d * d + 1e-7f); // This function is not intended to be running on Mobile,
                                                  // therefore epsilon is smaller than what can be represented by half
        }

        //-----------------------------------------------------------------------------
        // Helper to convert smoothness to roughness
        //-----------------------------------------------------------------------------

        float PerceptualRoughnessToRoughness(float perceptualRoughness)
        {
            return perceptualRoughness * perceptualRoughness;
        }

        float RoughnessToPerceptualRoughness(float roughness)
        {
            return sqrt(roughness);
        }

        // Smoothness is the user facing name
        // it should be perceptualSmoothness but we don't want the user to have to deal with this name
        float SmoothnessToRoughness(float smoothness)
        {
            return (1.0f - smoothness) * (1.0f - smoothness);
        }

        float SmoothnessToPerceptualRoughness(float smoothness)
        {
            return (1.0f - smoothness);
        }

        //-----------------------------------------------------------------------------
        // Helper for fresnel calculation.

        inline float3 FresnelTerm(const float3& F0, float cosA)
        {
            float t = Pow5(1.0f - cosA);   // ala Schlick interpoliation
            return F0 + (float3(1.0f) - F0) * t;
        }

        inline float3 FresnelLerp(const float3& F0, const float3& F90, float cosA)
        {
            float t = Pow5(1.0f - cosA);   // ala Schlick interpoliation
            return lerp(F0, F90, t);
        }

        // approximage Schlick with ^4 instead of ^5
        inline float3 FresnelLerpFast(const float3& F0, const float3& F90, float cosA)
        {
            float t = Pow4(1 - cosA);
            return lerp(F0, F90, t);
        }

        //-------------------------------------------------------------------------------------

        inline float Pow4(float x)
        {
            return x * x * x * x;
        }

        inline float2 Pow4(const float2& x)
        {
            //return x * x * x * x;
            return float2(
                x.x * x.x * x.x * x.x,
                x.y * x.y * x.y * x.y
            );
        }

        inline float3 Pow4(const float3& x)
        {
            //return x * x * x * x;
            return float3(
                x.x * x.x * x.x * x.x,
                x.y * x.y * x.y * x.y,
                x.z * x.z * x.z * x.z
            );
        }

        inline float4 Pow4(const float4& x)
        {
            //return x * x * x * x;
            return float4(
                x.x * x.x * x.x * x.x,
                x.y * x.y * x.y * x.y,
                x.z * x.z * x.z * x.z,
                x.w * x.w * x.w * x.w
            );
        }

        //-----------------------------------------------------------------------------
        // Pow5 uses the same amount of instructions as generic pow(), but has 2 advantages:
        // 1) better instruction pipelining
        // 2) no need to worry about NaNs
        inline float Pow5(float x)
        {
            return x * x * x * x * x;
        }

        inline float2 Pow5(const float2& x)
        {
            //return x * x * x * x * x;
            return float2(
                x.x * x.x * x.x * x.x * x.x,
                x.y * x.y * x.y * x.y * x.y
            );
        }

        inline float3 Pow5(const float3& x)
        {
            //return x * x * x * x * x;
            return float3(
                x.x * x.x * x.x * x.x * x.x,
                x.y * x.y * x.y * x.y * x.y,
                x.z * x.z * x.z * x.z * x.z
            );
        }

        inline float4 Pow5(const float4& x)
        {
            //return x * x * x * x * x;
            return float4(
                x.x * x.x * x.x * x.x * x.x,
                x.y * x.y * x.y * x.y * x.y,
                x.z * x.z * x.z * x.z * x.z,
                x.w * x.w * x.w * x.w * x.w
            );
        }

        //-------------------------------------------------------------------------------------
        /*
        // https://s3.amazonaws.com/docs.knaldtech.com/knald/1.0.0/lys_power_drops.html

        const float k0 = 0.00098, k1 = 0.9921;
        // pass this as a constant for optimization
        const float fUserMaxSPow = 100000; // sqrt(12M)
        const float g_fMaxT = ( exp2(-10.0/fUserMaxSPow) - k0)/k1;
        float GetSpecPowToMip(float fSpecPow, int nMips)
        {
           // Default curve - Inverse of TB2 curve with adjusted constants
           float fSmulMaxT = ( exp2(-10.0/sqrt( fSpecPow )) - k0)/k1;
           return float(nMips-1)*(1.0 - clamp( fSmulMaxT/g_fMaxT, 0.0, 1.0 ));
        }

            //float specPower = PerceptualRoughnessToSpecPower(perceptualRoughness);
            //float mip = GetSpecPowToMip (specPower, 7);
        */
        inline float3 Unity_SafeNormalize(const float3& inVec)
        {
            float dp3 = max(0.001f, dot(inVec, inVec));
            return inVec / sqrt(dp3);
        }

        // Unpack a scaled normal from normal map pixel.
        inline float3 UnpackScaleNormal(const float4& packedNormal, float bumpScale)
        {
            float3 normal = UnpackNormal(packedNormal);
            normal.x *= bumpScale;
            normal.y *= bumpScale;

            return normal;
        }

        // Unpack a normal from normal map pixel.
        inline float3 UnpackNormal(const float4& packedNormal)
        {
            return packedNormal * 2.0f - float4(1.0f);
        }

    protected:
        virtual FragmentCommonData OnFragmentSetup(const float4& albedoTint, const float4& albedoColor, bool hasMetallicOrSpecularGlossMap, const float4& metallicOrSpecularGlossMap, const float4& specularColor, float metallic, float glossiness, float glossMapScale) = 0;
    };

    // PBR Metallic Setup pixel shader.
    class DemoPBRMetallicSetupPixelShader : public DemoPBRPixelShader
    {
    protected:
        virtual inline FragmentCommonData OnFragmentSetup(const float4& albedoTint, const float4& albedoColor, bool hasMetallicOrSpecularGlossMap, const float4& metallicOrSpecularGlossMap, const float4& specularColor, float metallic, float glossiness, float glossMapScale)
        {
            return MetallicSetup(albedoTint, albedoColor, hasMetallicOrSpecularGlossMap, metallicOrSpecularGlossMap, metallic, glossiness, glossMapScale);
        }
    };

    // PBR Specular Setup pixel shader.
    class DemoPBRSpecularSetupPixelShader : public DemoPBRPixelShader
    {
    protected:
        virtual inline FragmentCommonData OnFragmentSetup(const float4& albedoTint, const float4& albedoColor, bool hasMetallicOrSpecularGlossMap, const float4& metallicOrSpecularGlossMap, const float4& specularColor, float metallic, float glossiness, float glossMapScale)
        {
            return SpecularSetup(albedoTint, albedoColor, hasMetallicOrSpecularGlossMap, metallicOrSpecularGlossMap, specularColor, metallic, glossiness, glossMapScale);
        }
    };
}

#endif //!__YW_DEMO_PBR_SHADER_H__
