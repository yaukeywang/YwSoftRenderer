// Add by Yaukey at 2023-06-04.
// YW Soft Renderer demo with PBR-IBL-Textured class.

#ifndef __YW_DEMO_PBR_IBL_TEXTURED_H__
#define __YW_DEMO_PBR_IBL_TEXTURED_H__

#include "Yw3d.h"
#include "YwEntity.h"
#include "YwResourceHelper.h"

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
        // Load resources for rusted iron.
        bool LoadRustedIronResources();

        // Load resources for aluminium insulator.
        bool LoadAluminiumInsulatorResources();

        // Render rusted iron.
        void RenderRustedIron(int32_t pass);

        // Render demo AluminiumInsulator in SubstancePainter
        void RenderAluminiumInsulator(int32_t pass);

    private:
        // Render all pre-computing data we nee.
        bool LoadAllPreComputingData();

        // Render sky environment.
        void RenderSky(int32_t pass);

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
        // Enable debug info enabled.
        bool m_DebugInfoEnabled;

        // Model resources.
        ResourceWrapper* m_ModelSphereResource;
        ResourceWrapper* m_ModelCubeResource;
        ResourceWrapper* m_ModelPBRResource;

        // Pre-computing resources.
        ResourceWrapper* m_EnvEquirectangularTextureResource;
        ResourceWrapper* m_EnvCubeTextureResource;
        ResourceWrapper* m_IrrandianceCubeTextureResource;
        ResourceWrapper* m_PrefilterReflectionCubeTextureResource;
        ResourceWrapper* m_PreintegrateBRDFTextureResource;
        StringA m_EnvEquirectangularTextureName;

        // Shader related.
        DemoPBRIBLCubeMapVertexShader* m_SkyVertexShader;
        DemoPBRIBLCubeMapPixelShader* m_SkyPixelShader;
        DemoPBRIBLTexturedVertexShader* m_PBRIBLTexturedVertexShader;
        DemoPBRIBLTexturedPixelShader* m_PBRIBLTexturedPixelShader;

        // Rusted iron assets.
        ResourceWrapper* m_RustedIronAlbedoMapResource;
        ResourceWrapper* m_RustedIronNormalMapResource;
        ResourceWrapper* m_RustedIronMetallicMapResource;
        ResourceWrapper* m_RustedIronRoughnessMapResource;
        ResourceWrapper* m_RustedIronAOMapResource;

        // Aluminium insulator assets.
        ResourceWrapper* m_AluminiumInsulatorAlbedoMapResource;
        ResourceWrapper* m_AluminiumInsulatorNormalMapResource;
        ResourceWrapper* m_AluminiumInsulatorMetallicMapResource;
        ResourceWrapper* m_AluminiumInsulatorRoughnessMapResource;
        ResourceWrapper* m_AluminiumInsulatorAOMapResource;
    };
}

#endif // !__YW_DEMO_PBR_IBL_TEXTURED_H__
