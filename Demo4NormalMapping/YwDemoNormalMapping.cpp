// Add by Yaukey at 2020-01-23.
// YW Soft Renderer demo with normal-mapping class.

#include "YwDemoNormalMapping.h"
#include "YwDemoNormalMappingApp.h"
#include "YwDemoNormalMappingCamera.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"
#include "YwModel.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Normal-Mapping shader.

    // Normal-Mapping vertex shader.
    // Vertex input format:
    // 0 - Vector3 position;
    // 1 - Vector3 normal;
    // 2 - Vector4 tangent;
    // 3 - Vector4 color;
    // 4 - Vector2 texcoord;
    // 5 - Vector2 texcoord2;
    class DemoNormalMappingVertexShader : public IYw3dVertexShader
    {
    protected:
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
        {
            // The projection vertex position.
            position = vsShaderInput[0] * (*GetWVPMatrix());

            // Model normal.
            //vsShaderOutput[0] = vsShaderInput[1];

            // Get light and view direction.
            const float4& lightDir = GetVector(0);
            const float4& viewDir = GetVector(1);

            // Get world inverse matrix.
            const float44& worldInverse = GetMatrix(0);

            // Transform light and view direction into model space.
            float4 modelLightDir = lightDir * worldInverse;
            float4 modelViewDir = viewDir * worldInverse;

            // Other vertex attribute.
            vsShaderOutput[0] = -modelLightDir;
            vsShaderOutput[1] = -modelViewDir;

            // Texcoord.
            vsShaderOutput[2] = vsShaderInput[4];

            // TBN.
            const float3 normal = vsShaderInput[1];
            const float4 tangent = vsShaderInput[2];
            const float3 binormal = cross(normal, float3(tangent)) * tangent.w;
            vsShaderOutput[3] = float3(tangent);
            vsShaderOutput[4] = binormal;
            vsShaderOutput[5] = normal;
        }

        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister)
        {
            switch (shaderRegister)
            {
            case 0:
                return Yw3d_SRT_Vector3; // Light direction.
            case 1:
                return Yw3d_SRT_Vector3; // View direction.
            case 2:
                return Yw3d_SRT_Vector2; // Vertex texcoord.
            case 3:
                return Yw3d_SRT_Vector3; // Tangent of TBN..
            case 4:
                return Yw3d_SRT_Vector3; // Bi-Normal of TBN.
            case 5:
                return Yw3d_SRT_Vector3; // Normal of TBN.
            default:
                return Yw3d_SRT_Unused;
            }
        }
    };

    // Normal-Mapping pixel shader.
    class DemoNormalMappingPixelShader : public IYw3dPixelShader
    {
    protected:
        bool MightKillPixels()
        {
            return false;
        }

        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
        {
            // Form TBN matrix from tangent space to model space.
            const float3 tangent = normalize(float3(input[3]));
            const float3 binormal = normalize(float3(input[4]));
            const float3 normal = normalize(float3(input[5]));
            float33 TBN = float33(
                tangent.x, tangent.y, tangent.z,
                binormal.x, binormal.y, binormal.z,
                normal.x, normal.y, normal.z
            );

            // Sample main texture.
            float2 texCoord = input[2];
            float4 texColor = tex2D(2, 0, texCoord);

            // Sample normal texture.
            float4 normalTexColor = tex2D(2, 1, texCoord);;
            float3 normalTangent = normalTexColor * 2 - float4(1.0f, 1.0f, 1.0f, 0.0f);
            float3 normalModel = normalize(normalTangent * TBN);

            float3 modelLightDir = float3(input[0]).Normalize();
            float3 modelViewDir = float3(input[1]).Normalize();

            // Get half vector.
            float3 h = (modelLightDir + modelViewDir);

            // Get diffuse.
            float diff = max(0.0f, dot(normalModel, modelLightDir));

            // Get N dot H.
            float nh = max(0.0f, dot(normalModel, h));

            // Get specular.
            float specular = GetFloat(0);
            float gloss = GetFloat(1);
            float spec = pow(nh, specular * 128.0f) * gloss;

            // Get light color.
            float4 lightColor = GetVector(0);
            float4 albedo = GetVector(1);
            float4 specColor = GetVector(2);

            float4 c = albedo * texColor + lightColor * diff * texColor * 1.3f + lightColor * specColor * spec * 1.8f;
            c.a = 1.0f;

            color = c;
            return true;
        }
    };

    // ------------------------------------------------------------------
    // Normal-Mapping entity.

    DemoNormalMapping::DemoNormalMapping(Scene* scene) :
        IEntity(scene),
        m_Model(nullptr),
        m_ModelTexture(nullptr),
        m_ModelNormalTexture(nullptr),
        m_ModelHandle(0),
        m_ModelTextureHandle(0),
        m_ModelNormalTextureHandle(0),
        m_VertexShader(nullptr),
        m_PixelShader(nullptr),
        m_Specular(0.08f),
        m_Gloss(0.01f)
    {
    }

    DemoNormalMapping::~DemoNormalMapping()
    {
        // Resource should released by resource manager.
        m_Model = nullptr;

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

        if (m_ModelNormalTextureHandle > 0)
        {
            resManager->UnloadResource(m_ModelNormalTextureHandle);
        }

        YW_SAFE_RELEASE(m_VertexShader);
        YW_SAFE_RELEASE(m_PixelShader);
    }

    bool DemoNormalMapping::Initialize()
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Load model and texture.
        m_ModelHandle = resManager->LoadResource("Cylinder.obj");
        if (m_ModelHandle <= 0)
        {
            LOGE(_T("Load resource \"Cylinder.obj\" failed."));
            return false;
        }

        m_ModelTextureHandle = resManager->LoadResource("bricks_color.bmp");
        if (m_ModelTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"bricks_color.bmp\" failed."));
            return false;
        }

        m_ModelNormalTextureHandle = resManager->LoadResource("bricks_nmap.bmp");
        if (m_ModelNormalTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"bricks_nmap.bmp\" failed."));
            return false;
        }

        // Get model and texture.
        m_Model = (Model*)resManager->GetResource(m_ModelHandle);
        if (nullptr == m_Model)
        {
            LOGE(_T("Get resource \"Cylinder.obj\" failed."));
            return false;
        }

        m_ModelTexture = (Yw3dTexture*)resManager->GetResource(m_ModelTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"bricks_color.bmp\" failed."));
            return false;
        }

        m_ModelNormalTexture = (Yw3dTexture*)resManager->GetResource(m_ModelNormalTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"bricks_nmap.bmp\" failed."));
            return false;
        }

        // Create vertex and pixel shader.
        m_VertexShader = new DemoNormalMappingVertexShader();
        m_PixelShader = new DemoNormalMappingPixelShader();

        // Initialize environments.
        m_LightDirection.Set(0.0f, -0.65f, 1.0f);
        m_LightColor.Set(0.6f, 0.6f, 0.6f, 1.0f);
        m_AlbedoColor.Set(0.3f, 0.3f, 0.3f, 1.0f);
        m_SpecularColor.Set(0.35f, 0.4f, 0.24f, 1.0f);
        m_Specular = 0.08f;
        m_Gloss = 0.002f;

        return true;
    }

    bool DemoNormalMapping::FrameMove()
    {
        return false;
    }

    void DemoNormalMapping::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        Camera* camera = graphics->GetCurrentCamera();

        Matrix44 matWorld;
        Matrix44Identity(matWorld);

        // Set rotation.
        Matrix44 matRotate;
        Matrix44RotationY(matRotate, ((DemoNormalMappingApp*)(GetScene()->GetApplication()))->GetModelRotationAngle());
        matWorld *= matRotate;

        // Set scale.
        Matrix44 matScale;
        Matrix44Scaling(matScale, 0.5f, 0.5f, 0.5f);
        matWorld *= matScale;

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
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(0, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        graphics->SetTexture(1, m_ModelNormalTexture);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressU, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_AddressV, Yw3d_TA_Clamp);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MinFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MagFilter, Yw3d_TF_Linear);
        graphics->SetTextureSamplerState(1, Yw3d_TSS_MipFilter, Yw3d_TF_Linear);

        // Update shader property.
        Matrix44 worldInverse;
        Matrix44Inverse(worldInverse, camera->GetWorldMatrix());

        Matrix44 lightRotate;
        Matrix44RotationY(lightRotate, ((DemoNormalMappingApp*)(GetScene()->GetApplication()))->GetLightRotationAngle());
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
