// Add by Yaukey at 2022-06-04.
// YW Soft Renderer demo with PBR-IBL-Textured class.

#include "YwDemoPBRIBLTextured.h"
#include "YwDemoPBRIBLTexturedApp.h"
#include "YwDemoPBRIBLTexturedCamera.h"
#include "YwDemoPBRIBLTexturedShader.h"
#include "YwDemoPBRIBLShaderSky.h"
#include "YwDemoPBRIBLPrecompute.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"
#include "YwTextureDataConverter.h"
#include "YwFileIO.h"

#ifndef YW_SAFE_UNLOAD_RESOURCE
#define YW_SAFE_UNLOAD_RESOURCE(m, r) {if ((r) > 0) {(m)->UnloadResource((r)); (r) = 0;}}
#endif

namespace yw
{
    // ------------------------------------------------------------------
    // PBR entity.

    DemoPBRIBLTextured::DemoPBRIBLTextured(Scene* scene) :
        IEntity(scene),

        m_ModelSphereResource(nullptr),
        m_ModelCubeResource(nullptr),
        m_ModelPBRResource(nullptr),

        m_EnvEquirectangularTextureResource(nullptr),
        m_EnvCubeTextureResource(nullptr),
        m_IrrandianceCubeTextureResource(nullptr),
        m_PrefilterReflectionCubeTextureResource(nullptr),
        m_PreintegrateBRDFTextureResource(nullptr),

        m_SkyVertexShader(nullptr),
        m_SkyPixelShader(nullptr),
        m_PBRIBLTexturedVertexShader(nullptr),
        m_PBRIBLTexturedPixelShader(nullptr),

        // Rusted iron.
        m_RustedIronAlbedoMapResource(nullptr),
        m_RustedIronNormalMapResource(nullptr),
        m_RustedIronMetallicMapResource(nullptr),
        m_RustedIronRoughnessMapResource(nullptr),
        m_RustedIronAOMapResource(nullptr),

        // Aluminium insulator.
        m_AluminiumInsulatorAlbedoMapResource(nullptr),
        m_AluminiumInsulatorNormalMapResource(nullptr),
        m_AluminiumInsulatorMetallicMapResource(nullptr),
        m_AluminiumInsulatorRoughnessMapResource(nullptr),
        m_AluminiumInsulatorAOMapResource(nullptr)
    {
        m_DebugInfoEnabled = false;
        m_EnvEquirectangularTextureName = "newport_loft";
    }

    DemoPBRIBLTextured::~DemoPBRIBLTextured()
    {
        // Get resource manager and release all resources.
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Resource should released by resource manager.
        YW_SAFE_DELETE(m_ModelSphereResource);
        YW_SAFE_DELETE(m_ModelCubeResource);
        YW_SAFE_DELETE(m_ModelPBRResource);

        YW_SAFE_DELETE(m_EnvEquirectangularTextureResource);
        YW_SAFE_DELETE(m_EnvCubeTextureResource);
        YW_SAFE_DELETE(m_IrrandianceCubeTextureResource);
        YW_SAFE_DELETE(m_PrefilterReflectionCubeTextureResource);
        YW_SAFE_DELETE(m_PreintegrateBRDFTextureResource);

        YW_SAFE_RELEASE(m_SkyVertexShader);
        YW_SAFE_RELEASE(m_SkyPixelShader);
        YW_SAFE_RELEASE(m_PBRIBLTexturedVertexShader);
        YW_SAFE_RELEASE(m_PBRIBLTexturedPixelShader);

        // Rusted iron.
        YW_SAFE_DELETE(m_RustedIronAlbedoMapResource);
        YW_SAFE_DELETE(m_RustedIronNormalMapResource);
        YW_SAFE_DELETE(m_RustedIronMetallicMapResource);
        YW_SAFE_DELETE(m_RustedIronRoughnessMapResource);
        YW_SAFE_DELETE(m_RustedIronAOMapResource);

        // Aluminium insulator.
        YW_SAFE_DELETE(m_AluminiumInsulatorAlbedoMapResource);
        YW_SAFE_DELETE(m_AluminiumInsulatorNormalMapResource);
        YW_SAFE_DELETE(m_AluminiumInsulatorMetallicMapResource);
        YW_SAFE_DELETE(m_AluminiumInsulatorRoughnessMapResource);
        YW_SAFE_DELETE(m_AluminiumInsulatorAOMapResource);
    }

    bool DemoPBRIBLTextured::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load basic model and texture.
        m_ModelSphereResource = ResourceHelper::LoadResource(resManager, "sphere.obj");
        if (nullptr == m_ModelSphereResource)
        {
            return false;
        }

        m_ModelCubeResource = ResourceHelper::LoadResource(resManager, "cube.obj");
        if (nullptr == m_ModelCubeResource)
        {
            return false;
        }

        m_ModelPBRResource = ResourceHelper::LoadResource(resManager, "sphere.obj");
        if (nullptr == m_ModelPBRResource)
        {
            return false;
        }

        // Load or create pre-computing data.
        if (!LoadAllPreComputingData())
        {
            return false;
        }

        // Create vertex and pixel shader.
        m_SkyVertexShader = new DemoPBRIBLCubeMapVertexShader();
        m_SkyPixelShader = new DemoPBRIBLCubeMapPixelShader();
        m_PBRIBLTexturedVertexShader = new DemoPBRIBLTexturedVertexShader();
        m_PBRIBLTexturedPixelShader = new DemoPBRIBLTexturedPixelShader();

        // Load rusted iron assets.
        if (!LoadRustedIronResources())
        {
            return false;
        }

        // Load aluminium insulator assets.
        if (!LoadAluminiumInsulatorResources())
        {
            return false;
        }

        return true;
    }

    bool DemoPBRIBLTextured::FrameMove()
    {
        return false;
    }

    void DemoPBRIBLTextured::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();

        // Render sky pass.
        graphics->PushStateBlock();
        RenderSky(pass);
        graphics->PopStateBlock();

        // Render rusted iron.
        //RenderRustedIron(pass);

        // Render aluminium insulator.
        RenderAluminiumInsulator(pass);
    }

    bool DemoPBRIBLTextured::LoadRustedIronResources()
    {
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load and get rusted iron.
        m_RustedIronAlbedoMapResource = ResourceHelper::LoadResource(resManager, "PBR/RustedIron/albedo.png");
        if (nullptr == m_RustedIronAlbedoMapResource)
        {
            return false;
        }

        m_RustedIronNormalMapResource = ResourceHelper::LoadResource(resManager, "PBR/RustedIron/normal.png");
        if (nullptr == m_RustedIronNormalMapResource)
        {
            return false;
        }

        m_RustedIronMetallicMapResource = ResourceHelper::LoadResource(resManager, "PBR/RustedIron/metallic.png");
        if (nullptr == m_RustedIronMetallicMapResource)
        {
            return false;
        }

        m_RustedIronRoughnessMapResource = ResourceHelper::LoadResource(resManager, "PBR/RustedIron/roughness.png");
        if (nullptr == m_RustedIronRoughnessMapResource)
        {
            return false;
        }

        m_RustedIronAOMapResource = ResourceHelper::LoadResource(resManager, "PBR/RustedIron/ao.png");
        if (nullptr == m_RustedIronAOMapResource)
        {
            return false;
        }

        return true;
    }

    bool DemoPBRIBLTextured::LoadAluminiumInsulatorResources()
    {
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load and get rusted iron.
        m_AluminiumInsulatorAlbedoMapResource = ResourceHelper::LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_BaseColor.png");
        if (nullptr == m_AluminiumInsulatorAlbedoMapResource)
        {
            return false;
        }

        m_AluminiumInsulatorNormalMapResource = ResourceHelper::LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_Normal.png");
        if (nullptr == m_AluminiumInsulatorNormalMapResource)
        {
            return false;
        }

        m_AluminiumInsulatorMetallicMapResource = ResourceHelper::LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_Metallic.png");
        if (nullptr == m_AluminiumInsulatorMetallicMapResource)
        {
            return false;
        }

        m_AluminiumInsulatorRoughnessMapResource = ResourceHelper::LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_Roughness.png");
        if (nullptr == m_AluminiumInsulatorRoughnessMapResource)
        {
            return false;
        }

        m_AluminiumInsulatorAOMapResource = ResourceHelper::LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_AO.png");
        if (nullptr == m_AluminiumInsulatorAOMapResource)
        {
            return false;
        }

        return true;
    }

    void DemoPBRIBLTextured::RenderRustedIron(int32_t pass)
    {
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        graphics->PushStateBlock();

        RenderTexturedPbrModel(
            pass,
            m_ModelPBRResource->GetResource<Model>(),
            Vector3::Zero(),
            QuaternionFromEuler(Quaternion(), 90.0f * DEG_TO_RAD, 0.0f, 0.0f),
            Vector3(1.8f, 1.8f, 1.8f),
            m_RustedIronAlbedoMapResource->GetResource<Yw3dTexture>(),
            m_RustedIronNormalMapResource->GetResource<Yw3dTexture>(),
            m_RustedIronMetallicMapResource->GetResource<Yw3dTexture>(),
            m_RustedIronRoughnessMapResource->GetResource<Yw3dTexture>(),
            m_RustedIronAOMapResource->GetResource<Yw3dTexture>()
        );

        graphics->PopStateBlock();
    }

    void DemoPBRIBLTextured::RenderAluminiumInsulator(int32_t pass)
    {
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        graphics->PushStateBlock();

        RenderTexturedPbrModel(
            pass,
            m_ModelPBRResource->GetResource<Model>(),
            Vector3::Zero(),
            QuaternionFromEuler(Quaternion(), 90.0f * DEG_TO_RAD, 0.0f, 0.0f),
            Vector3(1.8f, 1.8f, 1.8f),
            m_AluminiumInsulatorAlbedoMapResource->GetResource<Yw3dTexture>(),
            m_AluminiumInsulatorNormalMapResource->GetResource<Yw3dTexture>(),
            m_AluminiumInsulatorMetallicMapResource->GetResource<Yw3dTexture>(),
            m_AluminiumInsulatorRoughnessMapResource->GetResource<Yw3dTexture>(),
            m_AluminiumInsulatorAOMapResource->GetResource<Yw3dTexture>(),
            Yw3d_TF_Linear,
            Yw3d_TF_Linear,
            Yw3d_TF_Point,
            Yw3d_TF_Linear,
            Yw3d_TF_Linear,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f,
            -1.0f
        );

        graphics->PopStateBlock();
    }

    bool DemoPBRIBLTextured::LoadAllPreComputingData()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // The file checker.
        FileIO fileChecker;

        // Generate hdr cube map from hdr equirectangular map.
        const StringA& assetPath = resManager->GetDataPath();
        const StringA environmentMapName = "PBR/IBL/Environment/" + m_EnvEquirectangularTextureName + "_environment";
        const StringA environmentMapPath = assetPath + "/" + environmentMapName;
        if (!fileChecker.FileExists(environmentMapPath + "_ywt.cube"))
        {
            m_EnvEquirectangularTextureResource = ResourceHelper::LoadResource(resManager, m_EnvEquirectangularTextureName + ".hdr");
            if (nullptr == m_EnvEquirectangularTextureResource)
            {
                return false;
            }

            // Create temp cube texture.
            Yw3dCubeTexture* envCubeTexture = nullptr;

            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderEquirectangularMapToCubeMap(graphics, m_EnvEquirectangularTextureResource->GetResource<Yw3dTexture>(), &envCubeTexture, m_ModelSphereResource->GetResource<Model>());
            graphics->PopStateBlock();

            // Save cube texture to resource wrapper.
            m_EnvCubeTextureResource = new ResourceWrapper(resManager, ".cube", 0, envCubeTexture);

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(environmentMapPath + "_ywt.cube", envCubeTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(environmentMapPath + "_hdr.cube", envCubeTexture, true);
                YwTextureDataConverter::SaveCubeTextureDataToBMPFile(environmentMapPath + "_bmp.cube", envCubeTexture, true);
            }
        }
        else
        {
            // Load environment map.
            const StringA loadFileName = environmentMapName + "_ywt.cube";
            m_EnvCubeTextureResource = ResourceHelper::LoadResource(resManager, loadFileName);
            if (nullptr == m_EnvCubeTextureResource)
            {
                return false;
            }
        }

        // Generate diffuse irradiance map from hdr cube map.
        const StringA irradianceMapName = "PBR/IBL/Irradiance/" + m_EnvEquirectangularTextureName + "_irradiance";
        const StringA irradianceMapPath = assetPath + "/" + irradianceMapName;
        if (!fileChecker.FileExists(irradianceMapPath + "_ywt.cube"))
        {
            // Create temp cube texture.
            Yw3dCubeTexture* irrandianceCubeTexture = nullptr;
            
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderCubeMapToIrradianceMap(graphics, m_EnvCubeTextureResource->GetResource<Yw3dCubeTexture>(), &irrandianceCubeTexture, m_ModelSphereResource->GetResource<Model>());
            graphics->PopStateBlock();

            // Save cube texture to resource wrapper.
            m_IrrandianceCubeTextureResource = new ResourceWrapper(resManager, ".cube", 0, irrandianceCubeTexture);

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(irradianceMapPath + "_ywt.cube", irrandianceCubeTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(irradianceMapPath + "_hdr.cube", irrandianceCubeTexture, false);
                YwTextureDataConverter::SaveCubeTextureDataToBMPFile(irradianceMapPath + "_bmp.cube", irrandianceCubeTexture, false);
            }
        }
        else
        {
            // Load irradiance map.
            const StringA loadFileName = irradianceMapName + "_ywt.cube";
            m_IrrandianceCubeTextureResource = ResourceHelper::LoadResource(resManager, loadFileName);
            if (nullptr == m_IrrandianceCubeTextureResource)
            {
                return false;
            }
        }

        // Generate specular pre-filter reflection environment map.
        const StringA reflectionMapName = "PBR/IBL/Reflection/" + m_EnvEquirectangularTextureName + "_reflection";
        const StringA reflectionMapPath = assetPath + "/" + reflectionMapName;
        if (!fileChecker.FileExists(reflectionMapPath + "_ywt.cube"))
        {
            // Create temp cube texture.
            Yw3dCubeTexture* prefilterReflectionCubeTexture = nullptr;
            
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderPrefilterReflectionMap(graphics, m_EnvCubeTextureResource->GetResource<Yw3dCubeTexture>(), &prefilterReflectionCubeTexture, m_ModelSphereResource->GetResource<Model>());
            graphics->PopStateBlock();

            // Save cube texture to resource wrapper.
            m_PrefilterReflectionCubeTextureResource = new ResourceWrapper(resManager, ".cube", 0, prefilterReflectionCubeTexture);

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(reflectionMapPath + "_ywt.cube", prefilterReflectionCubeTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(reflectionMapPath + "_hdr.cube", prefilterReflectionCubeTexture, true);
                YwTextureDataConverter::SaveCubeTextureDataToBMPFile(reflectionMapPath + "_bmp.cube", prefilterReflectionCubeTexture, true);
            }
        }
        else
        {
            // Load pre-filter reflection map.
            const StringA loadFileName = reflectionMapName + "_ywt.cube";
            m_PrefilterReflectionCubeTextureResource = ResourceHelper::LoadResource(resManager, loadFileName);
            if (nullptr == m_PrefilterReflectionCubeTextureResource)
            {
                return false;
            }
        }

        // Generate specular specular pre-integral BRDF map.
        const StringA brdfMapName = "PBR/IBL/BRDF/brdf";
        const StringA brdfMapPath = assetPath + "/" + brdfMapName;
        if (!fileChecker.FileExists(brdfMapPath + ".ywt"))
        {
            // Create temp texture.
            Yw3dTexture* preintegrateBRDFTexture = nullptr;
            
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderPreintegrateBRDFMap(graphics, &preintegrateBRDFTexture);
            graphics->PopStateBlock();

            // Save texture to resource wrapper.
            m_PreintegrateBRDFTextureResource = new ResourceWrapper(resManager, ".ywt", 0, preintegrateBRDFTexture);

            // Used in runtime.
            YwTextureDataConverter::SaveTextureDataToYWTFile(brdfMapPath + ".ywt", preintegrateBRDFTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveTextureDataToRGBEFile(brdfMapPath + ".hdr", preintegrateBRDFTexture, false);
                YwTextureDataConverter::SaveTextureDataToBMPFile(brdfMapPath + ".bmp", preintegrateBRDFTexture, false);
            }
        }
        else
        {
            // Load pre-integral BRDF map.
            const StringA loadFileName = brdfMapName + ".ywt";
            m_PreintegrateBRDFTextureResource = ResourceHelper::LoadResource(resManager, loadFileName);
            if (nullptr == m_PreintegrateBRDFTextureResource)
            {
                return false;
            }
        }

        return true;
    }

    void DemoPBRIBLTextured::RenderSky(int32_t pass)
    {
        // Render sky.

        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        DemoPBRIBLTexturedCamera* camera = (DemoPBRIBLTexturedCamera*)(graphics->GetCurrentCamera());
        DemoPBRIBLTexturedApp* app = (DemoPBRIBLTexturedApp*)(GetScene()->GetApplication());

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        Quaternion quatWorld;
        QuaternionIdentity(quatWorld);

        // Apply model rotation.
        Matrix44Transformation(matWorld, Vector3(5.0f, 5.0f, 5.0f), quatWorld, camera->GetPosition());

        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // Setting matrices for device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set sky texture.
        graphics->SetTexture(0, m_EnvCubeTextureResource->GetResource<Yw3dCubeTexture>());
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_SkyVertexShader);
        graphics->SetPixelShader(m_SkyPixelShader);

        // Render sky model.
        m_ModelCubeResource->GetResource<Model>()->Render(device);
    }

    void DemoPBRIBLTextured::RenderTexturedPbrModel(
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
        Yw3dTextureFilter albedoMapFilter,
        Yw3dTextureFilter normalMapFilter,
        Yw3dTextureFilter metallicMapFilter,
        Yw3dTextureFilter roughnessMapFilter,
        Yw3dTextureFilter aoMapFilter,
        float albedoMipLodBias,
        float normalMipLodBias,
        float metallicMipLodBias,
        float roughnessMipLodBias,
        float aoMipLodBias
    )
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        DemoPBRIBLTexturedCamera* camera = (DemoPBRIBLTexturedCamera*)(graphics->GetCurrentCamera());
        DemoPBRIBLTexturedApp* app = (DemoPBRIBLTexturedApp*)(GetScene()->GetApplication());

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set precompute textures.

        graphics->SetTexture(0, m_IrrandianceCubeTextureResource->GetResource<Yw3dCubeTexture>());
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(1, m_PrefilterReflectionCubeTextureResource->GetResource<Yw3dCubeTexture>());
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(2, m_PreintegrateBRDFTextureResource->GetResource<Yw3dTexture>());
        graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set model textures

        graphics->SetTexture(3, albedoMap);
        graphics->SetTextureSamplerState(3, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(3, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(3, Yw3d_TSS_MinFilter, albedoMapFilter);
        graphics->SetTextureSamplerState(3, Yw3d_TSS_MagFilter, albedoMapFilter);
        graphics->SetTextureSamplerState(3, Yw3d_TSS_MipFilter, albedoMapFilter);
        graphics->SetTextureSamplerState(3, Yw3d_TSS_MipLodBias, *(uint32_t*)&albedoMipLodBias);

        graphics->SetTexture(4, normalMap);
        graphics->SetTextureSamplerState(4, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(4, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(4, Yw3d_TSS_MinFilter, normalMapFilter);
        graphics->SetTextureSamplerState(4, Yw3d_TSS_MagFilter, normalMapFilter);
        graphics->SetTextureSamplerState(4, Yw3d_TSS_MipFilter, normalMapFilter);
        graphics->SetTextureSamplerState(4, Yw3d_TSS_MipLodBias, *(uint32_t*)&normalMipLodBias);

        graphics->SetTexture(5, metallicMap);
        graphics->SetTextureSamplerState(5, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(5, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(5, Yw3d_TSS_MinFilter, metallicMapFilter);
        graphics->SetTextureSamplerState(5, Yw3d_TSS_MagFilter, metallicMapFilter);
        graphics->SetTextureSamplerState(5, Yw3d_TSS_MipFilter, metallicMapFilter);
        graphics->SetTextureSamplerState(5, Yw3d_TSS_MipLodBias, *(uint32_t*)&metallicMipLodBias);

        graphics->SetTexture(6, roughnessMap);
        graphics->SetTextureSamplerState(6, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(6, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(6, Yw3d_TSS_MinFilter, roughnessMapFilter);
        graphics->SetTextureSamplerState(6, Yw3d_TSS_MagFilter, roughnessMapFilter);
        graphics->SetTextureSamplerState(6, Yw3d_TSS_MipFilter, roughnessMapFilter);
        graphics->SetTextureSamplerState(6, Yw3d_TSS_MipLodBias, *(uint32_t*)&roughnessMipLodBias);

        graphics->SetTexture(7, aoMap);
        graphics->SetTextureSamplerState(7, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(7, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(7, Yw3d_TSS_MinFilter, aoMapFilter);
        graphics->SetTextureSamplerState(7, Yw3d_TSS_MagFilter, aoMapFilter);
        graphics->SetTextureSamplerState(7, Yw3d_TSS_MipFilter, aoMapFilter);
        graphics->SetTextureSamplerState(7, Yw3d_TSS_MipLodBias, *(uint32_t*)&aoMipLodBias);

        // Set pbr vertex and pixel shader.
        graphics->SetVertexShader(m_PBRIBLTexturedVertexShader);
        graphics->SetPixelShader(m_PBRIBLTexturedPixelShader);

        // The whole world transform.
        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Apply model rotation.
        Matrix44Transformation(matWorld, scale, rotation, translation);

        // Calculating transpose of inverse of world matrix, to transform normal and tangent of model to world space.
        Matrix44 matInvTrs;
        Matrix44Inverse(matInvTrs, matWorld);
        Matrix44Transpose(matInvTrs, matInvTrs);
        
        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Update shader parameters.
        m_PBRIBLTexturedVertexShader->SetMatrix(0, matInvTrs); // Inverse transpose of world matrix.
        m_PBRIBLTexturedPixelShader->SetVector(0, Vector3(1.0f, 1.0f, -1.0f)); // Light position.
        m_PBRIBLTexturedPixelShader->SetVector(1, Vector3(1.0f, 1.0f, 1.0f)); // Light color.
        m_PBRIBLTexturedPixelShader->SetVector(2, camera->GetPosition());

        // Render model.
        pbrModel->Render(device);
    }

    HRESOURCE DemoPBRIBLTextured::LoadResource(ResourceManager* resManager, const StringA& resourcePath)
    {
        if (nullptr == resManager)
        {
            return 0;
        }

        HRESOURCE resourceHandle = resManager->LoadResource(resourcePath);
        if (resourceHandle <= 0)
        {
            StringA errorMsg = "Load resource \"";
            errorMsg += resourcePath;
            errorMsg += "\" failed.";
            LOGE_A(errorMsg);

            return 0;
        }

        return resourceHandle;
    }

    void* DemoPBRIBLTextured::GetResource(ResourceManager* resManager, HRESOURCE resourceHandle)
    {
        if ((nullptr == resManager) || (resourceHandle <= 0))
        {
            return 0;
        }

        void* resourcePtr = resManager->GetResource(resourceHandle);
        return resourcePtr;
    }
}
