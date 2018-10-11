// Add by Yaukey at 2018-10-10.
// YW Soft Renderer demo triangle class.

#include "YwDemoTriangle.h"
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
            position = vsShaderInput[0] * GetMatrix(Yw3d_SC_Matrix_WVP);

            // Vertex color.
            vsShaderOutput[0] = vsShaderInput[1];
        }

        Yw3dShaderRegisterType GetOutputRegisters(uint32_t shaderRegister)
        {
            switch (shaderRegister)
            {
            case 0:
                // Vertex color.
                return Yw3d_SRT_Vector4;
            default:
                return Yw3d_SRT_Unused;
            }
        }
    };

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
            color = input[0];
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
        YW_SAFE_RELEASE(m_VertexShader);
        YW_SAFE_RELEASE(m_PixelShader);
    }

    bool DemoTriangle::Initialize(const Vector3& pointA, const Vector3& pointB, const Vector3& pointC, const Vector4& color)
    {
        // Get graphics and device.
        Graphics* graphics = GetScene()->GetApplication()->GetGraphics();
        Yw3dDevice* device = graphics->GetYw3dDevice();

        // Create vertex format.
        if (YW3D_FAILED(device->CreateVertexFormat(&m_VertexFormat, VertexDeclaration, sizeof(VertexDeclaration))))
        {
            return false;
        }

        // Create vertex data.
        m_NumVertices = 3;
        m_NumPrimitives = 1;

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
        vertexFormat[0].color = color;
        vertexFormat[1].position = pointB;
        vertexFormat[1].color = color;
        vertexFormat[2].position = pointC;
        vertexFormat[2].color = color;

        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

        // Create vertex and pixel shader.
        m_VertexShader = new DemoTriangleVertexShader();
        m_PixelShader = new DemoTrianglePixelShader();

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
        camera->SetWorldMatrix(matWorld);

        // This should be from device.
        m_VertexShader->SetMatrix(Yw3d_SC_Matrix_World, camera->GetWorldMatrix());
        m_VertexShader->SetMatrix(Yw3d_SC_Matrix_View, camera->GetViewMatrix());
        m_VertexShader->SetMatrix(Yw3d_SC_Matrix_Projection, camera->GetProjectionMatrix());
        m_VertexShader->SetMatrix(Yw3d_SC_Matrix_WVP, camera->GetWorldMatrix() * camera->GetViewMatrix() * camera->GetProjectionMatrix());

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
