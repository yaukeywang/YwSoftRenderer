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
        // Render sky environment.
        void RenderSky(int32_t pass);

        // Render pbr model.
        void RenderPbrModel(int32_t pass);

    private:
        // Model resources.
        class Model* m_ModelSkySphere;
        class Model* m_ModelPBR;

        // Texture resources.
        class Yw3dCubeTexture* m_ModelSkySphereTexture;
        class Yw3dTexture* m_ModelPBRTexture;
        class Yw3dTexture* m_ModelPBRNormalTexture;
        class Yw3dTexture* m_ModelPBRSpecularTexture;

        // Model Resource handle.
        HRESOURCE m_ModelSkySphereHandle;
        HRESOURCE m_ModelPBRHandle;

        // Texture Resource handle.
        HRESOURCE m_ModelSkySphereTextureHandle;
        HRESOURCE m_ModelPBRTextureHandle;
        HRESOURCE m_ModelPBRNormalTextureHandle;
        HRESOURCE m_ModelPBRSpecularTextureHandle;

        // Shader related.
        class DemoPBRIBLSkyVertexShader* m_SkyVertexShader;
        class DemoPBRIBLSkyPixelShader* m_SkyPixelShader;
        class DemoPBRIBLVertexShader* m_PbrVertexShader;
        class DemoPBRIBLPixelShader* m_PbrPixelShader;

        // Lighting params.
        Vector3 m_LightDirection;
        Vector4 m_LightColor;
        Vector4 m_AlbedoColor;
        Vector4 m_SpecularColor;

        // _Metallic/"Metallic".
        float m_Metallic;

        // _Glossiness/"Smoothness".
        float m_Smoothness;

        // _GlossMapScale/"Smoothness Scale".
        float m_SmoothnessScale;
    };
}

#endif // !__YW_DEMO_PBR_IBL_H__
