// Add by Yaukey at 2018-07-10.
// YW Soft Renderer 3d device render state control class.

#include "YwStateBlock.h"
#include "YwGraphics.h"

namespace yw
{
    StateBlock::StateBlock(Graphics* graphics) :
        m_Graphics(graphics),
        m_ChangedVertexFormat(false),
        m_ChangedPrimitiveAssembler(false),
        m_ChangedVertexShader(false),
        m_ChangedTriangleShader(false),
        m_ChangedPixelShader(false),
        m_ChangedIndexBuffer(false),
        m_ChangedRenderTarget(false),
        m_ChangedScissorRect(false),
        m_ChangedCamera(false)
    {
    }

    StateBlock::~StateBlock()
    {
        RestoreStates();
    }

    void StateBlock::RestoreStates()
    {

    }

    Yw3dResult StateBlock::SetRenderState(Yw3dRenderState renderState, uint32_t value)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record state if not yet saved.
        if (m_RenderStates.find(renderState) == m_RenderStates.end())
        {
            uint32_t currentValue = 0;
            if (YW3D_SUCCESSFUL(device->GetRenderState(renderState, currentValue)))
            {
                if (currentValue == value)
                {
                    return Yw3d_S_OK;
                }

                m_RenderStates[renderState] = currentValue;
            }
        }

        return device->SetRenderState(renderState, value);
    }

    Yw3dResult StateBlock::SetVertexFormat(Yw3dVertexFormat* vertexFormat)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedVertexFormat)
        {
            m_VertexFormat = device->AcquireVertexFormat();
            if (vertexFormat == m_VertexFormat)
            {
                YW_SAFE_RELEASE(m_VertexFormat);
                return Yw3d_S_OK;
            }

            m_ChangedVertexFormat = true;
        }

        return device->SetVertexFormat(vertexFormat);
    }

    void StateBlock::SetPrimitiveAssembler(IYw3dPrimitiveAssembler* primitiveAssembler)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedPrimitiveAssembler)
        {
            m_PrimitiveAssembler = device->AcquirePrimitiveAssembler();
            if (primitiveAssembler == m_PrimitiveAssembler)
            {
                YW_SAFE_RELEASE(m_PrimitiveAssembler);
                return;
            }

            m_ChangedPrimitiveAssembler = true;
        }

        device->SetPrimitiveAssembler(primitiveAssembler);
    }

    Yw3dResult StateBlock::SetVertexShader(IYw3dVertexShader* vertexShader)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedVertexShader)
        {
            m_VertexShader = device->AcquireVertexShader();
            if (vertexShader == m_VertexShader)
            {
                YW_SAFE_RELEASE(m_VertexShader);
                return Yw3d_S_OK;
            }

            m_ChangedVertexShader = true;
        }

        return device->SetVertexShader(vertexShader);
    }

    void StateBlock::SetTriangleShader(IYw3dTriangleShader* triangleShader)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedTriangleShader)
        {
            m_TriangleShader = device->AcquireTriangleShader();
            if (triangleShader == m_TriangleShader)
            {
                YW_SAFE_RELEASE(m_TriangleShader);
                return;
            }

            m_ChangedTriangleShader = true;
        }

        device->SetTriangleShader(triangleShader);
    }

    Yw3dResult StateBlock::SetPixelShader(IYw3dPixelShader* pixelShader)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedPixelShader)
        {
            m_PixelShader = device->AcquirePixelShader();
            if (pixelShader == m_PixelShader)
            {
                YW_SAFE_RELEASE(m_PixelShader);
                return Yw3d_S_OK;
            }

            m_ChangedPixelShader = true;
        }

        return device->SetPixelShader(pixelShader);
    }

    Yw3dResult StateBlock::SetIndexBuffer(Yw3dIndexBuffer* indexBuffer)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedIndexBuffer)
        {
            m_IndexBuffer = device->AcquireIndexBuffer();
            if (indexBuffer == m_IndexBuffer)
            {
                YW_SAFE_RELEASE(m_IndexBuffer);
                return Yw3d_S_OK;
            }

            m_ChangedIndexBuffer = true;
        }

        return device->SetIndexBuffer(indexBuffer);
    }

    Yw3dResult StateBlock::SetVertexStream(uint32_t streamNumber, Yw3dVertexBuffer* vertexBuffer, uint32_t offset, uint32_t stride)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (m_VertexStreams.find(streamNumber) == m_VertexStreams.end())
        {
            VertexStreamInfo streamInfo;
            if (YW3D_SUCCESSFUL(device->AcquireVertexStream(streamNumber, &streamInfo.vertexBuffer, &streamInfo.offset, &streamInfo.stride)))
            {
                if (
                    (vertexBuffer == streamInfo.vertexBuffer) &&
                    (offset == streamInfo.offset) &&
                    (stride == streamInfo.stride)
                    )
                {
                    YW_SAFE_RELEASE(streamInfo.vertexBuffer);
                    return Yw3d_S_OK;
                }

                m_VertexStreams[streamNumber] = streamInfo;
            }
        }

        return device->SetVertexStream(streamNumber, vertexBuffer, offset, stride);
    }

    Yw3dResult StateBlock::SetTexture(uint32_t samplerNumber, IYw3dBaseTexture* texture)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (m_Textures.find(samplerNumber) == m_Textures.end())
        {
            IYw3dBaseTexture* curTexture = nullptr;
            if (YW3D_SUCCESSFUL(device->AcquireTexture(samplerNumber, &curTexture)))
            {
                if (curTexture == texture)
                {
                    YW_SAFE_RELEASE(curTexture);
                    return Yw3d_S_OK;
                }

                m_Textures[samplerNumber] = curTexture;
            }
        }

        return device->SetTexture(samplerNumber, texture);
    }

    Yw3dResult StateBlock::SetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t state)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        uint32_t index = samplerNumber * (uint32_t)Yw3d_TSS_NumTextureSamplerStates + (uint32_t)textureSamplerState;
        if (m_TextureSamplerStates.find(index) == m_TextureSamplerStates.end())
        {
            uint32_t curState = 0;
            if (YW3D_SUCCESSFUL(device->GetTextureSamplerState(samplerNumber, textureSamplerState, curState)))
            {
                if (curState == state)
                {
                    return Yw3d_S_OK;
                }

                m_TextureSamplerStates[index] = curState;
            }
        }

        return device->SetTextureSamplerState(samplerNumber, textureSamplerState, state);
    }

    void StateBlock::SetRenderTarget(Yw3dRenderTarget* renderTarget)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedRenderTarget)
        {
            m_RenderTarget = device->AcquireRenderTarget();
            if (renderTarget == m_RenderTarget)
            {
                YW_SAFE_RELEASE(m_RenderTarget);
                return;
            }

            m_ChangedRenderTarget = true;
        }

        device->SetRenderTarget(renderTarget);
    }

    Yw3dResult StateBlock::SetScissorRect(const Yw3dRect& scissorRect)
    {
        // Get device.
        Yw3dDevice* device = m_Graphics->GetYw3dDevice();

        // Record if not yet saved.
        if (!m_ChangedScissorRect)
        {
            m_ScissorRect = device->GetScissorRect();
            m_ChangedScissorRect = true;
        }

        return device->SetScissorRect(scissorRect);
    }

    void StateBlock::SetCurrentCamera(class Camera* camera)
    {
        //// Record if not yet saved.
        //if (!m_ChangedCamera)
        //{
        //    m_Camera = m_Graphics->GetCamera();
        //}
    }
}
