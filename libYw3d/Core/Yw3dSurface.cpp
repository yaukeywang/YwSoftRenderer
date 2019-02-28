// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d surface class.

#include "Yw3dSurface.h"
#include "Yw3dDevice.h"

namespace yw
{
    Yw3dSurface::Yw3dSurface(Yw3dDevice* device) :
        m_Device(device), 
        m_Format(Yw3d_FMT_R32G32B32A32F), 
        m_Width(0), 
        m_Height(0), 
        m_WidthMin1(0), 
        m_HeightMin1(0), 
        m_LockedComplete(false), 
        m_PartialLockData(nullptr), 
        m_Data(nullptr)
    {
        // Note: cannot add a reference to parent or the presenttarget will never be freed?
        m_Device->AddRef();
    }

    Yw3dSurface::~Yw3dSurface()
    {
        YW_SAFE_DELETE_ARRAY(m_PartialLockData); // somebody might have forgotten to unlock the surface ;)
        YW_SAFE_DELETE_ARRAY(m_Data);

        /// Note: see note in constructor.
        YW_SAFE_RELEASE(m_Device);
    }

    Yw3dResult Yw3dSurface::Create(const uint32_t width, const uint32_t height, const Yw3dFormat format)
    {
        if ((0 == width) || (0 == height))
        {
            LOGE(_T("Yw3dSurface::Create: surface dimensions are invalid.\n"));
            return Yw3d_E_InvalidParameters;
        }

        uint32_t floatCount = 0;
        switch (format)
        {
        case Yw3d_FMT_R32F:
            floatCount = 1;
            break;
        case Yw3d_FMT_R32G32F:
            floatCount = 2;
            break;
        case Yw3d_FMT_R32G32B32F:
            floatCount = 3;
            break;
        case Yw3d_FMT_R32G32B32A32F:
            floatCount = 4;
            break;
        default:
            LOGE(_T("Yw3dSurface::Create: invalid format specified.\n"));
            return Yw3d_E_InvalidFormat;
        }

        m_Format = format;
        m_Width = width;
        m_Height = height;
        m_WidthMin1 = width - 1;
        m_HeightMin1 = height - 1;

        m_Data = new float[m_Width * m_Height * floatCount];
        if (nullptr == m_Data)
        {
            LOGE(_T("Yw3dSurface::Create: out of memory, cannot create surface.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return Yw3d_S_OK;
    }

    void Yw3dSurface::SamplePoint(Vector4& outColor, const float u, const float v)
    {
        const float fX = u * m_WidthMin1;
        const float fY = v * m_HeightMin1;

        const uint32_t pixelX = ftol(fX);
        const uint32_t pixelY = ftol(fY);

        switch (m_Format)
        {
        case Yw3d_FMT_R32F:
            {
                const float* pixel = &m_Data[(pixelY * m_Width + pixelX) * 1];
                outColor.Set(*pixel, 0.0f, 0.0f, 1.0f);
            }
            break;
        case Yw3d_FMT_R32G32F:
            {
                const Vector2* pixel = (const Vector2*)&m_Data[(pixelY * m_Width + pixelX) * 2];
                outColor.Set(pixel->x, pixel->y, 0.0f, 1.0f);
            }
            break;
        case Yw3d_FMT_R32G32B32F:
            {
                const Vector3* pixel = (const Vector3*)&m_Data[(pixelY * m_Width + pixelX) * 3];
                outColor.Set(pixel->x, pixel->y, pixel->z, 1.0f);
            }
            break;
        case Yw3d_FMT_R32G32B32A32F:
            {
                const Vector4* pixel = (const Vector4*)&m_Data[(pixelY * m_Width + pixelX) * 4];
                outColor = *pixel;
            }
            break;
        default:
            // This can not happen.
            break;
        }
    }

    void Yw3dSurface::SampleLinear(Vector4& outColor, const float u, const float v)
    {
        const float fX = u * m_WidthMin1;
        const float fY = v * m_HeightMin1;

        const uint32_t pixelX = ftol(fX);
        const uint32_t pixelY = ftol(fY);

        uint32_t pixelX2 = pixelX + 1;
        if (pixelX2 >= m_Width)
        {
            pixelX2 = m_WidthMin1;
        }

        uint32_t pixelY2 = pixelY + 1;
        if (pixelY2 >= m_Height)
        {
            pixelY2 = m_HeightMin1;
        }

        const uint32_t pixelRows[2] = {pixelY * m_Width, pixelY2 * m_Width};
        const float pixelInterpoltaions[2] = {fX - pixelX, fY - pixelY};

        switch (m_Format)
        {
        case Yw3d_FMT_R32F:
            {
                float colorRows[2] = {0.0f, 0.0f};
                colorRows[0] = Lerp(m_Data[pixelRows[0] + pixelX], m_Data[pixelRows[0] + pixelX2], pixelInterpoltaions[0]);
                colorRows[1] = Lerp(m_Data[pixelRows[1] + pixelX], m_Data[pixelRows[1] + pixelX2], pixelInterpoltaions[0]);

                const float finalColor = Lerp(colorRows[0], colorRows[1], pixelInterpoltaions[1]);
                outColor.Set(finalColor, 0.0f, 0.0f, 1.0f);
            }
            break;
        case Yw3d_FMT_R32G32F:
            {
                const Vector2* pixelData = (const Vector2*)m_Data;

                Vector2 colorRows[2];
                Vector2Lerp(colorRows[0], pixelData[pixelRows[0] + pixelX], pixelData[pixelRows[0] + pixelX2], pixelInterpoltaions[0]);
                Vector2Lerp(colorRows[1], pixelData[pixelRows[1] + pixelX], pixelData[pixelRows[1] + pixelX2], pixelInterpoltaions[0]);

                Vector2 finalColor;
                Vector2Lerp(finalColor, colorRows[0], colorRows[1], pixelInterpoltaions[1]);
                outColor.Set(finalColor.x, finalColor.y, 0.0f, 1.0f);
            }
            break;
        case Yw3d_FMT_R32G32B32F:
            {
                const Vector3* pixelData = (const Vector3*)m_Data;

                Vector3 colorRows[2];
                Vector3Lerp(colorRows[0], pixelData[pixelRows[0] + pixelX], pixelData[pixelRows[0] + pixelX2], pixelInterpoltaions[0]);
                Vector3Lerp(colorRows[1], pixelData[pixelRows[1] + pixelX], pixelData[pixelRows[1] + pixelX2], pixelInterpoltaions[0]);

                Vector3 finalColor;
                Vector3Lerp(finalColor, colorRows[0], colorRows[1], pixelInterpoltaions[1]);
                outColor.Set(finalColor.x, finalColor.y, finalColor.z, 1.0f);
            }
            break;
        case Yw3d_FMT_R32G32B32A32F:
        {
            const Vector4* pixelData = (const Vector4*)m_Data;

            Vector4 colorRows[2];
            Vector4Lerp(colorRows[0], pixelData[pixelRows[0] + pixelX], pixelData[pixelRows[0] + pixelX2], pixelInterpoltaions[0]);
            Vector4Lerp(colorRows[1], pixelData[pixelRows[1] + pixelX], pixelData[pixelRows[1] + pixelX2], pixelInterpoltaions[0]);
            Vector4Lerp(outColor, colorRows[0], colorRows[1], pixelInterpoltaions[1]);
        }
            break;
        default:
            // This can not happen.
            break;
        }
    }

    Yw3dResult Yw3dSurface::Clear(const Vector4& color, const Yw3dRect* rect)
    {
        Yw3dRect clearRect;
        if (nullptr != rect)
        {
            // Check validation of rect.
            if ((rect->right > m_Width) || (rect->bottom > m_Height) || (rect->left < 0) || (rect->top < 0))
            {
                LOGE(_T("Yw3dSurface::Clear: clear-rectangle exceeds surface's dimensions.\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((rect->left >= rect->right) || (rect->top >= rect->bottom))
            {
                LOGE(_T("Yw3dSurface::Clear: invalid rectangle specified!\n"));
                return Yw3d_E_InvalidParameters;
            }

            clearRect = *rect;
        }
        else
        {
            // Get the whole region.
            clearRect.left = 0;
            clearRect.top = 0;
            clearRect.right = m_Width;
            clearRect.bottom = m_Height;
        }

        // Lock surface buffer first.
        float* surfaceData = nullptr;
        Yw3dResult lockResult = LockRect((void**)&surfaceData, nullptr);
        if (YW3D_FAILED(lockResult))
        {
            return lockResult;
        }

        // Calculate row stride.
        const uint32_t rowStride = (m_Width - clearRect.right) + clearRect.left;

        // Clear surface data with specified color.
        switch (m_Format)
        {
        case Yw3d_FMT_R32F:
            {
                float* curData = &m_Data[clearRect.top * m_Width + clearRect.left];
                for (uint32_t y = clearRect.top; y < clearRect.bottom; y++, curData += rowStride)
                {
                    for (uint32_t x = clearRect.left; x < clearRect.right; x++, curData++)
                    {
                        *curData = color.r;
                    }
                }
            }
            break;
        case Yw3d_FMT_R32G32F:
            {
                Vector2* curData = &((Vector2*)m_Data)[clearRect.top * m_Width + clearRect.left];
                for (uint32_t y = clearRect.top; y < clearRect.bottom; y++, curData += rowStride)
                {
                    for (uint32_t x = clearRect.left; x < clearRect.right; x++, curData++)
                    {
                        *curData = color;
                    }
                }
            }
            break;
        case Yw3d_FMT_R32G32B32F:
            {
                Vector3* curData = &((Vector3*)m_Data)[clearRect.top * m_Width + clearRect.left];
                for (uint32_t y = clearRect.top; y < clearRect.bottom; y++, curData += rowStride)
                {
                    for (uint32_t x = clearRect.left; x < clearRect.right; x++, curData++)
                    {
                        *curData = color;
                    }
                }
            }
            break;
        case Yw3d_FMT_R32G32B32A32F:
            {
                Vector4* curData = &((Vector4*)m_Data)[clearRect.top * m_Width + clearRect.left];
                for (uint32_t y = clearRect.top; y < clearRect.bottom; y++, curData += rowStride)
                {
                    for (uint32_t x = clearRect.left; x < clearRect.right; x++, curData++)
                    {
                        *curData = color;
                    }
                }
            }
            break;
        default:
            // This can not happen.
            LOGE(_T("Yw3dSurface::Clear: invalid surface format.\n"));
            UnlockRect();
            return Yw3d_E_InvalidFormat;
        }

        // Unlock surface buffer.
        UnlockRect();

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dSurface::CopyToSurface(const Yw3dRect* srcRect, Yw3dSurface* destSurface, const Yw3dRect* destRect, const Yw3dTextureFilter filter)
    {
        // Check dest surface.
        if (nullptr == destSurface)
        {
            LOGE(_T("Yw3dSurface::CopyToSurface: no destination surface specified!\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Check format.
        if ((Yw3d_TF_Point != filter) && (Yw3d_TF_Linear != filter))
        {
            LOGE(_T("Yw3dSurface::CopyToSurface: invalid filter specified!\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Check src rect.
        Yw3dRect curSrcRect;
        if (nullptr != srcRect)
        {
            // Check validation of rect.
            if ((srcRect->right > m_Width) || (srcRect->bottom > m_Height) || (srcRect->left < 0) || (srcRect->top < 0))
            {
                LOGE(_T("Yw3dSurface::Clear: clear-rectangle exceeds surface's dimensions.\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((srcRect->left >= srcRect->right) || (srcRect->top >= srcRect->bottom))
            {
                LOGE(_T("Yw3dSurface::Clear: invalid rectangle specified!\n"));
                return Yw3d_E_InvalidParameters;
            }

            curSrcRect = *srcRect;
        }
        else
        {
            // Get the whole region.
            curSrcRect.left = 0;
            curSrcRect.top = 0;
            curSrcRect.right = m_Width;
            curSrcRect.bottom = m_Height;
        }

        // Check dst rect.
        Yw3dRect curDestRect;
        if (nullptr != destRect)
        {
            if ((destRect->right > m_Width) || (destRect->bottom > m_Height) || (destRect->left < 0) || (destRect->top < 0))
            {
                LOGE(_T("Yw3dSurface::CopyToSurface: destination rectangle exceeds surface dimensions!\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((destRect->left >= destRect->right) || (destRect->top >= destRect->bottom))
            {
                LOGE(_T("CMuli3DSurface::CopyToSurface: invalid destination rectangle specified!\n"));
                return Yw3d_E_InvalidParameters;
            }

            curDestRect = *destRect;
        }
        else
        {
            curDestRect.left = 0;
            curDestRect.top = 0;
            curDestRect.right = m_Width;
            curDestRect.bottom = m_Height;
        }

        // Lock dest surface data.
        float* destData = nullptr;
        Yw3dResult lockResult = destSurface->LockRect((void**)destData, destRect);
        if (YW3D_FAILED(lockResult))
        {
            LOGE(_T("CMuli3DSurface::CopyToSurface: couldn't lock destination surface!\n"));
            return lockResult;
        }

        // Dest info.
        const uint32_t destFloatCount = destSurface->GetFormatFloats();
        const uint32_t destWidth = curDestRect.right - curDestRect.left;
        const uint32_t destHeight = curDestRect.bottom - curDestRect.top;

        // Check if we can directly copy.
        if ((nullptr == srcRect) && (nullptr == destRect) && (destFloatCount == GetFormatFloats()) && (destWidth == m_Width) && (destHeight == m_Height))
        {
            memcpy(destData, m_Data, sizeof(float) * destWidth * destHeight * destFloatCount);
            destSurface->UnlockRect();

            return Yw3d_S_OK;
        }

        // Get uv step from src surface.
        const float stepU = 1.0f / m_WidthMin1;
        const float stepV = 1.0f / m_HeightMin1;

        // Copy from src data into dest data.
        float srcV = curSrcRect.top * stepV;
        for (uint32_t y = 0; y < destHeight; y++, srcV += stepV)
        {
            float srcU = curSrcRect.left * stepU;
            for (uint32_t x = 0; x < destWidth; x++, srcU += stepU, destData += destFloatCount)
            {
                Vector4 srcColor;
                if (Yw3d_TF_Linear == filter)
                {
                    SampleLinear(srcColor, srcU, srcV);
                }
                else
                {
                    SamplePoint(srcColor, srcU, srcV);
                }

                switch (m_Format)
                {
                case Yw3d_FMT_R32G32B32A32F: destData[3] = srcColor.a;
                case Yw3d_FMT_R32G32B32F: destData[2] = srcColor.b;
                case Yw3d_FMT_R32G32F: destData[1] = srcColor.g;
                case Yw3d_FMT_R32F: destData[0] = srcColor.r;
                }
            }
        }

        // Unlock dest surface buffer.
        destSurface->UnlockRect();

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dSurface::LockRect(void** lockedData, const Yw3dRect* lockRect)
    {
        if (nullptr == lockedData)
        {
            LOGE(_T("Yw3dSurface::LockRect: o_ppData points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Check validation of rect.
        if (m_LockedComplete || (nullptr != m_PartialLockData))
        {
            LOGE(_T("Yw3dSurface::LockRect: mip-level is already locked!\n"));
            return Yw3d_E_InvalidState;
        }

        // Entire region.
        if (nullptr == lockRect)
        {
            *lockedData = m_Data;
            m_LockedComplete = true;

            return Yw3d_S_OK;
        }

        if ((lockRect->right > m_Width) || (lockRect->bottom > m_Height) || (lockRect->left < 0) || (lockRect->top < 0))
        {
            LOGE(_T("Yw3dSurface::LockRect: rectangle exceeds surface dimensions!\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((lockRect->left >= lockRect->right) || (lockRect->top >= lockRect->bottom))
        {
            LOGE(_T("Yw3dSurface::LockRect: invalid rectangle specified!\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Get partial rect.
        m_PartialLockRect = *lockRect;

        // Create lock buffer.
        const uint32_t lockWidth = lockRect->right - lockRect->left;
        const uint32_t lockHeight = lockRect->bottom - lockRect->top;
        const uint32_t surfaceFloatCount = GetFormatFloats();

        m_PartialLockData = new float[lockWidth * lockHeight * surfaceFloatCount];
        if (nullptr == m_PartialLockData)
        {
            LOGE(_T("Yw3dSurface::LockRect: memory allocation failed!\n"));
            return Yw3d_E_OutOfMemory;
        }

        // Copy region data.
        float* curLockedData = m_PartialLockData;
        for (uint32_t y = m_PartialLockRect.top; y < m_PartialLockRect.bottom; y++)
        {
            const float* curSurfaceData = &m_Data[(y * m_Width + m_PartialLockRect.left) * surfaceFloatCount];
            memcpy(curLockedData, curSurfaceData, sizeof(float) * lockWidth * surfaceFloatCount);
            curLockedData += lockWidth * surfaceFloatCount;
        }

        // Fill out the locked partial data.
        *lockedData = m_PartialLockData;

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dSurface::UnlockRect()
    {
        if (!m_LockedComplete && (nullptr != m_PartialLockData))
        {
            LOGE(_T("Yw3dSurface::UnlockRect: cannot unlock mip-level because it isn't locked!\n"));
            return Yw3d_E_InvalidState;
        }

        if (m_LockedComplete)
        {
            m_LockedComplete = false;
            return Yw3d_S_OK;
        }

        // Update surface if partial locked.
        const uint32_t lockWidth = m_PartialLockRect.right - m_PartialLockRect.left;
        const uint32_t surfaceFloatCount = GetFormatFloats();

        float* curLockedData = m_PartialLockData;
        for (uint32_t y = m_PartialLockRect.top; y < m_PartialLockRect.bottom; y++)
        {
            float* curSurfaceData = &m_Data[(y * m_Width + m_PartialLockRect.left) * surfaceFloatCount];
            memcpy(curSurfaceData, curLockedData, sizeof(float) * lockWidth * surfaceFloatCount);
            curLockedData += lockWidth * surfaceFloatCount;
        }

        // Release partial resource.
        YW_SAFE_DELETE_ARRAY(m_PartialLockData);

        return Yw3d_S_OK;
    }

    Yw3dFormat Yw3dSurface::GetFormat() const
    {
        return m_Format;
    }

    uint32_t Yw3dSurface::GetFormatFloats() const
    {
        switch (m_Format)
        {
        case Yw3d_FMT_R32F:
            return 1;
        case Yw3d_FMT_R32G32F:
            return 2;
        case Yw3d_FMT_R32G32B32F:
            return 3;;
        case Yw3d_FMT_R32G32B32A32F:
            return 4;
        default:
            // Can not happen.
            return 0;
        }
    }

    uint32_t Yw3dSurface::GetWidth() const
    {
        return m_Width;
    }

    uint32_t Yw3dSurface::GetHeight() const
    {
        return m_Height;
    }

    Yw3dDevice* Yw3dSurface::AcquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }
}
