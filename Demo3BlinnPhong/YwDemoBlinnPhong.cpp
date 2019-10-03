// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo with blinn-phong class.

#include "YwDemoBlinnPhong.h"
#include "YwDemoBlinnPhongApp.h"
#include "YwDemoBlinnPhongCamera.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Model shader.

    // Model vertex shader.
    class DemoBlinnPhongVertexShader : public IYw3dVertexShader
    {
    protected:
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
        {
            // The projection vertex position.
            position = vsShaderInput[0] * (*GetWVPMatrix());

            // Vertex attribute.
            vsShaderOutput[0] = vsShaderInput[1];
            vsShaderOutput[1] = vsShaderInput[2];
            vsShaderOutput[2] = vsShaderInput[3];
            vsShaderOutput[3] = vsShaderInput[4];
            vsShaderOutput[4] = vsShaderInput[5];
        }

        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister)
        {
            switch (shaderRegister)
            {
            case 0:
                return Yw3d_SRT_Vector3;    // Vertex normal.
            case 1:
                return Yw3d_SRT_Vector3;    // Vertex tangent.
            case 2:
                return Yw3d_SRT_Vector4;    // Vertex color.
            case 3:
                return Yw3d_SRT_Vector2;    // Vertex texcoord.
            case 4:
                return Yw3d_SRT_Vector2;    // Vertex texcoord2.
            default:
                return Yw3d_SRT_Unused;
            }
        }
    };

    // Model pixel shader.
    class DemoBlinnPhongPixelShader : public IYw3dPixelShader
    {
    protected:
        bool MightKillPixels()
        {
            return false;
        }

        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
        {
            color = Vector4(0.65f, 0.8f, 0.24f, 1.0f);
            return true;
        }
    };

    // ------------------------------------------------------------------
    // Model entity.

    DemoBlinnPhong::DemoBlinnPhong(Scene* scene) :
        IEntity(scene),
        m_Model(nullptr),
        m_ModelHandle(0),
        m_VertexShader(nullptr),
        m_PixelShader(nullptr)
    {
    }

    DemoBlinnPhong::~DemoBlinnPhong()
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

    bool DemoBlinnPhong::Initialize()
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
        m_VertexShader = new DemoBlinnPhongVertexShader();
        m_PixelShader = new DemoBlinnPhongPixelShader();

        return true;
    }

    bool DemoBlinnPhong::FrameMove()
    {
        return false;
    }

    void DemoBlinnPhong::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        Camera* camera = graphics->GetCurrentCamera();

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Set scale.
        Matrix44 matScale;
        Matrix44Scaling(matScale, 0.1f, 0.1f, 0.1f);
        matWorld *= matScale;

        // Set rotation.
        Matrix44 matRotate;
        Matrix44RotationY(matRotate, ((DemoBlinnPhongApp*)(GetScene()->GetApplication()))->GetRotationAngle());
        matWorld *= matRotate;

        // Set world transform to camera.
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetPixelShader(m_PixelShader);
        
        // Render model.
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        m_Model->Render(device);
    }
}
