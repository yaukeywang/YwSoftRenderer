// Add by Yaukey at 2020-04-18.
// YW Soft Renderer demo with PBR-IBL class.

#ifndef __YW_DEMO_PBR_IBL_H__
#define __YW_DEMO_PBR_IBL_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class DemoPBRIBL : public IEntity
    {
    protected:
        // Constructor.
        DemoPBRIBL(class Scene* scene);

        // Destructor.
        ~DemoPBRIBL();

    public:
        // The instance create function.
        inline static IEntity* Create(class Scene* scene)
        {
            return new DemoPBRIBL(scene);
        }

        // Initial demo.
        bool Initialize();

        // Returns true if the object has been moved -> scenegraph-care.
        bool FrameMove();

        // Render with a specified pass.
        void Render(int32_t pass);

    private:
        // Convert hdr equirectangular map to hdr cube map.
        bool RenderEquirectangularMapToCubeMap();

        // Convert hdr cube map to hdr irrandiance map.
        bool RenderCubeMapToIrradianceMap();

        // Render prefilter hdr reflection map.
        bool RenderPrefilterReflectionMap();

        // Render preintegrate brdf lut map.
        bool RenderPreintegrateBRDFMap();

        // Render sky environment.
        void RenderSky(int32_t pass);

        // Render pbr model.
        void RenderPbrModel(int32_t pass);

    private:
        // Has rendered cube map from equirectangular map or not, only need once.
        bool m_RenderedCubeMap;

        // Model resources.
        class Model* m_ModelSkySphere;
        //class Model* m_ModelPBR;

        // Texture resources.
        Yw3dTexture* m_EnvEquirectangularTexture;
        Yw3dCubeTexture* m_EnvCubeTexture;
        Yw3dCubeTexture* m_IrrandianceCubeTexture;
        Yw3dCubeTexture* m_PrefilterReflectionCubeTexture;
        Yw3dCubeTexture* m_PreintegrateBRDFTexture;
        //class Yw3dTexture* m_ModelPBRTexture;
        //class Yw3dTexture* m_ModelPBRNormalTexture;
        //class Yw3dTexture* m_ModelPBRSpecularTexture;

        // Model Resource handle.
        HRESOURCE m_ModelSkySphereHandle;
        //HRESOURCE m_ModelPBRHandle;

        // Texture Resource handle.
        HRESOURCE m_ModelSkySphereTextureHandle;
        //HRESOURCE m_ModelPBRTextureHandle;
        //HRESOURCE m_ModelPBRNormalTextureHandle;
        //HRESOURCE m_ModelPBRSpecularTextureHandle;

        // Shader related.
        class DemoPBRIBLCubeMapVertexShader* m_SkyVertexShader;
        class DemoPBRIBLCubeMapPixelShader* m_SkyPixelShader;
        //class DemoPBRIBLVertexShader* m_PbrVertexShader;
        //class DemoPBRIBLPixelShader* m_PbrPixelShader;

        //// Lighting params.
        //Vector3 m_LightDirection;
        //Vector4 m_LightColor;
        //Vector4 m_AlbedoColor;
        //Vector4 m_SpecularColor;

        //// _Metallic/"Metallic".
        //float m_Metallic;

        //// _Glossiness/"Smoothness".
        //float m_Smoothness;

        //// _GlossMapScale/"Smoothness Scale".
        //float m_SmoothnessScale;
    };
}

#endif // !__YW_DEMO_PBR_IBL_H__
