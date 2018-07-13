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

    }

    Yw3dResult StateBlock::SetVertexShader(IYw3dVertexShader* vertexShader)
    {
        return Yw3d_E_Unknown;
    }

    void StateBlock::SetTriangleShader(IYw3dTriangleShader* triangleShader)
    {

    }

    Yw3dResult StateBlock::SetPixelShader(IYw3dPixelShader* pixelShader)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult StateBlock::SetIndexBuffer(Yw3dIndexBuffer* indexBuffer)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult StateBlock::SetVertexStream(uint32_t streamNumber, Yw3dVertexBuffer* vertexBuffer, uint32_t offset, uint32_t stride)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult StateBlock::SetTexture(uint32_t samplerNumber, IYw3dBaseTexture* texture)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult StateBlock::SetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t state)
    {
        return Yw3d_E_Unknown;
    }

    void StateBlock::SetRenderTarget(Yw3dRenderTarget* renderTarget)
    {

    }

    Yw3dResult StateBlock::SetScissorRect(const Yw3dRect& scissorRect)
    {
        return Yw3d_E_Unknown;
    }

    void StateBlock::SetCurrentCamera(class Camera* camera)
    {
    }
}
