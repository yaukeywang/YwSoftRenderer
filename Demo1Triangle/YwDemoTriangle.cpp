// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle class.

#include "YwDemoTriangle.h"
#include "YwDemoTriangleApp.h"
#include "YwDemoTriangleCamera.h"
#include "YwGraphics.h"
#include "YwScene.h"
#include "YwBaseApplication.h"

namespace yw
{
    // ------------------------------------------------------------------
    // Triangle shader.

    // Triangle vertex shader.
    class DemoTriangleVertexShader : public IYw3dVertexShader
    {
    protected:
        void Execute(const Yw3dShaderRegister* vsShaderInput, Vector4& position, Yw3dShaderRegister* vsShaderOutput)
        {
            // The projection vertex position.
            position = vsShaderInput[0] * (*GetWVPMatrix());

            // Vertex color.
            vsShaderOutput[0] = vsShaderInput[1];
        }

        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister)
        {
            switch (shaderRegister)
            {
            case 0:
                // Vertex color.
                return Yw3d_SRT_Vector2;
            default:
                return Yw3d_SRT_Unused;
            }
        }
    };

    Vector3 ACESToneMapping(Vector3 color)
    {
        const float A = 2.51f;
        const float B = 0.03f;
        const float C = 2.43f;
        const float D = 0.59f;
        const float E = 0.14f;
        //return (color * (A * color + B)) / (color * (C * color + D) + E);

        const Vector3 LeftRes = color * (A * color + B);
        const Vector3 RightRes = color * (C * color + D) + E;
        return Vector3(LeftRes.r / RightRes.r, LeftRes.g / RightRes.g, LeftRes.b / RightRes.b);
    }

    // Triangle pixel shader.
    class DemoTrianglePixelShader : public IYw3dPixelShader
    {
    protected:
        bool MightKillPixels()
        {
            return false;
        }

        bool Execute(const Yw3dShaderRegister* input, Vector4& color, float& depth)
        {
            Vector2 uv = input[0];
            Vector4 hdrColor = tex2D(0, 0, uv);

#if 0
            const float exposure = 0.7f;
            const float gamma = 2.2f;

            // exposure tone mapping
            Vector3 expSrc = -hdrColor * exposure;
            Vector3 mapped = Vector3(1.0f, 1.0f, 1.0f) - Vector3(exp(expSrc.x), exp(expSrc.y), exp(expSrc.z));
            //Vector3 mapped = ACESToneMapping(hdrColor);

            // gamma correction 
            float gammaExp = 1.0f / gamma;
            mapped = Vector3(pow(mapped.r, gammaExp), pow(mapped.g, gammaExp), pow(mapped.b, gammaExp));

            color = mapped;
#else
            const float gammaExp = 1.0f / 2.2f;
            color = Vector4(pow(hdrColor.r, gammaExp), pow(hdrColor.g, gammaExp), pow(hdrColor.b, gammaExp), hdrColor.a);
#endif

            return true;
        }
    };

    // ------------------------------------------------------------------
    // Triangle entity.

    // Vertex element declaration.
    Yw3dVertexElement VertexDeclaration[] =
    {
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector3, 0),
        YW3D_VERTEX_FORMAT_DECL(0, Yw3d_VET_Vector4, 1)
    };

    DemoTriangle::DemoTriangle(Scene* scene) :
        IEntity(scene),
        m_VertexFormat(nullptr),
        m_VertexBuffer(nullptr),
        m_IndexBuffer(nullptr),
        m_ModelTexture(nullptr),
        m_ModelTextureHandle(0),
        m_VertexShader(nullptr),
        m_PixelShader(nullptr),
        m_NumVertices(0),
        m_NumPrimitives(0)
    {
    }

    DemoTriangle::~DemoTriangle()
    {
        YW_SAFE_RELEASE(m_VertexFormat);
        YW_SAFE_RELEASE(m_VertexBuffer);
        YW_SAFE_RELEASE(m_IndexBuffer);
        YW_SAFE_RELEASE(m_ModelTexture);
        YW_SAFE_RELEASE(m_VertexShader);
        YW_SAFE_RELEASE(m_PixelShader);

        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();
        if (m_ModelTextureHandle > 0)
        {
            resManager->UnloadResource(m_ModelTextureHandle);
            m_ModelTextureHandle = 0;
        }
    }

    bool DemoTriangle::Initialize(
        const Vector3& pointA, const Vector2& uvA,
        const Vector3& pointB, const Vector2& uvB,
        const Vector3& pointC, const Vector2& uvC,
        const Vector3& pointD, const Vector2& uvD
    )
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        ResourceManager* resManager = GetScene()->GetApplication()->GetResourceManager();

        // Create vertex format.
        if (YW3D_FAILED(device->CreateVertexFormat(&m_VertexFormat, VertexDeclaration, sizeof(VertexDeclaration))))
        {
            return false;
        }

        // Create vertex data.
        m_NumVertices = 4;
        m_NumPrimitives = 2;

        // Create vertex buffer.
        if (YW3D_FAILED(device->CreateVertexBuffer(&m_VertexBuffer, sizeof(Vertexformat) * m_NumVertices)))
        {
            return false;
        }

        // Create index buffer.
        if (YW3D_FAILED(device->CreateIndexBuffer(&m_IndexBuffer, sizeof(uint16_t) * m_NumPrimitives * 3, Yw3d_FMT_INDEX16)))
        {
            return false;
        }

        // Get vertex buffer pointer.
        Vertexformat* vertexFormat = nullptr;
        if (YW3D_FAILED(m_VertexBuffer->GetPointer(0, (void**)&vertexFormat)))
        {
            return false;
        }

        // Get index buffer pointer.
        uint16_t* indices = nullptr;
        if (YW3D_FAILED(m_IndexBuffer->GetPointer(0, (void**)&indices)))
        {
            return false;
        }

        // Combine data.
        vertexFormat[0].position = pointA;
        vertexFormat[0].uv = uvA;
        vertexFormat[1].position = pointB;
        vertexFormat[1].uv = uvB;
        vertexFormat[2].position = pointC;
        vertexFormat[2].uv = uvC;
        vertexFormat[3].position = pointD;
        vertexFormat[3].uv = uvD;

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;

        // Create vertex and pixel shader.
        m_VertexShader = new DemoTriangleVertexShader();
        m_PixelShader = new DemoTrianglePixelShader();

        m_ModelTextureHandle = resManager->LoadResource("lilienstein_1k.hdr");
        //m_ModelTextureHandle = resManager->LoadResource("bricks_color.bmp");
        if (m_ModelTextureHandle <= 0)
        {
            LOGE(_T("Load resource \"Wood.png\" failed."));
            return false;
        }

        m_ModelTexture = (Yw3dTexture*)resManager->GetResource(m_ModelTextureHandle);
        if (nullptr == m_ModelTexture)
        {
            LOGE(_T("Get resource \"Wood.png\" failed."));
            return false;
        }

        return true;
    }

    bool DemoTriangle::FrameMove()
    {
        return false;
    }

    void DemoTriangle::Render(int32_t pass)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();
        Camera* camera = graphics->GetCurrentCamera();

        Matrix44 matWorld;
        Matrix44Identity(matWorld);
        Matrix44RotationY(matWorld, ((DemoTriangleApp*)(GetScene()->GetApplication()))->GetRotationAngle());
        //Matrix44RotationY(matWorld, 180.0f * DEG_TO_RAD);
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

        graphics->SetVertexFormat(m_VertexFormat);
        graphics->SetVertexStream(0, m_VertexBuffer, 0, sizeof(Vertexformat));
        graphics->SetIndexBuffer(m_IndexBuffer);
        graphics->SetVertexShader(m_VertexShader);
        graphics->SetPixelShader(m_PixelShader);

        //device->SetRenderState(Yw3d_RS_FillMode, Yw3d_Fill_WireFrame);
        device->SetRenderState(Yw3d_RS_CullMode, Yw3d_Cull_None);
        device->DrawIndexedPrimitive(Yw3d_PT_TriangleList, 0, 0, m_NumVertices, 0, m_NumPrimitives);
    }
}
