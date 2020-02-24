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
        m_ModelHandle(0),
        m_VertexShader(nullptr),
        m_PixelShader(nullptr)
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
        m_ModelHandle = resManager->LoadResource("HydrantPart.obj");
        if (m_ModelHandle <= 0)
        {
            LOGE(_T("Load resource \"teapot.obj\" failed."));
            return false;
        }

        // Get model and texture.
        m_Model = (Model*)resManager->GetResource(m_ModelHandle);
        if (nullptr == m_Model)
        {
            LOGE(_T("Get resource \"teapot.obj\" failed."));
            return false;
        }

        // Create vertex and pixel shader.
        m_VertexShader = new DemoPBRVertexShader();
        m_PixelShader = new DemoPBRPixelShader();

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
        Camera* camera = graphics->GetCurrentCamera();

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Set rotation.
        Matrix44 matRotate;
        Matrix44RotationY(matRotate, ((DemoPBRApp*)(GetScene()->GetApplication()))->GetModelRotationAngle());
        matWorld *= matRotate;

        // Set scale.
        Matrix44 matScale;
        Matrix44Scaling(matScale, 0.035f, 0.035f, 0.035f);
        matWorld *= matScale;

        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Update shader property.
        Matrix44 worldInverse;
        Matrix44Inverse(worldInverse, camera->GetWorldMatrix());

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetPixelShader(m_PixelShader);
        
        // Render model.
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        m_Model->Render(device);
    }
}
