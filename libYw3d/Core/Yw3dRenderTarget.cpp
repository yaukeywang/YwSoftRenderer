// Add by Yaukey at 2018-01-31.
// YW Soft Renderer render target class.

#include "Yw3dRenderTarget.h"
#include "Yw3dDevice.h"
#include "Yw3dSurface.h"

namespace yw
{
    Yw3dRenderTarget::Yw3dRenderTarget(Yw3dDevice* device) :
        m_Device(device),
        m_ColorBuffer(nullptr),
        m_DepthBuffer(nullptr),
        m_StencilBuffer(nullptr)
    {
        m_Device->AddRef();
    }

    Yw3dRenderTarget::~Yw3dRenderTarget()
    {
        YW_SAFE_RELEASE(m_ColorBuffer);
        YW_SAFE_RELEASE(m_DepthBuffer);
        YW_SAFE_RELEASE(m_Device);
        YW_SAFE_RELEASE(m_StencilBuffer);
    }

    Yw3dDevice* Yw3dRenderTarget::AcquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }

    Yw3dResult Yw3dRenderTarget::ClearColorBuffer(const Vector4& color, const Yw3dRect* rect)
    {
        if (nullptr == m_ColorBuffer)
        {
            LOGE(_T("Yw3dRenderTarget::ClearColorBuffer: no framebuffer has been set.\n"));
            return Yw3d_E_InvalidFormat;
        }

        return m_ColorBuffer->Clear(color, rect);
    }

    Yw3dResult Yw3dRenderTarget::ClearDepthBuffer(const float depth, const Yw3dRect* rect)
    {
        if (NULL == m_DepthBuffer)
        {
            LOGE(_T("Yw3dRenderTarget::ClearDepthBuffer: no depthbuffer has been set.\n"));
            return Yw3d_E_InvalidFormat;
        }

        return m_DepthBuffer->Clear(Vector4(depth, 0.0f, 0.0f, 0.0f), rect);
    }

    Yw3dResult Yw3dRenderTarget::ClearStencilBuffer(const uint32_t stencil, const Yw3dRect* rect)
    {
        if (NULL == m_StencilBuffer)
        {
            LOGE(_T("Yw3dRenderTarget::ClearDepthBuffer: no depthbuffer has been set.\n"));
            return Yw3d_E_InvalidFormat;
        }

        float value = 0.0f;
        uint32_t* uintValue = (uint32_t*)(&value);
        *uintValue = stencil;
        return m_StencilBuffer->Clear(Vector4(value, 0.0f, 0.0f, 0.0f), rect);
    }

    Yw3dResult Yw3dRenderTarget::SetColorBuffer(Yw3dSurface* colorBuffer)
    {
        if (nullptr != colorBuffer)
        {
            if ((colorBuffer->GetFormat() < Yw3d_FMT_R32F) || (colorBuffer->GetFormat() > Yw3d_FMT_R32G32B32A32F))
            {
                LOGE(_T("Yw3dRenderTarget::SetColorBuffer: invalid framebuffer format.\n"));
                return Yw3d_E_InvalidFormat;
            }

            if (nullptr != m_DepthBuffer)
            {
                if ((m_DepthBuffer->GetWidth() != colorBuffer->GetWidth()) || (m_DepthBuffer->GetHeight() != colorBuffer->GetHeight()))
                {
                    LOGE(_T("Yw3dRenderTarget::SetColorBuffer: framebuffer and depthbuffer dimensions are not equal.\n"));
                    return Yw3d_E_InvalidFormat;
                }
            }
        }

        // Release old buffer and assign new buffer.
        YW_SAFE_RELEASE(m_ColorBuffer);
        m_ColorBuffer = colorBuffer;
        if (nullptr != m_ColorBuffer)
        {
            m_ColorBuffer->AddRef();
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dRenderTarget::SetDepthBuffer(Yw3dSurface* depthBuffer)
    {
        if (nullptr != depthBuffer)
        {
            if (Yw3d_FMT_R32F != depthBuffer->GetFormat())
            {
                LOGE(_T("Yw3dRenderTarget::SetDepthBuffer: invalid depthbuffer format.\n"));
                return Yw3d_E_InvalidFormat;
            }

            if (nullptr != m_ColorBuffer)
            {
                if ((m_ColorBuffer->GetWidth() != depthBuffer->GetWidth()) || (m_ColorBuffer->GetHeight() != depthBuffer->GetHeight()))
                {
                    LOGE(_T("Yw3dRenderTarget::SetDepthBuffer: depthbuffer and framebuffer dimensions are not equal.\n"));
                    return Yw3d_E_InvalidFormat;
                }
            }
        }

        // Release old buffer and assign new buffer.
        YW_SAFE_RELEASE(m_DepthBuffer);
        m_DepthBuffer = depthBuffer;
        if (nullptr != m_DepthBuffer)
        {
            m_DepthBuffer->AddRef();
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dRenderTarget::SetStencilBuffer(Yw3dSurface* stencilBuffer)
    {
        if (nullptr != stencilBuffer)
        {
            if (Yw3d_FMT_R32F != stencilBuffer->GetFormat())
            {
                LOGE(_T("Yw3dRenderTarget::SetStencilBuffer: invalid stencilbuffer format.\n"));
                return Yw3d_E_InvalidFormat;
            }

            if (nullptr != m_ColorBuffer)
            {
                if ((m_ColorBuffer->GetWidth() != stencilBuffer->GetWidth()) || (m_ColorBuffer->GetHeight() != stencilBuffer->GetHeight()))
                {
                    LOGE(_T("Yw3dRenderTarget::SetStencilBuffer: stencilbuffer and framebuffer dimensions are not equal.\n"));
                    return Yw3d_E_InvalidFormat;
                }
            }
        }

        // Release old buffer and assign new buffer.
        YW_SAFE_RELEASE(m_StencilBuffer);
        m_StencilBuffer = stencilBuffer;
        if (nullptr != stencilBuffer)
        {
            stencilBuffer->AddRef();
        }

        return Yw3d_S_OK;
    }

    Yw3dSurface* Yw3dRenderTarget::AcquireColorBuffer()
    {
        if (nullptr != m_ColorBuffer)
        {
            m_ColorBuffer->AddRef();
        }

        return m_ColorBuffer;
    }

    Yw3dSurface* Yw3dRenderTarget::AcquireDepthBuffer()
    {
        if (nullptr != m_DepthBuffer)
        {
            m_DepthBuffer->AddRef();
        }

        return m_DepthBuffer;
    }

    Yw3dSurface* Yw3dRenderTarget::AcquireStencilBuffer()
    {
        if (nullptr != m_StencilBuffer)
        {
            m_StencilBuffer->AddRef();
        }

        return m_StencilBuffer;
    }

    const Matrix44* Yw3dRenderTarget::GetViewportMatrix() const
    {
        const Matrix44* viewportMatrix = nullptr;
        if (YW3D_FAILED(m_Device->GetViewportMatrix(viewportMatrix)))
        {
            return nullptr;
        }

        return viewportMatrix;
    }
}
