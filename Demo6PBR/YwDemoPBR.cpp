// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with PBR class.

#include "YwDemoPBR.h"
#include "YwDemoPBRApp.h"
#include "YwDemoPBRCamera.h"
#include "YwDemoPBRShader.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"

namespace yw
{
    // ------------------------------------------------------------------
    // PBR entity.

    DemoPBR::DemoPBR(Scene* scene) :
        IEntity(scene),
        m_Model(nullptr),
        m_ModelTexture(nullptr),
        m_ModelNormalTexture(nullptr),
        m_ModelSpecularTexture(nullptr),
        m_ModelHandle(0),
        m_ModelTextureHandle(0),
        m_ModelNormalTextureHandle(0),
        m_ModelSpecularTextureHandle(0),
        m_VertexShader(nullptr),
        m_PixelShader(nullptr),
        m_Metallic(0.0f),
        m_Smoothness(0.0f),
        m_SmoothnessScale(1.0f)
    {
    }

    DemoPBR::~DemoPBR()
    {
        // Resource should released by resource manager.
        m_Model = nullptr;

        // Release model.
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();
        if (m_ModelHandle > 0)
        {
            resManager->UnloadResource(m_ModelHandle);
        }

        YW_SAFE_RELEASE(m_VertexShader);
        YW_SAFE_RELEASE(m_PixelShader);
    }

    bool DemoPBR::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load model and texture.
        m_ModelHandle = resManager->LoadResource("Lu_Head.obj");
        if (m_ModelHandle <= 0)
        {
            LOGE(_T("Load resource \"Lu_Head.obj\" failed."));
            return false;
        }

        m_ModelTextureHandle = resManager->LoadResource("Lieutenant_head_diffuse.tga");
        if (m_ModelTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"Lieutenant_head_diffuse.tga\" failed."));
            return false;
        }

        m_ModelNormalTextureHandle = resManager->LoadResource("Lieutenant_head_normal.tga");
        if (m_ModelNormalTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"Lieutenant_head_normal.tga\" failed."));
            return false;
        }

        m_ModelSpecularTextureHandle = resManager->LoadResource("Lieutenant_head_specular.tga");
        if (m_ModelSpecularTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"Lieutenant_head_specular.tga\" failed."));
            return false;
        }

        // Get model and texture.
        m_Model = (Model*)resManager->GetResource(m_ModelHandle);
        if (nullptr == m_Model)
        {
            LOGE(_T("Get resource \"Lu_Head.obj\" failed."));
            return false;
        }

        m_ModelTexture = (Yw3dTexture*)resManager->GetResource(m_ModelTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"Lieutenant_head_diffuse.tga\" failed."));
            return false;
        }

        m_ModelNormalTexture = (Yw3dTexture*)resManager->GetResource(m_ModelNormalTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"Lieutenant_head_normal.tga\" failed."));
            return false;
        }

        m_ModelSpecularTexture = (Yw3dTexture*)resManager->GetResource(m_ModelSpecularTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"Lieutenant_head_specular.tga\" failed."));
            return false;
        }

        // Create vertex and pixel shader.
        m_VertexShader = new DemoPBRVertexShader();
        m_PixelShader = new DemoPBRSpecularSetupPixelShader();

        // Initialize environments.
        m_LightDirection.Set(0.0f, -0.5f, 1.0f);
        m_LightColor.Set(1.0f, 0.93f, 0.79f, 1.0f);
        m_AlbedoColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
        m_SpecularColor.Set(0.2f, 0.2f, 0.2f, 1.0f);
        m_Metallic = 0.5f;
        m_Smoothness = 0.5f;
        m_SmoothnessScale = 1.0f;

        return true;
    }

    bool DemoPBR::FrameMove()
    {
        return false;
    }

    void DemoPBR::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        DemoPBRCamera* camera = (DemoPBRCamera*)(graphics->GetCurrentCamera());
        DemoPBRApp* app = (DemoPBRApp*)(GetScene()->GetApplication());

        // Update light direction.
        //Matrix44 lightRotate;
        //Matrix44RotationY(lightRotate, ((DemoPBRApp*)(GetScene()->GetApplication()))->GetLightRotationAngle());
        //Vector3 lightDir = Vector4(m_LightDirection) * lightRotate;
        Vector3 lightDir = m_LightDirection;

        // Get material parameters.
        m_Metallic = 0.0f; //((DemoPBRApp*)(GetScene()->GetApplication()))->GetMetallic();
        m_Smoothness = 0.321f; //((DemoPBRApp*)(GetScene()->GetApplication()))->GetSmoothness();
        m_SmoothnessScale = 1.0f;

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Apply model rotation.
        //Quaternion iden;
        Matrix44Transformation(matWorld, Vector3(0.025f, 0.025f, 0.025f), camera->GetViewRotation(), Vector3(0.0f, 0.0f, 0.0f));

        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Set texture.
        graphics->SetTexture(0, m_ModelTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(1, m_ModelNormalTexture);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(2, m_ModelSpecularTexture);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressU, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_AddressV, Yw3d_TA_Wrap);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(2, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Update shader parameters.
        m_PixelShader->SetVector(0, lightDir);
        m_PixelShader->SetVector(1, m_LightColor * 1.5f);
        m_PixelShader->SetVector(2, m_AlbedoColor);
        m_PixelShader->SetVector(3, m_SpecularColor);
        m_PixelShader->SetVector(4, camera->GetForward());
        m_PixelShader->SetFloat(0, m_Metallic);
        m_PixelShader->SetFloat(1, m_Smoothness);
        m_PixelShader->SetFloat(2, m_SmoothnessScale);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetPixelShader(m_PixelShader);

        // Render model.
        //graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        m_Model->Render(device);
    }
}
