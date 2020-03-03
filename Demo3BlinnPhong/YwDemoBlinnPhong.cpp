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
    // Blinn-Phong shader.

    // Model vertex shader.
    // Vertex input format:
    // 0 - Vector3 position;
    // 1 - Vector3 normal;
    // 2 - Vector4 tangent;
    // 3 - Vector4 color;
    // 4 - Vector2 texcoord;
    // 5 - Vector2 texcoord2;
    class DemoBlinnPhongVertexShader : public IYw3dVertexShader
    {
    protected:
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
        {
            // The projection vertex position.
            position = vsShaderInput[0] * (*GetWVPMatrix());

            // Model normal.
            vsShaderOutput[0] = vsShaderInput[1];

            // Get light and view direction.
            const float4& lightDir = GetVector(0);
            const float4& viewDir = GetVector(1);

            // Get world inverse matrix.
            const float44& worldInverse = GetMatrix(0);

            // Transform light and view direction into model space.
            float4 modelLightDir = lightDir * worldInverse;
            float4 modelViewDir = viewDir * worldInverse;

            // Other vertex attribute.
            vsShaderOutput[1] = -modelLightDir;
            vsShaderOutput[2] = -modelViewDir;

            // Other vertex attribute.
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
            float3 normal = normalize(float3(input[0]));
            float3 modelLightDir = normalize(float3(input[1]));
            float3 modelViewDir = normalize(float3(input[2]));

            // Get half vector.
            float3 h = (modelLightDir + modelViewDir);

            // Get diffuse.
            float diff = max(0.0f, dot(normal, modelLightDir));

            // Get N dot H.
            float nh = max(0.0f, dot(normal, h));

            // Get specular.
            float specular = GetFloat(0);
            float gloss = GetFloat(1);
            float spec = pow(nh, specular * 128.0f) * gloss;

            float4 lightColor = GetVector(0);
            float4 albedo = GetVector(1);
            float4 specColor = GetVector(2);

            float4 c = albedo + lightColor * diff + lightColor * specColor * spec;
            c.a = 1.0f;

            color = c;
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
        m_PixelShader(nullptr),
        m_Specular(0.08f),
        m_Gloss(0.01f)
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

        // Initialize environments.
        m_LightDirection.Set(0.5f, 0.0f, 1.0f);
        m_LightColor.Set(0.3f, 0.5f, 0.3f, 1.0f);
        m_AlbedoColor.Set(0.1f, 0.1f, 0.1f, 1.0f);
        m_SpecularColor.Set(0.65f, 0.8f, 0.24f, 1.0f);
        m_Specular = 0.08f;
        m_Gloss = 0.002f;

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
        Matrix44RotationY(matRotate, ((DemoBlinnPhongApp*)(GetScene()->GetApplication()))->GetModelRotationAngle());
        matWorld *= matRotate;

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

        Matrix44 lightRotate;
        Matrix44RotationY(lightRotate, ((DemoBlinnPhongApp*)(GetScene()->GetApplication()))->GetLightRotationAngle());
        Vector3 lightDir = Vector4(m_LightDirection) * lightRotate;

        m_VertexShader->SetMatrix(0, worldInverse);
        m_VertexShader->SetVector(0, lightDir);
        m_VertexShader->SetVector(1, camera->GetForward());

        m_PixelShader->SetVector(0, m_LightColor);
        m_PixelShader->SetVector(1, m_AlbedoColor);
        m_PixelShader->SetVector(2, m_SpecularColor);
        m_PixelShader->SetFloat(0, m_Specular);
        m_PixelShader->SetFloat(1, m_Gloss);

        // Set vertex and pixel shader.
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetPixelShader(m_PixelShader);
        
        // Render model.
        //graphics->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        m_Model->Render(device);
    }
}
