// Add by Yaukey at 2022-04-18.
// YW Soft Renderer demo with PBR-IBL class.

#include "YwDemoPBRIBL.h"
#include "YwDemoPBRIBLApp.h"
#include "YwDemoPBRIBLCamera.h"
#include "YwDemoPBRIBLShaderPrecompute.h"
#include "YwDemoPBRIBLShaderSky.h"
#include "YwDemoPBRIBLShader.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"
#include "YwTextureDataConverter.h"
#include "YwFileIO.h"
#include "YwDemoPBRIBLPrecompute.h"

#ifndef YW_SAFE_UNLOAD_RESOURCE
#define YW_SAFE_UNLOAD_RESOURCE(m, r) {if ((r) > 0) {(m)->UnloadResource((r)); (r) = 0;}}
#endif

namespace yw
{
    // ------------------------------------------------------------------
    // PBR entity.

    DemoPBRIBL::DemoPBRIBL(Scene* scene) :
        IEntity(scene),
        m_ModelSkySphere(nullptr),
        m_ModelSkyCube(nullptr),
        //m_ModelPBR(nullptr),

        m_EnvEquirectangularTexture(nullptr),
        m_EnvCubeTexture(nullptr),
        m_IrrandianceCubeTexture(nullptr),
        m_PrefilterReflectionCubeTexture(nullptr),
        m_PreintegrateBRDFTexture(nullptr),

        m_ModelPBRTexture(nullptr),
        m_ModelPBRNormalTexture(nullptr),
        m_ModelPBRSpecularTexture(nullptr),

        m_ModelSkySphereHandle(0),
        m_ModelSkyCubeHandle(0),
        m_ModelPBRHandle(0),
        m_ModelSkySphereTextureHandle(0),
        m_ModelPBRTextureHandle(0),
        m_ModelPBRNormalTextureHandle(0),
        m_ModelPBRSpecularTextureHandle(0),

        m_EnvCubeTextureHandle(0),
        m_IrrandianceCubeTextureHandle(0),
        m_PrefilterReflectionCubeTextureHandle(0),
        m_PreintegrateBRDFTextureHandle(0),

        m_SkyVertexShader(nullptr),
        m_SkyPixelShader(nullptr),
        m_PbrVertexShader(nullptr),
        m_PbrPixelShader(nullptr),
        m_Metallic(0.0f),
        m_Roughness(0.0f),
        m_DebugInfoEnabled(false)
    {
        m_EnvEquirectangularTextureName = "newport_loft";
    }

    DemoPBRIBL::~DemoPBRIBL()
    {
        // Resource should released by resource manager.
        m_ModelSkySphere = nullptr;
        m_ModelPBR = nullptr;
        m_EnvEquirectangularTexture = nullptr;
        m_ModelPBRTexture = nullptr;
        m_ModelPBRNormalTexture = nullptr;
        m_ModelPBRSpecularTexture = nullptr;

        YW_SAFE_RELEASE(m_EnvCubeTexture);
        YW_SAFE_RELEASE(m_IrrandianceCubeTexture);
        YW_SAFE_RELEASE(m_PrefilterReflectionCubeTexture);
        YW_SAFE_RELEASE(m_PreintegrateBRDFTexture);

        // Get resource manager and release all resources.
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelSkySphereHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelSkyCubeHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelPBRHandle);

        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelSkySphereTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelPBRTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelPBRNormalTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_ModelPBRSpecularTextureHandle);

        YW_SAFE_UNLOAD_RESOURCE(resManager, m_EnvCubeTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_IrrandianceCubeTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_PrefilterReflectionCubeTextureHandle);
        YW_SAFE_UNLOAD_RESOURCE(resManager, m_PreintegrateBRDFTextureHandle);

        YW_SAFE_RELEASE(m_SkyVertexShader);
        YW_SAFE_RELEASE(m_SkyPixelShader);
        YW_SAFE_RELEASE(m_PbrVertexShader);
        YW_SAFE_RELEASE(m_PbrPixelShader);
    }

    bool DemoPBRIBL::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load model and texture.
        m_ModelSkySphereHandle = resManager->LoadResource("sphere.obj");
        if (m_ModelSkySphereHandle <= 0)
        {
            LOGE(_T("Load resource \"sphere.obj\" failed."));
            return false;
        }

        m_ModelSkyCubeHandle = resManager->LoadResource("cube.obj");
        if (m_ModelSkyCubeHandle <= 0)
        {
            LOGE(_T("Load resource \"cube.obj\" failed."));
            return false;
        }

        m_ModelPBRHandle = resManager->LoadResource("sphere.obj"); // Lu_Head.obj
        if (m_ModelPBRHandle <= 0)
        {
            LOGE(_T("Load resource \"Lu_Head.obj\" failed."));
            return false;
        }

        m_ModelSkySphereTextureHandle = resManager->LoadResource(m_EnvEquirectangularTextureName + ".hdr");
        if (m_ModelSkySphereTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"newport_loft.hdr\" failed."));
            return false;
        }

        //m_ModelPBRTextureHandle = resManager->LoadResource("Lieutenant_head_diffuse.tga");
        //if (m_ModelPBRTextureHandle <= 0)
        //{
        //    LOGE(_T("Load resource \"Lieutenant_head_diffuse.tga\" failed."));
        //    return false;
        //}

        //m_ModelPBRNormalTextureHandle = resManager->LoadResource("Lieutenant_head_normal.tga");
        //if (m_ModelPBRNormalTextureHandle <= 0)
        //{
        //    LOGE(_T("Load resource \"Lieutenant_head_normal.tga\" failed."));
        //    return false;
        //}

        //m_ModelPBRSpecularTextureHandle = resManager->LoadResource("Lieutenant_head_specular.tga");
        //if (m_ModelPBRSpecularTextureHandle <= 0)
        //{
        //    LOGE(_T("Load resource \"Lieutenant_head_specular.tga\" failed."));
        //    return false;
        //}

        // Get model and texture.
        m_ModelSkySphere = (Model*)resManager->GetResource(m_ModelSkySphereHandle);
        if (nullptr == m_ModelSkySphere)
        {
            LOGE(_T("Get resource \"sphere.obj\" failed."));
            return false;
        }

        m_ModelSkyCube = (Model*)resManager->GetResource(m_ModelSkyCubeHandle);
        if (nullptr == m_ModelSkyCube)
        {
            LOGE(_T("Get resource \"cube.obj\" failed."));
            return false;
        }

        m_ModelPBR = (Model*)resManager->GetResource(m_ModelPBRHandle);
        if (nullptr == m_ModelPBR)
        {
            LOGE(_T("Get resource \"Lu_Head.obj\" failed."));
            return false;
        }

        m_EnvEquirectangularTexture = (Yw3dTexture*)resManager->GetResource(m_ModelSkySphereTextureHandle);
        if (nullptr == m_EnvEquirectangularTexture)
        {
            LOGE(_T("Get resource \"room.cube\" failed."));
            return false;
        }

        //m_ModelPBRTexture = (Yw3dTexture*)resManager->GetResource(m_ModelPBRTextureHandle);
        //if (nullptr == m_ModelPBRTexture)
        //{
        //    LOGE(_T("Get resource \"Lieutenant_head_diffuse.tga\" failed."));
        //    return false;
        //}

        //m_ModelPBRNormalTexture = (Yw3dTexture*)resManager->GetResource(m_ModelPBRNormalTextureHandle);
        //if (nullptr == m_ModelPBRNormalTexture)
        //{
        //    LOGE(_T("Get resource \"Lieutenant_head_normal.tga\" failed."));
        //    return false;
        //}

        //m_ModelPBRSpecularTexture = (Yw3dTexture*)resManager->GetResource(m_ModelPBRSpecularTextureHandle);
        //if (nullptr == m_ModelPBRSpecularTexture)
        //{
        //    LOGE(_T("Get resource \"Lieutenant_head_specular.tga\" failed."));
        //    return false;
        //}

        // Render pre-computing data.
        LoadAllPreComputingData();

        // Create vertex and pixel shader.
        m_SkyVertexShader = new DemoPBRIBLCubeMapVertexShader();
        m_SkyPixelShader = new DemoPBRIBLCubeMapPixelShader();
        m_PbrVertexShader = new DemoPBRIBLVertexShader();
        m_PbrPixelShader = new DemoPBRIBLPixelShader();

        // Initialize environments.
        m_Albedo.Set(0.5f, 0.5f, 0.5f);
        m_Metallic = 0.4f;
        m_Roughness = 0.2f;

        return true;
    }

    bool DemoPBRIBL::FrameMove()
    {
        return false;
    }

    void DemoPBRIBL::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();

        // Render sky pass.
        graphics->PushStateBlock();
        RenderSky(pass);
        graphics->PopStateBlock();

        // Render model.
        graphics->PushStateBlock();
        RenderPbrModel(pass);
        graphics->PopStateBlock();
    }

    bool DemoPBRIBL::LoadAllPreComputingData()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // The file checker.
        FileIO fileChecker;

        // Generate hdr cube map from hdr equirectangular map.
        const StringA& assetPath = resManager->GetDataPath();
        const StringA environmentMapName = "IBL/Environment/" + m_EnvEquirectangularTextureName + "_environment";
        const StringA environmentMapPath = assetPath + "/" + environmentMapName;
        if (!fileChecker.FileExists(environmentMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderEquirectangularMapToCubeMap(graphics, m_EnvEquirectangularTexture, &m_EnvCubeTexture, m_ModelSkySphere);
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
            m_EnvCubeTextureHandle = resManager->LoadResource(loadFileName);
            if (m_EnvCubeTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-computing environment cube texture failed."));
                return false;
            }

            m_EnvCubeTexture = (Yw3dCubeTexture*)resManager->GetResource(m_EnvCubeTextureHandle);
            if (nullptr == m_EnvCubeTexture)
            {
                LOGE(_T("Get resource pre-computing environment cube texture failed."));
                return false;
            }
        }

        // Generate diffuse irradiance map from hdr cube map.
        const StringA irradianceMapName = "IBL/Irradiance/" + m_EnvEquirectangularTextureName + "_irradiance";
        const StringA irradianceMapPath = assetPath + "/" + irradianceMapName;
        if (!fileChecker.FileExists(irradianceMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderCubeMapToIrradianceMap(graphics, m_EnvCubeTexture, &m_IrrandianceCubeTexture, m_ModelSkySphere);
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
            m_IrrandianceCubeTextureHandle = resManager->LoadResource(loadFileName);
            if (m_IrrandianceCubeTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-computing irrandiance cube texture failed."));
                return false;
            }

            m_IrrandianceCubeTexture = (Yw3dCubeTexture*)resManager->GetResource(m_IrrandianceCubeTextureHandle);
            if (nullptr == m_IrrandianceCubeTexture)
            {
                LOGE(_T("Get resource pre-computing irrandiance cube texture failed."));
                return false;
            }
        }

        // Generate specular pre-filter reflection environment map.
        const StringA reflectionMapName = "IBL/Reflection/" + m_EnvEquirectangularTextureName + "_reflection";
        const StringA reflectionMapPath = assetPath + "/" + reflectionMapName;
        if (!fileChecker.FileExists(reflectionMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            DemoPBRIBLPrecompute::RenderPrefilterReflectionMap(graphics, m_EnvCubeTexture, &m_PrefilterReflectionCubeTexture, m_ModelSkySphere);
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
            m_PrefilterReflectionCubeTextureHandle = resManager->LoadResource(loadFileName);
            if (m_PrefilterReflectionCubeTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-filter reflection cube texture failed."));
                return false;
            }

            m_PrefilterReflectionCubeTexture = (Yw3dCubeTexture*)resManager->GetResource(m_PrefilterReflectionCubeTextureHandle);
            if (nullptr == m_PrefilterReflectionCubeTexture)
            {
                LOGE(_T("Get resource pre-filter reflection cube texture failed."));
                return false;
            }
        }

        // Generate specular specular pre-integral BRDF map.
        const StringA brdfMapName = "IBL/BRDF/brdf";
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
            m_PreintegrateBRDFTextureHandle = resManager->LoadResource(loadFileName);
            if (m_PreintegrateBRDFTextureHandle <= 0)
            {
                LOGE(_T("Load resource pre-integral brdf texture failed."));
                return false;
            }

            m_PreintegrateBRDFTexture = (Yw3dTexture*)resManager->GetResource(m_PreintegrateBRDFTextureHandle);
            if (nullptr == m_PreintegrateBRDFTexture)
            {
                LOGE(_T("Get resource pre-integral brdf texture failed."));
                return false;
            }
        }

        return true;
    }

    void DemoPBRIBL::RenderSky(int32_t pass)
    {
        // Render sky.

        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        DemoPBRIBLCamera* camera = (DemoPBRIBLCamera*)(graphics->GetCurrentCamera());
        DemoPBRIBLApp* app = (DemoPBRIBLApp*)(GetScene()->GetApplication());

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Apply model rotation.
        Matrix44Transformation(matWorld, Vector3(5.0f, 5.0f, 5.0f), camera->GetWorldRotation(), camera->GetPosition());

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
        m_ModelSkyCube->Render(device);
    }

    void DemoPBRIBL::RenderPbrModel(int32_t pass)
    {
        // Render pbr model.

        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        DemoPBRIBLCamera* camera = (DemoPBRIBLCamera*)(graphics->GetCurrentCamera());
        DemoPBRIBLApp* app = (DemoPBRIBLApp*)(GetScene()->GetApplication());

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set textures.

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

        // Set pbr vertex and pixel shader.
        graphics->SetVertexShader(m_PbrVertexShader);
        graphics->SetPixelShader(m_PbrPixelShader);

        const float startX = -1.3f;
        const float startY = 1.0f;
        const float stepX = 0.45f;
        const float stepY = -0.5f;
        const int32_t maxRows = 5;
        const int32_t maxColumns = 7;
        for (int32_t i = 0; i < maxColumns; i++)
        {
            for (int32_t j = 0; j < maxRows; j++)
            {
                Matrix44 matWorld;
                Matrix44Identity(matWorld);

                // Apply model rotation.
                Matrix44Transformation(matWorld, Vector3(0.4f, 0.4f, 0.4f), camera->GetWorldRotation(), Vector3(startX + stepX * i, startY + stepY * j, 0.8f));

                // Set world transform to camera.
                camera->SetWorldMatrix(matWorld);

                // This should be from device.
                Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
                device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
                device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
                device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
                device->SetTransform(Yw3d_TS_WVP, &matProjection);

                // Update shader parameters.
                m_PbrPixelShader->SetFloat(0, 1.0f - (float)j / (float)maxRows);
                m_PbrPixelShader->SetFloat(1, Clamp((float)i / (float)maxColumns, 0.05f, 1.0f));
                m_PbrPixelShader->SetVector(0, m_Albedo);
                m_PbrPixelShader->SetVector(1, camera->GetPosition());

                // Render model.
                m_ModelPBR->Render(device);
            }
        }
    }
}
