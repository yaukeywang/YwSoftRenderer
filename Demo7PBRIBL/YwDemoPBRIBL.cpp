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

namespace yw
{
    // ------------------------------------------------------------------
    // PBR entity.

    DemoPBRIBL::DemoPBRIBL(Scene* scene) :
        IEntity(scene),
        m_ModelSkySphere(nullptr),
        //m_ModelPBR(nullptr),
        m_ModelSkySphereTexture(nullptr),
        //m_ModelPBRTexture(nullptr),
        //m_ModelPBRNormalTexture(nullptr),
        //m_ModelPBRSpecularTexture(nullptr),
        m_ModelSkySphereHandle(0),
        //m_ModelPBRHandle(0),
        m_ModelSkySphereTextureHandle(0),
        //m_ModelPBRTextureHandle(0),
        //m_ModelPBRNormalTextureHandle(0),
        //m_ModelPBRSpecularTextureHandle(0),
        m_SkyVertexShader(nullptr),
        m_SkyPixelShader(nullptr)
        //m_PbrVertexShader(nullptr),
        //m_PbrPixelShader(nullptr),
        //m_Metallic(0.0f),
        //m_Smoothness(0.0f),
        //m_SmoothnessScale(1.0f)
    {
    }

    DemoPBRIBL::~DemoPBRIBL()
    {
        // Resource should released by resource manager.
        m_ModelSkySphere = nullptr;
        //m_ModelPBR = nullptr;
        m_ModelSkySphereTexture = nullptr;
        //m_ModelPBRTexture = nullptr;
        //m_ModelPBRNormalTexture = nullptr;
        //m_ModelPBRSpecularTexture = nullptr;

        // Get resource manager and release all resources.
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        if (m_ModelSkySphereHandle > 0)
        {
            resManager->UnloadResource(m_ModelSkySphereHandle);
        }

        //if (m_ModelPBRHandle > 0)
        //{
        //    resManager->UnloadResource(m_ModelPBRHandle);
        //}

        if (m_ModelSkySphereTextureHandle > 0)
        {
            resManager->UnloadResource(m_ModelSkySphereTextureHandle);
        }

        //if (m_ModelPBRTextureHandle > 0)
        //{
        //    resManager->UnloadResource(m_ModelPBRTextureHandle);
        //}

        //if (m_ModelPBRNormalTextureHandle > 0)
        //{
        //    resManager->UnloadResource(m_ModelPBRNormalTextureHandle);
        //}

        //if (m_ModelPBRSpecularTextureHandle > 0)
        //{
        //    resManager->UnloadResource(m_ModelPBRSpecularTextureHandle);
        //}

        YW_SAFE_RELEASE(m_SkyVertexShader);
        YW_SAFE_RELEASE(m_SkyPixelShader);
        //YW_SAFE_RELEASE(m_PbrVertexShader);
        //YW_SAFE_RELEASE(m_PbrPixelShader);
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

        //m_ModelPBRHandle = resManager->LoadResource("Lu_Head.obj");
        //if (m_ModelPBRHandle <= 0)
        //{
        //    LOGE(_T("Load resource \"Lu_Head.obj\" failed."));
        //    return false;
        //}

        m_ModelSkySphereTextureHandle = resManager->LoadResource("newport_loft.hdr");
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

        //m_ModelPBR = (Model*)resManager->GetResource(m_ModelPBRHandle);
        //if (nullptr == m_ModelPBR)
        //{
        //    LOGE(_T("Get resource \"Lu_Head.obj\" failed."));
        //    return false;
        //}

        m_ModelSkySphereTexture = (Yw3dCubeTexture*)resManager->GetResource(m_ModelSkySphereTextureHandle);
        if (nullptr == m_ModelSkySphereTexture)
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

        // Create vertex and pixel shader.
        m_SkyVertexShader = new DemoPBRIBLSkyVertexShader();
        m_SkyPixelShader = new DemoPBRIBLSkyPixelShader();
        //m_PbrVertexShader = new DemoPBRIBLVertexShader();
        //m_PbrPixelShader = new DemoPBRIBLSpecularSetupPixelShader();

        //// Initialize environments.
        //m_LightDirection.Set(0.0f, -0.5f, 1.0f);
        //m_LightColor.Set(1.0f, 0.93f, 0.79f, 1.0f);
        //m_AlbedoColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
        //m_SpecularColor.Set(0.2f, 0.2f, 0.2f, 1.0f);
        //m_Metallic = 0.5f;
        //m_Smoothness = 0.5f;
        //m_SmoothnessScale = 1.0f;

        return true;
    }

    bool DemoPBRIBL::FrameMove()
    {
        return false;
    }

    void DemoPBRIBL::Render(int32_t pass)
    {
        // Render the entire scene.
        RenderSky(pass);
        RenderPbrModel(pass);
    }

    bool DemoPBRIBL::RenderEquirectangularMapToCubeMap()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();

        // Create a temp render target.
        Yw3dRenderTarget* cubeRenderTarget = nullptr;
        if (YW3D_FAILED(device->CreateRenderTarget(&cubeRenderTarget, 512, 512, Yw3d_FMT_R32G32B32A32F, Yw3d_FMT_R32F, Yw3d_FMT_R32F)))
        {
            return false;
        }

        // Backup old render target.
        Yw3dRenderTarget* curRenderTarget = device->AcquireRenderTarget();
        device->SetRenderTarget(cubeRenderTarget);

        YW_SAFE_RELEASE(curRenderTarget);

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
        Matrix44Transformation(matWorld, Vector3(10.0f, 10.0f, 10.0f), camera->GetWorldRotation(), camera->GetPosition());

        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Set states.
        graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CW);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        // Set sky texture.
        graphics->SetTexture(0, m_ModelSkySphereTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
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

        //// Update light direction.
        ////Matrix44 lightRotate;
        ////Matrix44RotationY(lightRotate, ((DemoPBRIBLApp*)(GetScene()->GetApplication()))->GetLightRotationAngle());
        ////Vector3 lightDir = Vector4(m_LightDirection) * lightRotate;
        //Vector3 lightDir = m_LightDirection;

        //// Get material parameters.
        //m_Metallic = 0.0f; //((DemoPBRIBLApp*)(GetScene()->GetApplication()))->GetMetallic();
        //m_Smoothness = 0.321f; //((DemoPBRIBLApp*)(GetScene()->GetApplication()))->GetSmoothness();
        //m_SmoothnessScale = 1.0f;

        //Matrix44 matWorld;
        //Matrix44Identity(matWorld);

        //// Apply model rotation.
        //Matrix44Transformation(matWorld, Vector3(0.025f, 0.025f, 0.025f), camera->GetWorldRotation(), Vector3(0.0f, 0.0f, 0.0f));

        //// Set world transform to camera.
        //camera->SetWorldMatrix(matWorld);

        //// This should be from device.
        //Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        //device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        //device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        //device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        //device->SetTransform(Yw3d_TS_WVP, &matProjection);

        //// Set states.
        //graphics->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_CCW);
        //graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);

        //// Set texture.
        //graphics->SetTexture(0, m_ModelPBRTexture);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        //graphics->SetTexture(1, m_ModelPBRNormalTexture);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(1, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        //graphics->SetTexture(2, m_ModelPBRSpecularTexture);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        //graphics->SetTextureSamplerState(2, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        //// Update shader parameters.
        //m_PbrPixelShader->SetVector(0, lightDir);
        //m_PbrPixelShader->SetVector(1, m_LightColor * 1.5f);
        //m_PbrPixelShader->SetVector(2, m_AlbedoColor);
        //m_PbrPixelShader->SetVector(3, m_SpecularColor);
        //m_PbrPixelShader->SetVector(4, camera->GetForward());
        //m_PbrPixelShader->SetFloat(0, m_Metallic);
        //m_PbrPixelShader->SetFloat(1, m_Smoothness);
        //m_PbrPixelShader->SetFloat(2, m_SmoothnessScale);

        //// Set pbr vertex and pixel shader.
        //graphics->SetVertexShader(m_PbrVertexShader);
        //graphics->SetPixelShader(m_PbrPixelShader);

        //// Render model.
        //m_ModelPBR->Render(device);
    }
}
