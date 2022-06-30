// Add by Yaukey at 2019-09-19.
// YW Soft Renderer demo model class.

#include "YwDemoModel.h"
#include "YwDemoModelApp.h"
#include "YwDemoModelCamera.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Model shader.

    // Vertex input format:
    // 0 - Vector3 position;
    // 1 - Vector3 normal;
    // 2 - Vector4 tangent;
    // 3 - Vector4 color;
    // 4 - Vector2 texcoord;
    // 5 - Vector2 texcoord2;

    // Model vertex shader.
    class DemoModelVertexShader : public IYw3dVertexShader
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
                return Yw3d_SRT_Vector4;    // Vertex tangent.
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

    // Model wire frame pixel shader.
    class DemoModelWireframePixelShader : public IYw3dPixelShader
    {
    protected:
        bool MightKillPixels()
        {
            return false;
        }

        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
        {
            //color = input[0];
            color = color = Vector4(0.0f, 0.33f, 0.76f, 1.0f);;
            return true;
        }
    };

    // Model pixel shader.
    class DemoModelPixelShader : public IYw3dPixelShader
    {
    protected:
        bool MightKillPixels()
        {
            return false;
        }

        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
        {
            Vector4 vDdx, vDdy;
            GetPartialDerivatives(3, vDdx, vDdy);

            Vector2 texCoord = input[3];
            Vector4 texColor;
            SampleTexture(texColor, 0, texCoord.x, texCoord.y, 0.0f, 0.0f, &vDdx, &vDdy);

            color = texColor;
            return true;
        }
    };

    // ------------------------------------------------------------------
    // Model entity.

    DemoModel::DemoModel(Scene* scene) : 
        IEntity(scene),
        m_Model(nullptr),
        m_ModelTexture(nullptr),
        m_ModelHandle(0),
        m_ModelTextureHandle(0),
        m_VertexShader(nullptr),
        m_PixelShaderWireframe(nullptr),
        m_PixelShader(nullptr)
    {
    }

    DemoModel::~DemoModel()
    {
        // Resource should released by resource manager.
        m_Model = nullptr;
        m_ModelTexture = nullptr;

        // Release model.
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();
        if (m_ModelHandle > 0)
        {
            resManager->UnloadResource(m_ModelHandle);
        }

        // Release texture.
        if (m_ModelTextureHandle > 0)
        {
            resManager->UnloadResource(m_ModelTextureHandle);
        }

        YW_SAFE_RELEASE(m_VertexShader);
        YW_SAFE_RELEASE(m_PixelShaderWireframe);
        YW_SAFE_RELEASE(m_PixelShader);
    }

    bool DemoModel::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load model and texture.
        m_ModelHandle = resManager->LoadResource("SM_Chair.obj");
        if (m_ModelHandle <= 0)
        {
            LOGE(_T("Load resource \"SM_Chair.obj\" failed."));
            return false;
        }

        m_ModelTextureHandle = resManager->LoadResource("Wood.png");
        if (m_ModelTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"Wood.png\" failed."));
            return false;
        }

        // Get model and texture.
        m_Model = (Model*)resManager->GetResource(m_ModelHandle);
        if (nullptr == m_Model)
        {
            LOGE(_T("Get resource \"SM_Chair.obj\" failed."));
            return false;
        }

        m_ModelTexture = (Yw3dTexture*)resManager->GetResource(m_ModelTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"Wood.png\" failed."));
            return false;
        }

        // Create vertex and pixel shader.
        m_VertexShader = new DemoModelVertexShader();
        m_PixelShaderWireframe = new DemoModelWireframePixelShader();
        m_PixelShader = new DemoModelPixelShader();

        return true;
    }

    bool DemoModel::FrameMove()
    {
        return false;
    }

    void DemoModel::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        Camera* camera = graphics->GetCurrentCamera();

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Set scale.
        Matrix44 matScale;
        Matrix44Scaling(matScale, 0.005f, 0.005f, 0.005f);
        matWorld *= matScale;

        // Set rotation.
        Matrix44 matRotate;
        Matrix44RotationY(matRotate, ((DemoModelApp*)(GetScene()->GetApplication()))->GetRotationAngle());
        matWorld *= matRotate;

        // Prepare to render left model.
        Matrix44 leftOffset;
        Matrix44Translation(leftOffset, -0.3f, 0.0f, 0.0f);

        // Get final left model transform matrix.
        Matrix44 leftMatrix = matWorld * leftOffset;

        // Set world transform to camera.
        camera->SetWorldMatrix(leftMatrix);

        // This should be from device.
        Matrix44 matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Set texture.
        graphics->SetTexture(0, m_ModelTexture);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetPixelShader(m_PixelShaderWireframe);
        
        // Render left model.
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        m_Model->Render(device);

        // Prepare to render right model.
        Matrix44 rightOffset;
        Matrix44Translation(rightOffset, 0.3f, 0.0f, 0.0f);

        // Get final right model transform matrix.
        Matrix44 rightMatrix = matWorld * rightOffset;

        // Set world transform to camera.
        camera->SetWorldMatrix(rightMatrix);

        // This should be from device.
        matProjection = camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix();
        device->SetTransform(Yw3d_TS_World, &camera->GetWorldMatrix());
        device->SetTransform(Yw3d_TS_View, &camera->GetViewMatrix());
        device->SetTransform(Yw3d_TS_Projection, &camera->GetProjectionMatrix());
        device->SetTransform(Yw3d_TS_WVP, &matProjection);

        // Render right model.
        graphics->SetPixelShader(m_PixelShader);
        graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_Solid);
        m_Model->Render(device);
    }
}
