// Add by Yaukey at 2018-07-11.
// YW Soft Renderer 3d device graphics class.

#include "YwGraphics.h"
#include "YwBaseApplication.h"
#include "YwStateBlock.h"

namespace yw
{
    Graphics::Graphics(IApplication* application) :
        m_Application(application),
        m_Yw3d(nullptr),
        m_Yw3dDevice(nullptr)
    {
    }

    Graphics::~Graphics()
    {
        while (!m_StateBlocks.empty())
        {
            PopStateBlock();
        }

        YW_SAFE_RELEASE(m_Yw3dDevice);
        YW_SAFE_RELEASE(m_Yw3d);
    }

    bool Graphics::Initialize(const struct ApplicationCreationFlags& creationFlags)
    {
        // Initialize Yw3d.
        if (YW3D_FAILED(CreateYw3d(&m_Yw3d)))
        {
            return false;
        }

        // Initialize Yw3d device parameters - defaulting to 32 bit colors in full screen mode.
        Yw3dDeviceParameters deviceParams =
        {
            m_Application->GetWindowHandle(),
            m_Application->GetWindowed(),
            32,
            m_Application->GetWindowWidth(),
            m_Application->GetWindowHeight()
        };

        if (YW3D_FAILED(m_Yw3d->CreateDevice(&m_Yw3dDevice, &deviceParams)))
        {
            YW_SAFE_RELEASE(m_Yw3d);
            return false;
        }

        // Create subsystems.
        PushStateBlock();

        return false;
    }

    void Graphics::PushStateBlock()
    {
        StateBlock* newStateBlock = new StateBlock(this);
        m_StateBlocks.push(newStateBlock);
    }

    void Graphics::PopStateBlock()
    {
        if (!m_StateBlocks.empty())
        {
            YW_SAFE_DELETE(m_StateBlocks.top());
            m_StateBlocks.pop();
        }
    }

    Yw3dResult Graphics::SetRenderState(Yw3dRenderState renderState, uint32_t value)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetRenderState(renderState, value);
    }

    Yw3dResult Graphics::SetVertexFormat(Yw3dVertexFormat* vertexFormat)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetVertexFormat(vertexFormat);
    }

    void Graphics::SetPrimitiveAssembler(IYw3dPrimitiveAssembler* primitiveAssembler)
    {
        if (m_StateBlocks.empty())
        {
            return;
        }

        m_StateBlocks.top()->SetPrimitiveAssembler(primitiveAssembler);
    }

    Yw3dResult Graphics::SetVertexShader(IYw3dVertexShader* vertexShader)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetVertexShader(vertexShader);
    }

    void Graphics::SetTriangleShader(IYw3dTriangleShader* triangleShader)
    {
        if (m_StateBlocks.empty())
        {
            return;
        }

        m_StateBlocks.top()->SetTriangleShader(triangleShader);
    }

    Yw3dResult Graphics::SetPixelShader(IYw3dPixelShader* pixelShader)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetPixelShader(pixelShader);
    }

    Yw3dResult Graphics::SetIndexBuffer(Yw3dIndexBuffer* indexBuffer)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetIndexBuffer(indexBuffer);
    }

    Yw3dResult Graphics::SetVertexStream(uint32_t streamNumber, Yw3dVertexBuffer* vertexBuffer, uint32_t offset, uint32_t stride)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetVertexStream(streamNumber, vertexBuffer, offset, stride);
    }

    Yw3dResult Graphics::SetTexture(uint32_t samplerNumber, IYw3dBaseTexture* texture)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetTexture(samplerNumber, texture);
    }

    Yw3dResult Graphics::SetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t state)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetTextureSamplerState(samplerNumber, textureSamplerState, state);
    }

    void Graphics::SetRenderTarget(Yw3dRenderTarget* renderTarget)
    {
        if (m_StateBlocks.empty())
        {
            return;
        }

        m_StateBlocks.top()->SetRenderTarget(renderTarget);
    }

    Yw3dResult Graphics::SetScissorRect(const Yw3dRect& scissorRect)
    {
        if (m_StateBlocks.empty())
        {
            return Yw3d_E_Unknown;
        }

        return m_StateBlocks.top()->SetScissorRect(scissorRect);
    }
}
