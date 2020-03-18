// Add by Yaukey at 2020-02-02.
// YW Soft Renderer demo with triangle shader wireframe class.

#include "YwDemoTriangleShaderWireframe.h"
#include "YwDemoTriangleShaderWireframeApp.h"
#include "YwDemoTriangleShaderWireframeCamera.h"
#include "YwDemoTriangleShaderWireframeShader.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Wireframe entity.

    DemoTriangleShaderWireframe::DemoTriangleShaderWireframe(Scene* scene) :
        IEntity(scene),
        m_Model(nullptr),
        m_ModelHandle(0),
        m_VertexShader(nullptr),
        m_TriangleShader(nullptr),
        m_DefaultPixelShader(nullptr),
        m_PatternPixelShader(nullptr),
        m_LineWidth(1.7f),
        m_FadeDistance(50),
        m_PatternPeriod(1.5f)
    {
    }

    DemoTriangleShaderWireframe::~DemoTriangleShaderWireframe()
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
        YW_SAFE_RELEASE(m_TriangleShader);
        YW_SAFE_RELEASE(m_DefaultPixelShader);
        YW_SAFE_RELEASE(m_PatternPixelShader);
    }

    bool DemoTriangleShaderWireframe::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load model and texture.
        m_ModelHandle = resManager->LoadResource("teapot.obj");
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
        m_VertexShader = new DemoTriangleShaderWireframeVertexShader();
        m_TriangleShader = new DemoTriangleShaderWireframeTriangleShader();
        m_DefaultPixelShader = new DemoTriangleShaderWireframeDefaultPixelShader();
        m_PatternPixelShader = new DemoTriangleShaderWireframePatternPixelShader();

        // Set default shader parameters.
        m_LineWidth = 1.5f;
        m_FadeDistance = 50.0f;
        m_PatternPeriod = 1.5f;
        m_FillColor = Vector4(0.1f, 0.2f, 0.4f, 1.0f);
        m_WireColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        m_PatternColor = Vector4(1.0f, 1.0f, 0.5f, 1.0f);

        return true;
    }

    bool DemoTriangleShaderWireframe::FrameMove()
    {
        return false;
    }

    void DemoTriangleShaderWireframe::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        Camera* camera = graphics->GetCurrentCamera();
        DemoTriangleShaderWireframeApp* app = (DemoTriangleShaderWireframeApp*)(GetScene()->GetApplication());

        // Get shader parameters.
        m_LineWidth = app->GetLineWidth();
        m_PatternPeriod = app->GetPatternPeriod();

        // Identity the world.
        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Set rotation.
        Matrix44 matRotate;
        Matrix44RotationY(matRotate, app->GetModelRotationAngle());
        matWorld *= matRotate;

        // Set scale.
        Matrix44 matScale;
        Matrix44Scaling(matScale, 0.3f, 0.3f, 0.3f);
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

        // Set alpha blend state.
        graphics->SetRenderState(Yw3d_RS_AlphaBlendEnable, true);
        graphics->SetRenderState(Yw3d_RS_SrcBlend, Yw3d_Blend_SrcAlpha);
        graphics->SetRenderState(Yw3d_RS_DestBlend, Yw3d_Blend_InvSrcAlpha);

        // Set viewport matrix to triangle shader.
        m_TriangleShader->SetVector(0, Vector4((float)app->GetWindowWidth(), (float)app->GetWindowHeight(), 0.0f, 0.0f));

        // Set default pixel shader parameters.
        m_DefaultPixelShader->SetFloat(0, m_LineWidth);
        m_DefaultPixelShader->SetFloat(1, m_FadeDistance);
        m_DefaultPixelShader->SetFloat(2, m_PatternPeriod);
        m_DefaultPixelShader->SetVector(0, m_FillColor);
        m_DefaultPixelShader->SetVector(1, m_WireColor);
        m_DefaultPixelShader->SetVector(2, m_PatternColor);

        // Set pattern pixel shader parameters.
        m_PatternPixelShader->SetFloat(0, m_LineWidth);
        m_PatternPixelShader->SetFloat(1, m_FadeDistance);
        m_PatternPixelShader->SetFloat(2, m_PatternPeriod);
        m_PatternPixelShader->SetVector(0, m_FillColor);
        m_PatternPixelShader->SetVector(1, m_WireColor);
        m_PatternPixelShader->SetVector(2, m_PatternColor);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetTriangleShader(m_TriangleShader);
        //graphics->SetPixelShader(m_DefaultPixelShader);
        graphics->SetPixelShader(m_PatternPixelShader);
        
        // Render model.
        //graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        m_Model->Render(device);
    }
}
