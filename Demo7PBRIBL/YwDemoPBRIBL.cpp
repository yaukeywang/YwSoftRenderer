// Add by Yaukey at 2022-04-18.
// YW Soft Renderer demo with PBR-IBL class.

#include "YwDemoPBRIBL.h"
#include "YwDemoPBRIBLApp.h"
#include "YwDemoPBRIBLCamera.h"
#include "YwDemoPBRIBLShader.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"
#include "YwTextureDataConverter.h"
#include "YwFileIO.h"
#include "YwPaths.h"

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
        m_Roughness(0.0f)
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
        m_Albedo.Set(0.8f, 0.2f, 0.2f);
        m_Metallic = 0.9f;
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
        const StringA environmentMapName = "IBL/Environment/" + m_EnvEquirectangularTextureName + "_environment";
        const StringA environmentMapPath = "./Resources/" + environmentMapName;
        if (!fileChecker.FileExists(environmentMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            RenderEquirectangularMapToCubeMap();
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(environmentMapPath + "_ywt.cube", m_EnvCubeTexture);

            // Used to debug.
            YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(environmentMapPath + "_hdr.cube", m_EnvCubeTexture, true);
            YwTextureDataConverter::SaveCubeTextureDataToBMPFile(environmentMapPath + "_bmp.cube", m_EnvCubeTexture, true);
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
        const StringA irradianceMapPath = "./Resources/" + irradianceMapName;
        if (!fileChecker.FileExists(irradianceMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            RenderCubeMapToIrradianceMap();
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(irradianceMapPath + "_ywt.cube", m_IrrandianceCubeTexture);

            // Used to debug.
            YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(irradianceMapPath + "_hdr.cube", m_IrrandianceCubeTexture, false);
            YwTextureDataConverter::SaveCubeTextureDataToBMPFile(irradianceMapPath + "_bmp.cube", m_IrrandianceCubeTexture, false);
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
        const StringA reflectionMapPath = "./Resources/" + reflectionMapName;
        if (!fileChecker.FileExists(reflectionMapPath + "_ywt.cube"))
        {
            graphics->PushStateBlock();
            RenderPrefilterReflectionMap();
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveCubeTextureDataToYWTFile(reflectionMapPath + "_ywt.cube", m_PrefilterReflectionCubeTexture);

            // Used to debug.
            YwTextureDataConverter::SaveCubeTextureDataToRGBEFile(reflectionMapPath + "_hdr.cube", m_PrefilterReflectionCubeTexture, true);
            YwTextureDataConverter::SaveCubeTextureDataToBMPFile(reflectionMapPath + "_bmp.cube", m_PrefilterReflectionCubeTexture, true);
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
        const StringA brdfMapPath = "./Resources/" + brdfMapName;
        if (!fileChecker.FileExists(brdfMapPath + ".ywt"))
        {
            graphics->PushStateBlock();
            RenderPreintegrateBRDFMap();
            graphics->PopStateBlock();

            // Used in runtime.
            YwTextureDataConverter::SaveTextureDataToYWTFile(brdfMapPath + ".ywt", m_PreintegrateBRDFTexture);

            // Used to debug.
            YwTextureDataConverter::SaveTextureDataToRGBEFile(brdfMapPath + ".hdr", m_PreintegrateBRDFTexture, false);
            YwTextureDataConverter::SaveTextureDataToBMPFile(brdfMapPath + ".bmp", m_PreintegrateBRDFTexture, false);
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

    bool DemoPBRIBL::RenderEquirectangularMapToCubeMap()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();

        // Render constants.
        const int32_t targetWidth = 512;
        const int32_t targetHeight = 512;
        const int32_t cubeLength = 512;
        const Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;

        // Create a temp render target.
        Yw3dRenderTarget* rtCubemap = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&rtCubemap, targetWidth, targetHeight, cubeFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Create cube map.
        YW_SAFE_RELEASE(m_EnvCubeTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(&m_EnvCubeTexture, cubeLength, 0, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Backup old viewport matrix.sss
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Set device viewport.
        Matrix44 matViewportCubeMap;
        Matrix44Viewport(matViewportCubeMap, 0, 0, targetWidth, targetHeight, 0.0f, 1.0f);
        device->SetViewportMatrix(&matViewportCubeMap);

        // Construct view matrices.
        Matrix44 matViews[Yw3d_CF_NumCubeFaces];
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_X], Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_X], Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Y], Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Y], Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLEquirectangularMap2CubeMapVertexShader* equirectangularMapVertexShader = new DemoPBRIBLEquirectangularMap2CubeMapVertexShader();
        DemoPBRIBLEquirectangularMap2CubeMapPixelShader* equirectangularMapPixelShader = new DemoPBRIBLEquirectangularMap2CubeMapPixelShader();

        // Set render target.
        graphics->SetRenderTarget(rtCubemap);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set cube texture.
        graphics->SetTexture(0, m_EnvEquirectangularTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(equirectangularMapVertexShader);
        graphics->SetPixelShader(equirectangularMapPixelShader);

        // Create cube map face textures.
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            // Render to target and copy to cube face.
            device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

            // This should be from device.
            Matrix44 matWorld;
            Matrix44 matWVP = matWorld * matViews[i] * matProjection;
            device->SetTransform(Yw3d_TS_World, &matWorld);
            device->SetTransform(Yw3d_TS_View, &matViews[i]);
            device->SetTransform(Yw3d_TS_Projection, &matProjection);
            device->SetTransform(Yw3d_TS_WVP, &matWVP);

            // Render this face.
            m_ModelSkySphere->Render(device);

            // Copy pixels from render target to cube face.
            Yw3dSurface* srcSurface = rtCubemap->AcquireColorBuffer();
            Yw3dTexture* dstCubeFace = m_EnvCubeTexture->AcquireCubeFace((Yw3dCubeFaces)i);
            Yw3dSurface* dstSurface = dstCubeFace->AcquireMipLevel(0);
            srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
            YW_SAFE_RELEASE(dstSurface);
            YW_SAFE_RELEASE(dstCubeFace);
            YW_SAFE_RELEASE(srcSurface);
        }

        // Generate mip-map levels, used by prefilter generation.
        m_EnvCubeTexture->GenerateMipSubLevels(0);

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        // Release temp resources.
        YW_SAFE_RELEASE(rtCubemap);
        YW_SAFE_RELEASE(equirectangularMapVertexShader);
        YW_SAFE_RELEASE(equirectangularMapPixelShader);

        return true;
    }

    bool DemoPBRIBL::RenderCubeMapToIrradianceMap()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();

        // Render constants.
        const int32_t targetWidth = 32;
        const int32_t targetHeight = 32;
        const int32_t cubeLength = 32;
        const Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;

        // Create a temp render target.
        Yw3dRenderTarget* rtCubemap = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&rtCubemap, targetWidth, targetHeight, cubeFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Create cube map.
        YW_SAFE_RELEASE(m_IrrandianceCubeTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(&m_IrrandianceCubeTexture, cubeLength, 0, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Backup old viewport matrix.
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Set device viewport.
        Matrix44 matViewportCubeMap;
        Matrix44Viewport(matViewportCubeMap, 0, 0, targetWidth, targetHeight, 0.0f, 1.0f);
        device->SetViewportMatrix(&matViewportCubeMap);

        // Construct view matrices.
        Matrix44 matViews[Yw3d_CF_NumCubeFaces];
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_X], Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_X], Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Y], Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Y], Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLCubeMap2IrrandianceMapVertexShader* irrandianceMapVertexShader = new DemoPBRIBLCubeMap2IrrandianceMapVertexShader();
        DemoPBRIBLCubeMap2IrrandianceMapPixelShader* irrandianceMapPixelShader = new DemoPBRIBLCubeMap2IrrandianceMapPixelShader();

        // Set render target.
        graphics->SetRenderTarget(rtCubemap);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set cube texture.
        graphics->SetTexture(0, m_EnvCubeTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(irrandianceMapVertexShader);
        graphics->SetPixelShader(irrandianceMapPixelShader);

        // Create cube map face textures.
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            // Render to target and copy to cube face.
            device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

            // This should be from device.
            Matrix44 matWorld;
            Matrix44 matWVP = matWorld * matViews[i] * matProjection;
            device->SetTransform(Yw3d_TS_World, &matWorld);
            device->SetTransform(Yw3d_TS_View, &matViews[i]);
            device->SetTransform(Yw3d_TS_Projection, &matProjection);
            device->SetTransform(Yw3d_TS_WVP, &matWVP);

            // Render this face.
            m_ModelSkySphere->Render(device);

            // Copy pixels from render target to cube face.
            Yw3dSurface* srcSurface = rtCubemap->AcquireColorBuffer();
            Yw3dTexture* dstCubeFace = m_IrrandianceCubeTexture->AcquireCubeFace((Yw3dCubeFaces)i);
            Yw3dSurface* dstSurface = dstCubeFace->AcquireMipLevel(0);
            srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
            YW_SAFE_RELEASE(dstSurface);
            YW_SAFE_RELEASE(dstCubeFace);
            YW_SAFE_RELEASE(srcSurface);
        }

        // Generate mip-map levels.
        // (这里没必要生成 mip-map。)
        //m_IrrandianceCubeTexture->GenerateMipSubLevels(0);

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        // Release temp resources.
        YW_SAFE_RELEASE(rtCubemap);
        YW_SAFE_RELEASE(irrandianceMapVertexShader);
        YW_SAFE_RELEASE(irrandianceMapPixelShader);

        return true;
    }

    bool DemoPBRIBL::RenderPrefilterReflectionMap()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();

        // Render constants.
        const int32_t targetWidth = 128;
        const int32_t targetHeight = 128;
        const int32_t cubeLength = 128;
        const Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;

        // Create cube map.
        YW_SAFE_RELEASE(m_PrefilterReflectionCubeTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(&m_PrefilterReflectionCubeTexture, cubeLength, 0, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Generate mip-map levels to store prefilter result by roughness.
        m_PrefilterReflectionCubeTexture->GenerateMipSubLevels(0);

        // Backup old viewport matrix.sss
        const Matrix44* matViewportCurrentPointer;
        device->GetViewportMatrix(matViewportCurrentPointer);
        Matrix44 matViewportCurrent(*matViewportCurrentPointer);

        // Construct view matrices.
        Matrix44 matViews[Yw3d_CF_NumCubeFaces];
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_X], Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_X], Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Y], Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Y], Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Positive_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));
        Matrix44LookAtLH(matViews[Yw3d_CF_Negative_Z], Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));

        // Construct projection matrix.
        Matrix44 matProjection;
        Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

        // Create shader.
        DemoPBRIBLPrefilterReflectionMapVertexShader* prefilterReflectionMapVertexShader = new DemoPBRIBLPrefilterReflectionMapVertexShader();
        DemoPBRIBLPrefilterReflectionMapPixelShader* prefilterReflectionMapPixelShader = new DemoPBRIBLPrefilterReflectionMapPixelShader();

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set cube texture.
        graphics->SetTexture(0, m_EnvCubeTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(prefilterReflectionMapVertexShader);
        graphics->SetPixelShader(prefilterReflectionMapPixelShader);

        uint32_t mipLevels = min(5, m_PrefilterReflectionCubeTexture->GetMipLevels());
        for (uint32_t mip = 0; mip < mipLevels; mip++)
        {
            // Get cube edge size by mip level.
            uint32_t mipEdgeLength = m_PrefilterReflectionCubeTexture->GetEdgeLength(mip);

            // Set device viewport.
            Matrix44 matViewportCubeMap;
            Matrix44Viewport(matViewportCubeMap, 0, 0, mipEdgeLength, mipEdgeLength, 0.0f, 1.0f);
            device->SetViewportMatrix(&matViewportCubeMap);

            // Create a temp render target.
            Yw3dRenderTarget* rtPrefilterMap = nullptr;
            if (YW3D_FAILED(device->CreateRenderTarget(&rtPrefilterMap, mipEdgeLength, mipEdgeLength, cubeFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
            {
                return false;
            }

            // Set render target.
            graphics->SetRenderTarget(rtPrefilterMap);

            // Create cube map face textures.
            for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
            {
                // Render to target and copy to cube face.
                device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

                // This should be from device.
                Matrix44 matWorld;
                Matrix44 matWVP = matWorld * matViews[i] * matProjection;
                device->SetTransform(Yw3d_TS_World, &matWorld);
                device->SetTransform(Yw3d_TS_View, &matViews[i]);
                device->SetTransform(Yw3d_TS_Projection, &matProjection);
                device->SetTransform(Yw3d_TS_WVP, &matWVP);

                float roughness = (float)mip / (float)(mipLevels - 1);
                prefilterReflectionMapPixelShader->SetFloat(0, roughness);

                // Render this face.
                m_ModelSkySphere->Render(device);

                // Copy pixels from render target to cube face.
                Yw3dSurface* srcSurface = rtPrefilterMap->AcquireColorBuffer();
                Yw3dTexture* dstCubeFace = m_PrefilterReflectionCubeTexture->AcquireCubeFace((Yw3dCubeFaces)i);
                Yw3dSurface* dstSurface = dstCubeFace->AcquireMipLevel(mip);
                srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
                YW_SAFE_RELEASE(dstSurface);
                YW_SAFE_RELEASE(dstCubeFace);
                YW_SAFE_RELEASE(srcSurface);
            }

            // Release render target.
            YW_SAFE_RELEASE(rtPrefilterMap);
        }

        // Recovery viewport.
        device->SetViewportMatrix(&matViewportCurrent);

        // Release temp resources.
        YW_SAFE_RELEASE(prefilterReflectionMapVertexShader);
        YW_SAFE_RELEASE(prefilterReflectionMapPixelShader);

        return true;
    }

    bool DemoPBRIBL::RenderPreintegrateBRDFMap()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();

        // Render constants.
        const int32_t targetWidth = 512;
        const int32_t targetHeight = 512;
        const Yw3dFormat targetFormat = Yw3d_FMT_R32G32B32A32F;
        const float fovy = YW_PI / 2.0f;
        const float aspect = 1.0f;
        const float ZNear = 0.1f;
        const float zFar = 10.0f;
		const uint32_t numVertices = 4;

		// Create primitive data.

		// Define vertex format.
		struct VertexElement
		{
			Vector3 position;
			Vector2 uv;
		};

		// Vertex element declaration.
		Yw3dVertexElement VertexDeclaration[] =
		{
			YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 0),
			YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector2, 1)
		};

		// Create vertex format.
		Yw3dVertexFormat* vertexFormat = nullptr;
		if (YW3D_FAILED(device->CreateVertexFormat(&vertexFormat, VertexDeclaration, sizeof(VertexDeclaration))))
		{
			return false;
		}

		// Create vertex buffer.
		Yw3dVertexBuffer* vertexBuffer = nullptr;
		if (YW3D_FAILED(device->CreateVertexBuffer(&vertexBuffer, sizeof(VertexElement) * numVertices)))
		{
			return false;
		}

		// Get vertex buffer pointer.
		VertexElement* vertexElement = nullptr;
		if (YW3D_FAILED(vertexBuffer->GetPointer(0, (void**)&vertexElement)))
		{
			return false;
		}

        // NDC vertex position and texture coordinates.
        vertexElement[0].position = Vector3(-1.0f, -1.0f, 0.0f);
        vertexElement[0].uv = Vector2(0.0f, 1.0f);
		vertexElement[1].position = Vector3(-1.0f, 1.0f, 0.0f);
		vertexElement[1].uv = Vector2(0.0f, 0.0f);
		vertexElement[2].position = Vector3(1.0f, -1.0f, 0.0f);
		vertexElement[2].uv = Vector2(1.0f, 1.0f);
		vertexElement[3].position = Vector3(1.0f, 1.0f, 0.0f);
		vertexElement[3].uv = Vector2(1.0f, 0.0f);

		// Create render target and render texture.

        // Create a temp render target.
        Yw3dRenderTarget* rtBRDFMap = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&rtBRDFMap, targetWidth, targetHeight, targetFormat, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Create brdf texture, no need mipmap.
        YW_SAFE_RELEASE(m_PreintegrateBRDFTexture);
        if (YW3D_FAILED(device->CreateTexture(&m_PreintegrateBRDFTexture, targetWidth, targetHeight, 1, targetFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create brdf texture failed."));
            return false;
        }

		// Backup old viewport matrix.sss
		const Matrix44* matViewportCurrentPointer;
		device->GetViewportMatrix(matViewportCurrentPointer);
		Matrix44 matViewportCurrent(*matViewportCurrentPointer);

		// Set device viewport.
		Matrix44 matViewportCubeMap;
		Matrix44Viewport(matViewportCubeMap, 0, 0, targetWidth, targetHeight, 0.0f, 1.0f);
		device->SetViewportMatrix(&matViewportCubeMap);

		// Construct view matrices.
		Camera* camera = graphics->GetCurrentCamera();
		Matrix44 matViews; // = graphics->GetCurrentCamera()->GetViewMatrix();
		Matrix44LookAtLH(matViews, Vector3(0.0f, 0.0f, -2.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));

		// Construct projection matrix.
		Matrix44 matProjection;
		Matrix44PerspectiveFovLH(matProjection, fovy, aspect, ZNear, zFar);

		// Create shader.
        DemoPBRIBLPreintegrateBRDFMapVertexShader* preintegrateBRDFMapVertexShader = new DemoPBRIBLPreintegrateBRDFMapVertexShader();
        DemoPBRIBLPreintegrateBRDFMapPixelShader* preintegrateBRDFMapPixelShader = new DemoPBRIBLPreintegrateBRDFMapPixelShader();

		// Set render target.
		graphics->SetRenderTarget(rtBRDFMap);

		// Set states.
		graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW); // Yw3d_Cull_CW
		graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

		// Set primitive data.
		// 这种非持久化的数据不可以设置到 Graphics 里面，否则因为在这里函数栈释放后，Graphics 再次自动释放会因为访问无效内存。
		device->SetVertexFormat(vertexFormat);
		device->SetVertexStream(0, vertexBuffer, 0, sizeof(VertexElement));

		// Set vertex and pixel shader.
		device->SetVertexShader(preintegrateBRDFMapVertexShader);
		device->SetPixelShader(preintegrateBRDFMapPixelShader);

		// Render to target and copy to cube face.
		device->Clear(nullptr, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

        // NDC vertices, no need transforms.

		// Render Quad.
        device->DrawPrimitive(Yw3d_PT_TriangleStrip, 0, 2);

        // Copy pixels from render target to cube face.
        Yw3dSurface* srcSurface = rtBRDFMap->AcquireColorBuffer();
        Yw3dSurface* dstSurface = m_PreintegrateBRDFTexture->AcquireMipLevel(0);
        srcSurface->CopyToSurface(nullptr, dstSurface, nullptr, Yw3d_TF_Linear);
        YW_SAFE_RELEASE(dstSurface);
        YW_SAFE_RELEASE(srcSurface);

		 // Recovery viewport.
		device->SetViewportMatrix(&matViewportCurrent);

		YW_SAFE_RELEASE(vertexFormat);
		YW_SAFE_RELEASE(vertexBuffer);
        YW_SAFE_RELEASE(rtBRDFMap);
		YW_SAFE_RELEASE(preintegrateBRDFMapVertexShader);
		YW_SAFE_RELEASE(preintegrateBRDFMapPixelShader);

        // Save brdf texture.
        YwTextureDataConverter::SaveTextureDataToBMPFile("./Resources/brdf", m_PreintegrateBRDFTexture, true);
        YwTextureDataConverter::SaveTextureDataToYWTFile("./Resources/brdf", m_PreintegrateBRDFTexture);

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
        Matrix44Transformation(matWorld, Vector3(100.0f, 100.0f, 100.0f), camera->GetWorldRotation(), Vector3::Zero());

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
        m_ModelSkySphere->Render(device);
    }

    void DemoPBRIBL::RenderPbrModel(int32_t pass)
    {
        // Render pbr model.

        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        DemoPBRIBLCamera* camera = (DemoPBRIBLCamera*)(graphics->GetCurrentCamera());
        DemoPBRIBLApp* app = (DemoPBRIBLApp*)(GetScene()->GetApplication());

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Apply model rotation.
        Matrix44Transformation(matWorld, Vector3(0.6f, 0.6f, 0.6f), camera->GetWorldRotation(), Vector3(0.0f, 0.0f, 0.0f));

        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        //// Set texture.

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

        //graphics->SetTexture(0, m_ModelPBRTexture);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        //graphics->SetTexture(1, m_ModelPBRNormalTexture);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        //graphics->SetTexture(2, m_ModelPBRSpecularTexture);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Update shader parameters.
        m_PbrPixelShader->SetFloat(0, m_Metallic);
        m_PbrPixelShader->SetFloat(1, m_Roughness);
        m_PbrPixelShader->SetVector(0, m_Albedo);
        m_PbrPixelShader->SetVector(1, camera->GetPosition());

        // Set pbr vertex and pixel shader.
        graphics->SetVertexShader(m_PbrVertexShader);
        graphics->SetPixelShader(m_PbrPixelShader);

        // Render model.
        m_ModelPBR->Render(device);
    }
}
