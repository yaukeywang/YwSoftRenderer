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
        m_ModelSphere(nullptr),
        m_ModelCube(nullptr),
        m_ModelPBR(nullptr),
        m_ModelSphereHandle(0),
        m_ModelCubeHandle(0),
        m_ModelPBRHandle(0),

        m_EnvEquirectangularTextureHandle(0),
        m_EnvCubeTextureHandle(0),
        m_IrrandianceCubeTextureHandle(0),
        m_PrefilterReflectionCubeTextureHandle(0),
        m_PreintegrateBRDFTextureHandle(0),
        m_EnvEquirectangularTexture(nullptr),
        m_EnvCubeTexture(nullptr),
        m_IrrandianceCubeTexture(nullptr),
        m_PrefilterReflectionCubeTexture(nullptr),
        m_PreintegrateBRDFTexture(nullptr),

        m_SkyVertexShader(nullptr),
        m_SkyPixelShader(nullptr),
        m_PBRIBLTexturedVertexShader(nullptr),
        m_PBRIBLTexturedPixelShader(nullptr),

        // Rusted iron.
        m_IronAlbedoMapHandle(0),
        m_IronNormalMapHandle(0),
        m_IronMetallicMapHandle(0),
        m_IronRoughnessMapHandle(0),
        m_IronAOMapHandle(0),
        m_IronAlbedoMap(nullptr),
        m_IronNormalMap(nullptr),
        m_IronMetallicMap(nullptr),
        m_IronRoughnessMap(nullptr),
        m_IronAOMap(nullptr),

        // Aluminium insulator.
        m_AluminiumInsulatorAlbedoMapHandle(0),
        m_AluminiumInsulatorNormalMapHandle(0),
        m_AluminiumInsulatorMetallicMapHandle(0),
        m_AluminiumInsulatorRoughnessMapHandle(0),
        m_AluminiumInsulatorAOMapHandle(0),
        m_AluminiumInsulatorAlbedoMap(nullptr),
        m_AluminiumInsulatorNormalMap(nullptr),
        m_AluminiumInsulatorMetallicMap(nullptr),
        m_AluminiumInsulatorRoughnessMap(nullptr),
        m_AluminiumInsulatorAOMap(nullptr)
    {
        m_DebugInfoEnabled = false;
        m_EnvEquirectangularTextureName = "newport_loft";
    }

    DemoPBRIBLTextured::~DemoPBRIBLTextured()
    {
        // Get resource manager and release all resources.
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Resource should released by resource manager.
        m_ModelSphere = nullptr;
        m_ModelCube = nullptr;
        m_ModelPBR = nullptr;
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelSphereHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelCubeHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelPBRHandle);

        YW_SAFE_RELEASE(m_EnvEquirectangularTexture);
        YW_SAFE_RELEASE(m_EnvCubeTexture);
        YW_SAFE_RELEASE(m_IrrandianceCubeTexture);
        YW_SAFE_RELEASE(m_PrefilterReflectionCubeTexture);
        YW_SAFE_RELEASE(m_PreintegrateBRDFTexture);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_EnvEquirectangularTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_EnvCubeTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IrrandianceCubeTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_PrefilterReflectionCubeTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_PreintegrateBRDFTextureHandle);

        YW_SAFE_RELEASE(m_SkyVertexShader);
        YW_SAFE_RELEASE(m_SkyPixelShader);
        YW_SAFE_RELEASE(m_PBRIBLTexturedVertexShader);
        YW_SAFE_RELEASE(m_PBRIBLTexturedPixelShader);

        // Rusted iron.
        YW_SAFE_RELEASE(m_IronAlbedoMap);
        YW_SAFE_RELEASE(m_IronNormalMap);
        YW_SAFE_RELEASE(m_IronMetallicMap);
        YW_SAFE_RELEASE(m_IronRoughnessMap);
        YW_SAFE_RELEASE(m_IronAOMap);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IronAlbedoMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IronNormalMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IronMetallicMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IronRoughnessMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IronAOMapHandle);

        // Aluminium insulator.
        YW_SAFE_RELEASE(m_AluminiumInsulatorAlbedoMap);
        YW_SAFE_RELEASE(m_AluminiumInsulatorNormalMap);
        YW_SAFE_RELEASE(m_AluminiumInsulatorMetallicMap);
        YW_SAFE_RELEASE(m_AluminiumInsulatorRoughnessMap);
        YW_SAFE_RELEASE(m_AluminiumInsulatorAOMap);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_AluminiumInsulatorAlbedoMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_AluminiumInsulatorNormalMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_AluminiumInsulatorMetallicMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_AluminiumInsulatorRoughnessMapHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_AluminiumInsulatorAOMapHandle);
    }

    bool DemoPBRIBLTextured::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load basic model and texture.
        m_ModelSphereHandle = LoadResource(resManager, "sphere.obj");
        m_ModelSphere = (Model*)GetResource(resManager, m_ModelSphereHandle);
        if (nullptr == m_ModelSphere)
        {
            return false;
        }

        m_ModelCubeHandle = LoadResource(resManager, "cube.obj");
        m_ModelCube = (Model*)GetResource(resManager, m_ModelCubeHandle);
        if (nullptr == m_ModelCube)
        {
            return false;
        }

        m_ModelPBRHandle = LoadResource(resManager, "sphere.obj");
        m_ModelPBR = (Model*)GetResource(resManager, m_ModelPBRHandle);
        if (nullptr == m_ModelPBR)
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
        m_IronAlbedoMapHandle = LoadResource(resManager, "PBR/RustedIron/albedo.png");
        m_IronAlbedoMap = (Yw3dTexture*)GetResource(resManager, m_IronAlbedoMapHandle);
        if (nullptr == m_IronAlbedoMap)
        {
            return false;
        }

        m_IronNormalMapHandle = LoadResource(resManager, "PBR/RustedIron/normal.png");
        m_IronNormalMap = (Yw3dTexture*)GetResource(resManager, m_IronNormalMapHandle);
        if (nullptr == m_IronNormalMap)
        {
            return false;
        }

        m_IronMetallicMapHandle = LoadResource(resManager, "PBR/RustedIron/metallic.png");
        m_IronMetallicMap = (Yw3dTexture*)GetResource(resManager, m_IronMetallicMapHandle);
        if (nullptr == m_IronMetallicMap)
        {
            return false;
        }

        m_IronRoughnessMapHandle = LoadResource(resManager, "PBR/RustedIron/roughness.png");
        m_IronRoughnessMap = (Yw3dTexture*)GetResource(resManager, m_IronRoughnessMapHandle);
        if (nullptr == m_IronRoughnessMap)
        {
            return false;
        }

        m_IronAOMapHandle = LoadResource(resManager, "PBR/RustedIron/ao.png");
        m_IronAOMap = (Yw3dTexture*)GetResource(resManager, m_IronAOMapHandle);
        if (nullptr == m_IronAOMap)
        {
            return false;
        }

        return true;
    }

    bool DemoPBRIBLTextured::LoadAluminiumInsulatorResources()
    {
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load and get rusted iron.
        m_AluminiumInsulatorAlbedoMapHandle = LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_BaseColor.png");
        m_AluminiumInsulatorAlbedoMap = (Yw3dTexture*)GetResource(resManager, m_AluminiumInsulatorAlbedoMapHandle);
        if (nullptr == m_AluminiumInsulatorAlbedoMap)
        {
            return false;
        }

        m_AluminiumInsulatorNormalMapHandle = LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_Normal.png");
        m_AluminiumInsulatorNormalMap = (Yw3dTexture*)GetResource(resManager, m_AluminiumInsulatorNormalMapHandle);
        if (nullptr == m_AluminiumInsulatorNormalMap)
        {
            return false;
        }

        m_AluminiumInsulatorMetallicMapHandle = LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_Metallic.png");
        m_AluminiumInsulatorMetallicMap = (Yw3dTexture*)GetResource(resManager, m_AluminiumInsulatorMetallicMapHandle);
        if (nullptr == m_AluminiumInsulatorMetallicMap)
        {
            return false;
        }

        m_AluminiumInsulatorRoughnessMapHandle = LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_Roughness.png");
        m_AluminiumInsulatorRoughnessMap = (Yw3dTexture*)GetResource(resManager, m_AluminiumInsulatorRoughnessMapHandle);
        if (nullptr == m_AluminiumInsulatorRoughnessMap)
        {
            return false;
        }

        m_AluminiumInsulatorAOMapHandle = LoadResource(resManager, "PBR/SubstancePainter/AluminiumInsulator/AI_AO.png");
        m_AluminiumInsulatorAOMap = (Yw3dTexture*)GetResource(resManager, m_AluminiumInsulatorAOMapHandle);
        if (nullptr == m_AluminiumInsulatorAOMap)
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
            m_ModelPBR,
            Vector3::Zero(),
            QuaternionFromEuler(Quaternion(), 90.0f * DEG_TO_RAD, 0.0f, 0.0f),
            Vector3(1.8f, 1.8f, 1.8f),
            m_IronAlbedoMap,
            m_IronNormalMap,
            m_IronMetallicMap,
            m_IronRoughnessMap,
            m_IronAOMap
        );

        graphics->PopStateBlock();
    }

    void DemoPBRIBLTextured::RenderAluminiumInsulator(int32_t pass)
    {
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        graphics->PushStateBlock();

        RenderTexturedPbrModel(
            pass,
            m_ModelPBR,
            Vector3::Zero(),
            QuaternionFromEuler(Quaternion(), 90.0f * DEG_TO_RAD, 0.0f, 0.0f),
            Vector3(1.8f, 1.8f, 1.8f),
            m_AluminiumInsulatorAlbedoMap,
            m_AluminiumInsulatorNormalMap,
            m_AluminiumInsulatorMetallicMap,
            m_AluminiumInsulatorRoughnessMap,
            m_AluminiumInsulatorAOMap,
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
            m_EnvEquirectangularTextureHandle = LoadResource(resManager, m_EnvEquirectangularTextureName + ".hdr");
            if (m_EnvEquirectangularTextureHandle <= 0)
            {
                return false;
            }

            m_EnvEquirectangularTexture = (Yw3dTexture*)GetResource(resManager, m_EnvEquirectangularTextureHandle);
            assert(nullptr != m_EnvEquirectangularTexture);

            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderEquirectangularMapToCubeMap(graphics, m_EnvEquirectangularTexture, &m_EnvCubeTexture, m_ModelSphere);
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(environmentMapPath + "_ywt.cube", m_EnvCubeTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(environmentMapPath + "_hdr.cube", m_EnvCubeTexture, true);
                YwTextureDataConverter::SaveCubeTextureDataToBMPFile(environmentMapPath + "_bmp.cube", m_EnvCubeTexture, true);
            }
        }
        else
        {
            // Load environment map.
            const StringA loadFileName = environmentMapName + "_ywt.cube";
            m_EnvCubeTextureHandle = LoadResource(resManager, loadFileName);
            if (m_EnvCubeTextureHandle <= 0)
            {
                return false;
            }

            m_EnvCubeTexture = (Yw3dCubeTexture*)GetResource(resManager, m_EnvCubeTextureHandle);
            assert(nullptr != m_EnvCubeTexture);
        }

        // Generate diffuse irradiance map from hdr cube map.
        const StringA irradianceMapName = "PBR/IBL/Irradiance/" + m_EnvEquirectangularTextureName + "_irradiance";
        const StringA irradianceMapPath = assetPath + "/" + irradianceMapName;
        if (!fileChecker.FileExists(irradianceMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderCubeMapToIrradianceMap(graphics, m_EnvCubeTexture, &m_IrrandianceCubeTexture, m_ModelSphere);
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(irradianceMapPath + "_ywt.cube", m_IrrandianceCubeTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(irradianceMapPath + "_hdr.cube", m_IrrandianceCubeTexture, false);
                YwTextureDataConverter::SaveCubeTextureDataToBMPFile(irradianceMapPath + "_bmp.cube", m_IrrandianceCubeTexture, false);
            }
        }
        else
        {
            // Load irradiance map.
            const StringA loadFileName = irradianceMapName + "_ywt.cube";
            m_IrrandianceCubeTextureHandle = LoadResource(resManager, loadFileName);
            if (m_IrrandianceCubeTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-computing irrandiance cube texture failed."));
                return false;
            }

            m_IrrandianceCubeTexture = (Yw3dCubeTexture*)GetResource(resManager, m_IrrandianceCubeTextureHandle);
            if (nullptr == m_IrrandianceCubeTexture)
            {
                LOGE(_T("Get resource pre-computing irrandiance cube texture failed."));
                return false;
            }
        }

        // Generate specular pre-filter reflection environment map.
        const StringA reflectionMapName = "PBR/IBL/Reflection/" + m_EnvEquirectangularTextureName + "_reflection";
        const StringA reflectionMapPath = assetPath + "/" + reflectionMapName;
        if (!fileChecker.FileExists(reflectionMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderPrefilterReflectionMap(graphics, m_EnvCubeTexture, &m_PrefilterReflectionCubeTexture, m_ModelSphere);
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(reflectionMapPath + "_ywt.cube", m_PrefilterReflectionCubeTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(reflectionMapPath + "_hdr.cube", m_PrefilterReflectionCubeTexture, true);
                YwTextureDataConverter::SaveCubeTextureDataToBMPFile(reflectionMapPath + "_bmp.cube", m_PrefilterReflectionCubeTexture, true);
            }
        }
        else
        {
            // Load pre-filter reflection map.
            const StringA loadFileName = reflectionMapName + "_ywt.cube";
            m_PrefilterReflectionCubeTextureHandle = LoadResource(resManager, loadFileName);
            if (m_PrefilterReflectionCubeTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-filter reflection cube texture failed."));
                return false;
            }

            m_PrefilterReflectionCubeTexture = (Yw3dCubeTexture*)GetResource(resManager, m_PrefilterReflectionCubeTextureHandle);
            if (nullptr == m_PrefilterReflectionCubeTexture)
            {
                LOGE(_T("Get resource pre-filter reflection cube texture failed."));
                return false;
            }
        }

        // Generate specular specular pre-integral BRDF map.
        const StringA brdfMapName = "PBR/IBL/BRDF/brdf";
        const StringA brdfMapPath = assetPath + "/" + brdfMapName;
        if (!fileChecker.FileExists(brdfMapPath + ".ywt"))
        {
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderPreintegrateBRDFMap(graphics, &m_PreintegrateBRDFTexture);
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveTextureDataToYWTFile(brdfMapPath + ".ywt", m_PreintegrateBRDFTexture);

            // Used to debug.
            if (m_DebugInfoEnabled)
            {
                YwTextureDataConverter::SaveTextureDataToRGBEFile(brdfMapPath + ".hdr", m_PreintegrateBRDFTexture, false);
                YwTextureDataConverter::SaveTextureDataToBMPFile(brdfMapPath + ".bmp", m_PreintegrateBRDFTexture, false);
            }
        }
        else
        {
            // Load pre-integral BRDF map.
            const StringA loadFileName = brdfMapName + ".ywt";
            m_PreintegrateBRDFTextureHandle = LoadResource(resManager, loadFileName);
            if (m_PreintegrateBRDFTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-integral brdf texture failed."));
                return false;
            }

            m_PreintegrateBRDFTexture = (Yw3dTexture*)GetResource(resManager, m_PreintegrateBRDFTextureHandle);
            if (nullptr == m_PreintegrateBRDFTexture)
            {
                LOGE(_T("Get resource pre-integral brdf texture failed."));
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
        graphics->SetTexture(0, m_EnvCubeTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_SkyVertexShader);
        graphics->SetPixelShader(m_SkyPixelShader);

        // Render sky model.
        m_ModelCube->Render(device);
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

        graphics->SetTexture(0, m_IrrandianceCubeTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(1, m_PrefilterReflectionCubeTexture);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(2, m_PreintegrateBRDFTexture);
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
