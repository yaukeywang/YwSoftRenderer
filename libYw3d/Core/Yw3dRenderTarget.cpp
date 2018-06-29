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
        m_DepthBuffer(nullptr)
    {
        m_Device->AddRef();
    }

    Yw3dRenderTarget::~Yw3dRenderTarget()
    {
        YW_SAFE_RELEASE(m_ColorBuffer);
        YW_SAFE_RELEASE(m_DepthBuffer);
        YW_SAFE_RELEASE(m_Device);
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

    Yw3dResult Yw3dRenderTarget::ClearDepthBuffer(const Vector4& depth, const Yw3dRect* rect)
    {
        if (NULL == m_DepthBuffer)
        {
            LOGE(_T("CMuli3DRenderTarget::ClearDepthBuffer: no depthbuffer has been set.\n"));
            return Yw3d_E_InvalidFormat;
        }

        return m_DepthBuffer->Clear(depth, rect);
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

    void Yw3dRenderTarget::SetViewportMatrix(const Matrix44& viewportMatrix)
    {
        m_ViewportMatrix = viewportMatrix;
    }

    const Matrix44& Yw3dRenderTarget::GetViewportMatrix() const
    {
        return m_ViewportMatrix;
    }
}
