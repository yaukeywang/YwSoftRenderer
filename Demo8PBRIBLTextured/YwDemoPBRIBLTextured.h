// Add by Yaukey at 2023-06-04.
// YW Soft Renderer demo with PBR-IBL-Textured class.

#ifndef __YW_DEMO_PBR_IBL_TEXTURED_H__
#define __YW_DEMO_PBR_IBL_TEXTURED_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceManager.h"

namespace yw
{
    class Model;
    class DemoPBRIBLCubeMapVertexShader;
    class DemoPBRIBLCubeMapPixelShader;
    class DemoPBRIBLTexturedVertexShader;
    class DemoPBRIBLTexturedPixelShader;

    class DemoPBRIBLTextured : public IEntity
    {
    protected:
        // Constructor.
        DemoPBRIBLTextured(class Scene* scene);

        // Destructor.
        ~DemoPBRIBLTextured();

    public:
        // The instance create function.
        inline static IEntity* Create(class Scene* scene)
        {
            return new DemoPBRIBLTextured(scene);
        }

        // Initial demo.
        bool Initialize();

        // Returns true if the object has been moved -> scenegraph-care.
        bool FrameMove();

        // Render with a specified pass.
        void Render(int32_t pass);

    private:
        // Render all pre-computing data we nee.
        bool LoadAllPreComputingData();

        // Render sky environment.
        void RenderSky(int32_t pass);

        // Render rusted iron.
        void RenderRustedIron(int32_t pass);

        // Render textured pbr model.
        void RenderTexturedPbrModel(
            int32_t pass,
            Model* pbrModel,
            const Vector3& translation,
            const Quaternion& rotation,
            const Vector3& scale,
            Yw3dTexture* albedoMap,
            Yw3dTexture* normalMap,
            Yw3dTexture* metallicMap,
            Yw3dTexture* roughnessMap,
            Yw3dTexture* aoMap,
            Yw3dTextureFilter albedoMapFilter = Yw3d_TF_Linear,
            Yw3dTextureFilter normalMapFilter = Yw3d_TF_Linear,
            Yw3dTextureFilter metallicMapFilter = Yw3d_TF_Linear,
            Yw3dTextureFilter roughnessMapFilter = Yw3d_TF_Linear,
            Yw3dTextureFilter aoMapFilter = Yw3d_TF_Linear,
            float albedoMipLodBias = 0.0f,
            float normalMipLodBias = 0.0f,
            float metallicMipLodBias = 0.0f,
            float roughnessMipLodBias = 0.0f,
            float aoMipLodBias = 0.0f
        );

    private:
        // Model resources.
        HRESOURCE m_ModelSphereHandle;
        HRESOURCE m_ModelCubeHandle;
        HRESOURCE m_ModelPBRHandle;
        Model* m_ModelSphere;
        Model* m_ModelCube;
        Model* m_ModelPBR;

        // Pre-computing texture handle.
        HRESOURCE m_EnvEquirectangularTextureHandle;
        HRESOURCE m_EnvCubeTextureHandle;
        HRESOURCE m_IrrandianceCubeTextureHandle;
        HRESOURCE m_PrefilterReflectionCubeTextureHandle;
        HRESOURCE m_PreintegrateBRDFTextureHandle;

        // Pre-computing texture resources.
        StringA m_EnvEquirectangularTextureName;
        Yw3dTexture* m_EnvEquirectangularTexture;
        Yw3dCubeTexture* m_EnvCubeTexture;
        Yw3dCubeTexture* m_IrrandianceCubeTexture;
        Yw3dCubeTexture* m_PrefilterReflectionCubeTexture;
        Yw3dTexture* m_PreintegrateBRDFTexture;

        // Rusted Iron textured.
        HRESOURCE m_IronAlbedoMapHandle;
        HRESOURCE m_IronNormalMapHandle;
        HRESOURCE m_IronMetallicMapHandle;
        HRESOURCE m_IronRoughnessMapHandle;
        HRESOURCE m_IronAOMapHandle;
        Yw3dTexture* m_IronAlbedoMap;
        Yw3dTexture* m_IronNormalMap;
        Yw3dTexture* m_IronMetallicMap;
        Yw3dTexture* m_IronRoughnessMap;
        Yw3dTexture* m_IronAOMap;

        // Shader related.
        DemoPBRIBLCubeMapVertexShader* m_SkyVertexShader;
        DemoPBRIBLCubeMapPixelShader* m_SkyPixelShader;
        DemoPBRIBLTexturedVertexShader* m_PBRIBLTexturedVertexShader;
        DemoPBRIBLTexturedPixelShader* m_PBRIBLTexturedPixelShader;

        // Enable debug info enabled.
        bool m_DebugInfoEnabled;
    };
}

#endif // !__YW_DEMO_PBR_IBL_TEXTURED_H__
