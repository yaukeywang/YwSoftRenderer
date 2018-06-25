// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional image class.

#include "stdafx.h"
#include "Yw3dVolume.h"
#include "Yw3dDevice.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dVolume::Yw3dVolume(Yw3dDevice* device) :
        m_Device(device),
        m_Format(Yw3d_FMT_R32G32B32A32F),
        m_Width(0),
        m_Height(0),
        m_Depth(0),
        m_WidthMin1(0),
        m_HeightMin1(0),
        m_DepthMin1(0),
        m_LockedComplete(false),
        m_PartialLockData(nullptr),
        m_Data(nullptr)
    {
    }

    Yw3dVolume::~Yw3dVolume()
    {
        YW_SAFE_DELETE_ARRAY(m_PartialLockData);
        YW_SAFE_DELETE_ARRAY(m_Data);
    }

    Yw3dResult Yw3dVolume::Create(uint32_t width, uint32_t height, uint32_t depth, Yw3dFormat format)
    {
        if ((0 == width) || (0 == height))
        {
            LOGE(_T( "Yw3dVolume::Create: volume dimensions are invalid.\n"));
            return Yw3d_E_InvalidParameters;
        }

        uint32_t fmtFloats = 0;
        switch (format)
        {
            case Yw3d_FMT_R32F:
                fmtFloats = 1;
                break;
            case Yw3d_FMT_R32G32F:
                fmtFloats = 2;
                break;
            case Yw3d_FMT_R32G32B32F:
                fmtFloats = 3;
                break;
            case Yw3d_FMT_R32G32B32A32F:
                fmtFloats = 4;
                break;
            default: /* This cannot happen. */
                LOGE(_T( "Yw3dVolume::Create: invalid format specified.\n"));
                return Yw3d_E_InvalidFormat;
        }

        m_Format = format;
        m_Width = width;
        m_Height = height;
        m_Depth = depth;
        m_WidthMin1 = m_Width - 1;
        m_HeightMin1 = m_Height - 1;
        m_DepthMin1 = m_Depth - 1;

        // Alloc data.
        m_Data = new float[m_Width * m_Height * m_Depth * fmtFloats];
        if (nullptr == m_Data)
        {
            LOGE(_T( "Yw3dVolume::Create: out of memory, cannot create volume.\n"));
            return Yw3d_E_OutOfMemory;
        }

        return Yw3d_S_OK;
    }

    void Yw3dVolume::SamplePoint(Vector4& color, float u, float v, float w)
    {
        const float fX = u * m_WidthMin1;
        const float fY = v * m_HeightMin1;
        const float fZ = w * m_DepthMin1;
        
        const uint32_t pixelX = ftol(fX);
        const uint32_t pixelY = ftol(fY);
        const uint32_t pixelZ = ftol(fZ);

        switch (m_Format)
        {
            case Yw3d_FMT_R32F:
                {
                    const float* pixel = &m_Data[pixelX + pixelY * m_Width + pixelZ * m_Width * m_Height];
                    color = Vector4(pixel[0], 0.0f, 0.0f, 1.0f);
                }
                break;
            case Yw3d_FMT_R32G32F:
                {
                    const Vector2* pixel = (const Vector2*)&m_Data[2 * (pixelX + pixelY * m_Width + pixelZ * m_Width * m_Height)];
                    color = Vector4(pixel->x, pixel->y, 0.0f, 1.0f);
                }
                break;
            case Yw3d_FMT_R32G32B32F:
                {
                    const Vector3* pixel = (const Vector3*)&m_Data[3 * (pixelX + pixelY * m_Width + pixelZ * m_Width * m_Height)];
                    color = Vector4(pixel->x, pixel->y, pixel->z, 1.0f);
                }
                break;
            case Yw3d_FMT_R32G32B32A32F:
                {
                    const Vector4* pixel = (const Vector4*)&m_Data[4 * (pixelX + pixelY * m_Width + pixelZ * m_Width * m_Height)];
                    color = *pixel;
                }
                break;
            default: /* This cannot happen. */
                break;
        }
    }

    void Yw3dVolume::SampleLinear(Vector4& color, float u, float v, float w)
    {
        const float fX = u * m_WidthMin1;
        const float fY = v * m_HeightMin1;
        const float fZ = w * m_DepthMin1;
        
        const uint32_t pixelX = ftol(fX);
        const uint32_t pixelY = ftol(fY);
        const uint32_t pixelZ = ftol(fZ);

        uint32_t pixelX2 = pixelX + 1;
        uint32_t pixelY2 = pixelY + 1;
        uint32_t pixelZ2 = pixelZ + 1;

        pixelX2 = (pixelX2 >= m_Width) ? m_WidthMin1 : pixelX2;
        pixelY2 = (pixelY2 >= m_Height) ? m_HeightMin1 : pixelY2;
        pixelZ2 = (pixelZ2 >= m_Depth) ? m_DepthMin1 : pixelZ2;

        const uint32_t indexRows[2] = { pixelY * m_Width, pixelY2 * m_Width };
        const uint32_t indexSlices[2] = { pixelZ * m_Width * m_Height, pixelZ2 * m_Width * m_Height };
        const float interpolation[3] = { fX - pixelX, fY - pixelY, fZ - pixelZ };

        switch (m_Format)
        {
            case Yw3d_FMT_R32F:
                {
                    float colorSlices[2] = { 0.0f, 0.0f };
                    float colorRows[2] = { 0.0f, 0.0f };

                    colorRows[0] = Lerp(m_Data[pixelX + indexRows[0] + indexSlices[0]], m_Data[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    colorRows[1] = Lerp(m_Data[pixelX + indexRows[1] + indexSlices[0]], m_Data[pixelX2 + indexRows[1] + indexSlices[0]], interpolation[0]);
                    colorSlices[0] = Lerp(colorRows[0], colorRows[1], interpolation[1]);

                    colorRows[0] = Lerp(m_Data[pixelX + indexRows[0] + indexSlices[1]], m_Data[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    colorRows[1] = Lerp(m_Data[pixelX + indexRows[1] + indexSlices[1]], m_Data[pixelX2 + indexRows[1] + indexSlices[1]], interpolation[0]);
                    colorSlices[1] = Lerp(colorRows[0], colorRows[1], interpolation[1]);

                    const float finalColor = Lerp(colorSlices[0], colorSlices[1], interpolation[2]);
                    color = Vector4(finalColor, 0.0f, 0.0f, 1.0f);
                }
                break;
            case Yw3d_FMT_R32G32F:
                {
                    const Vector2* pixelData = (const Vector2*)m_Data;

                    static Vector2 colorSlices[2];
                    static Vector2 colorRows[2];

                    Vector2Lerp(colorRows[0], pixelData[pixelX + indexRows[0] + indexSlices[0]], pixelData[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    Vector2Lerp(colorRows[1], pixelData[pixelX + indexRows[1] + indexSlices[0]], pixelData[pixelX2 + indexRows[1] + indexSlices[0]], interpolation[0]);
                    Vector2Lerp(colorSlices[0], colorRows[0], colorRows[1], interpolation[1]);

                    Vector2Lerp(colorRows[0], pixelData[pixelX + indexRows[0] + indexSlices[1]], pixelData[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    Vector2Lerp(colorRows[1], pixelData[pixelX + indexRows[1] + indexSlices[1]], pixelData[pixelX2 + indexRows[1] + indexSlices[1]], interpolation[0]);
                    Vector2Lerp(colorSlices[1], colorRows[0], colorRows[1], interpolation[1]);

                    static Vector2 finalColor;
                    Vector2Lerp(finalColor, colorSlices[0], colorSlices[1], interpolation[2]);

                    color = Vector4(finalColor.x, finalColor.y, 0.0f, 1.0f);
                }
                break;
            case Yw3d_FMT_R32G32B32F:
                {
                    const Vector3* pixelData = (const Vector3*)m_Data;

                    static Vector3 colorSlices[2];
                    static Vector3 colorRows[2];

                    Vector3Lerp(colorRows[0], pixelData[pixelX + indexRows[0] + indexSlices[0]], pixelData[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    Vector3Lerp(colorRows[1], pixelData[pixelX + indexRows[1] + indexSlices[0]], pixelData[pixelX2 + indexRows[1] + indexSlices[0]], interpolation[0]);
                    Vector3Lerp(colorSlices[0], colorRows[0], colorRows[1], interpolation[1]);

                    Vector3Lerp(colorRows[0], pixelData[pixelX + indexRows[0] + indexSlices[1]], pixelData[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    Vector3Lerp(colorRows[1], pixelData[pixelX + indexRows[1] + indexSlices[1]], pixelData[pixelX2 + indexRows[1] + indexSlices[1]], interpolation[0]);
                    Vector3Lerp(colorSlices[1], colorRows[0], colorRows[1], interpolation[1]);

                    static Vector3 finalColor;
                    Vector3Lerp(finalColor, colorSlices[0], colorSlices[1], interpolation[2]);

                    color = Vector4(finalColor.x, finalColor.y, finalColor.z, 1.0f);
                }
                break;
            case Yw3d_FMT_R32G32B32A32F:
                {
                    const Vector4* pixelData = (const Vector4*)m_Data;

                    static Vector4 colorSlices[2];
                    static Vector4 colorRows[2];

                    Vector4Lerp(colorRows[0], pixelData[pixelX + indexRows[0] + indexSlices[0]], pixelData[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    Vector4Lerp(colorRows[1], pixelData[pixelX + indexRows[1] + indexSlices[0]], pixelData[pixelX2 + indexRows[1] + indexSlices[0]], interpolation[0]);
                    Vector4Lerp(colorSlices[0], colorRows[0], colorRows[1], interpolation[1]);

                    Vector4Lerp(colorRows[0], pixelData[pixelX + indexRows[0] + indexSlices[1]], pixelData[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    Vector4Lerp(colorRows[1], pixelData[pixelX + indexRows[1] + indexSlices[1]], pixelData[pixelX2 + indexRows[1] + indexSlices[1]], interpolation[0]);
                    Vector4Lerp(colorSlices[1], colorRows[0], colorRows[1], interpolation[1]);

                    Vector4Lerp(color, colorSlices[0], colorSlices[1], interpolation[2]);
                }
                break;
            default: /* This cannot happen. */
                break;
        }
    }

    Yw3dResult Yw3dVolume::Clear(const Vector4& color, const Yw3dBox* box)
    {
        Yw3dBox clearBox;
        if (nullptr != box)
        {
            if ((box->right > m_Width) || (box->bottom > m_Height) || (box->back > m_Depth))
            {
                LOGE(_T("Yw3dVolume::Clear: clear-box exceeds volume's dimensions.\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((box->left >= box->right) || (box->top >= box->bottom) || (box->front >= box->back))
            {
                LOGE(_T("Yw3dVolume::Clear: invalid box specified!\n"));
                return Yw3d_E_InvalidParameters;
            }

            clearBox = *box;
        }
        else
        {
            clearBox.left = 0;
            clearBox.top = 0;
            clearBox.front = 0;
            clearBox.right = m_Width;
            clearBox.bottom = m_Height;
            clearBox.back = m_Depth;
        }

        // Start to lock data to clear.
        float* lockData = nullptr;
        Yw3dResult lockResult = LockBox((void**)&lockData, nullptr);
        if (YW3D_FAILED(lockResult))
        {
            return lockResult;
        }

        switch (m_Format)
        {
        case Yw3d_FMT_R32F:
            {
                for (uint32_t zIdx = clearBox.front; zIdx < clearBox.back; zIdx++)
                {
                    float* sliceData = &m_Data[zIdx * m_Width * m_Height];
                    for (uint32_t yIdx = clearBox.top; yIdx < clearBox.bottom; yIdx++)
                    {
                        float* curData = &m_Data[yIdx * m_Width + clearBox.left];
                        for (uint32_t xIdx = clearBox.left; xIdx < clearBox.right; xIdx++, curData++)
                        {
                            *curData = color.x;
                        }
                    }
                }
            }
            break;
        case Yw3d_FMT_R32G32F:
            {
                for (uint32_t zIdx = clearBox.front; zIdx < clearBox.back; zIdx++)
                {
                    Vector2* sliceData = &((Vector2*)m_Data)[zIdx * m_Width * m_Height];
                    for (uint32_t yIdx = clearBox.top; yIdx < clearBox.bottom; yIdx++)
                    {
                        Vector2* curData = &sliceData[yIdx * m_Width + clearBox.left];
                        for (uint32_t xIdx = clearBox.left; xIdx < clearBox.right; xIdx++, curData++)
                        {
                            *curData = color;
                        }
                    }
                }
            }
            break;
        case Yw3d_FMT_R32G32B32F:
            {
                for (uint32_t zIdx = clearBox.front; zIdx < clearBox.back; zIdx++)
                {
                    Vector3* sliceData = &((Vector3*)m_Data)[zIdx * m_Width * m_Height];
                    for (uint32_t yIdx = clearBox.top; yIdx < clearBox.bottom; yIdx++)
                    {
                        Vector3* curData = &sliceData[yIdx * m_Width + clearBox.left];
                        for (uint32_t xIdx = clearBox.left; xIdx < clearBox.right; xIdx++, curData++)
                        {
                            *curData = color;
                        }
                    }
                }
            }
            break;
        case Yw3d_FMT_R32G32B32A32F:
            {
                for (uint32_t zIdx = clearBox.front; zIdx < clearBox.back; zIdx++)
                {
                    Vector4* sliceData = &((Vector4*)m_Data)[zIdx * m_Width * m_Height];
                    for (uint32_t yIdx = clearBox.top; yIdx < clearBox.bottom; yIdx++)
                    {
                        Vector4* curData = &sliceData[yIdx * m_Width + clearBox.left];
                        for (uint32_t xIdx = clearBox.left; xIdx < clearBox.right; xIdx++, curData++)
                        {
                            *curData = color;
                        }
                    }
                }
            }
            break;
        default: /* This cannot happen. */
            LOGE(_T("Yw3dVolume::Clear: invalid volume format.\n"));
            UnlockBox();
            return Yw3d_E_InvalidFormat;
        }

        UnlockBox();
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dVolume::CopyToVolume(const Yw3dBox* srcBox, Yw3dVolume* destVolume, const Yw3dBox* destBox, Yw3dTextureFilter filter)
    {
        if (nullptr == destVolume)
        {
            LOGE(_T("Yw3dVolume::CopyToVolume: no destination volume specified!\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((Yw3d_TF_Point != filter) || (Yw3d_TF_Linear != filter))
        {
            LOGE(_T("Yw3dVolume::CopyToVolume: invalid filter specified!\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Get valid source box.
        Yw3dBox usedSrcBox;
        if (nullptr != srcBox)
        {
            if ((srcBox->right > m_Width) || (srcBox->bottom > m_Height) || (srcBox->back > m_Depth))
            {
                LOGE(_T("Yw3dVolume::CopyToVolume: source box exceeds volume dimensions!\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((srcBox->left >= srcBox->right) || (srcBox->top >= srcBox->bottom) || (srcBox->front >= srcBox->back))
            {
                LOGE(_T("Yw3dVolume::CopyToVolume: invalid source box specified!\n"));
                return Yw3d_E_InvalidParameters;
            }

            usedSrcBox = *srcBox;
        }
        else
        {
            usedSrcBox.left = 0;
            usedSrcBox.top = 0;
            usedSrcBox.front = 0;
            usedSrcBox.right = m_Width;
            usedSrcBox.bottom = m_Height;
            usedSrcBox.back = m_Depth;
        }

        // Get valid destination box.
        Yw3dBox usedDestBox;
        if (nullptr != destBox)
        {
            if ((destBox->right > destVolume->GetWidth()) || (destBox->bottom > destVolume->GetHeight()) || (destBox->back > destVolume->GetDepth()))
            {
                LOGE(_T("Yw3dVolume::CopyToVolume: destination box exceeds volume dimensions!\n"));
                return Yw3d_E_InvalidParameters;
            }

            if ((destBox->left >= destBox->right) || (destBox->top >= destBox->bottom) || (destBox->front >= destBox->back))
            {
                LOGE(_T("Yw3dVolume::CopyToVolume: invalid destination box specified!\n"));
                return Yw3d_E_InvalidParameters;
            }

            usedDestBox = *destBox;
        }
        else
        {
            usedDestBox.left = 0;
            usedDestBox.top = 0;
            usedDestBox.front = 0;
            usedDestBox.right = destVolume->GetWidth();
            usedDestBox.bottom = destVolume->GetHeight();
            usedDestBox.back = destVolume->GetDepth();
        }

        // Lock destination box data.
        float* destData = nullptr;
        Yw3dResult lockResult = destVolume->LockBox((void**)&destData, destBox);
        if (YW3D_FAILED(lockResult))
        {
            LOGE(_T("Yw3dVolume::CopyToVolume: couldn't lock destination volume!\n"));
            return lockResult;
        }

        const uint32_t destFloat = destVolume->GetFormatFloats();
        const uint32_t destWidth = usedDestBox.right - usedDestBox.left;
        const uint32_t destHeight = usedDestBox.bottom - usedDestBox.top;
        const uint32_t destDepth = usedDestBox.back - usedDestBox.front;

        // Directly copy possible?
        if ((nullptr == srcBox) && (nullptr == destBox) && (destFloat == GetFormatFloats()) && (destWidth == m_Width) && (destHeight == m_Height) && (destDepth == m_Depth))
        {
            memcpy(destData, m_Data, sizeof(float) * destFloat * destWidth * destHeight * destDepth);
            destVolume->UnlockBox();

            return Yw3d_S_OK;
        }

        // Copy data.
        const float stepU = 1.0f / m_WidthMin1;
        const float stepV = 1.0f / m_HeightMin1;
        const float stepW = 1.0f / m_DepthMin1;

        float srcW = usedSrcBox.front * stepW;
        for (uint32_t zIdx = 0; zIdx < destDepth; zIdx++, srcW += stepW)
        {
            float srcV = usedSrcBox.top * stepV;
            for (uint32_t yIdx = 0; yIdx < destHeight; yIdx++, srcV += stepV)
            {
                float srcU = usedSrcBox.left * stepU;
                for (uint32_t xIdx = 0; xIdx < destWidth; xIdx++, srcU += stepU, destData += destFloat)
                {
                    Vector4 srcColor;
                    if (Yw3d_TF_Linear == filter)
                    {
                        SampleLinear(srcColor, srcU, srcV, srcW);
                    }
                    else
                    {
                        SamplePoint(srcColor, srcU, srcV, srcW);
                    }

                    switch (destFloat)
                    {
                    case 4: destData[3] = srcColor.a;
                    case 3: destData[2] = srcColor.b;
                    case 2: destData[1] = srcColor.g;
                    case 1: destData[0] = srcColor.r;
                    }
                }
            }
        }

        destVolume->UnlockBox();
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dVolume::LockBox(void** data, const Yw3dBox* box)
    {
        if (nullptr == data)
        {
            LOGE(_T( "Yw3dVolume::LockBox: data points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (m_LockedComplete || (nullptr == m_PartialLockData))
        {
            LOGE(_T( "Yw3dVolume::LockBox: data points to null.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if (nullptr == box)
        {
            *data = m_Data;
            m_LockedComplete = true;

            return Yw3d_S_OK;
        }

        if ((box->right > m_Width) || (box->bottom > m_Height) || (box->back > m_Depth))
        {
            LOGE(_T( "Yw3dVolume::LockBox: box exceeds volume dimensions!\n"));
		    return Yw3d_E_InvalidParameters;
        }

        if ((box->left >= box->right) || (box->top >= box->bottom) || (box->front >= box->back))
        {
            LOGE(_T( "Yw3dVolume::LockBox: invalid box specified!\n"));
		    return Yw3d_E_InvalidParameters;
        }

        m_PartialLockBox = *box;

        // Create lock-buffer.
        const uint32_t lockWidth = m_PartialLockBox.right - m_PartialLockBox.left;
        const uint32_t lockHeight = m_PartialLockBox.bottom - m_PartialLockBox.top;
        const uint32_t lockDepth = m_PartialLockBox.back - m_PartialLockBox.front;
        const uint32_t volumeFloats = GetFormatFloats();

        m_PartialLockData = new float[lockWidth * lockHeight * lockDepth * volumeFloats];
        if (nullptr == m_PartialLockData)
        {
            LOGE(_T( "Yw3dVolume::LockBox: memory allocation failed!\n"));
		    return Yw3d_E_OutOfMemory;
        }

        float* curLockData = m_PartialLockData;
        for (uint32_t zIdx = m_PartialLockBox.front; zIdx < m_PartialLockBox.back; zIdx++)
        {
            const float* curVolumeData2 = &m_Data[(zIdx * m_Width * m_Height) * volumeFloats];
            for (uint32_t yIdx = m_PartialLockBox.top; yIdx < m_PartialLockBox.bottom; yIdx++)
            {
                const float* curVolumeData = &curVolumeData2[(yIdx * m_Width + m_PartialLockBox.left) * volumeFloats];
                memcpy(curLockData, curVolumeData, sizeof(float) * volumeFloats * lockWidth);
                curLockData += volumeFloats * lockWidth;
            }
        }
        
        *data = m_PartialLockData;
        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dVolume::UnlockBox()
    {
        if (!m_LockedComplete && (nullptr == m_PartialLockData))
        {
            LOGE(_T( "Yw3dVolume::UnlockBox: cannot unlock volume because it isn't locked!\n"));
		    return Yw3d_E_InvalidState;
        }

        if (m_LockedComplete)
        {
            m_LockedComplete = false;
            return Yw3d_S_OK;
        }

        // Update volume.
        const uint32_t lockWidth = m_PartialLockBox.right - m_PartialLockBox.left;
        const uint32_t volumeFloats = GetFormatFloats();

        const float* curLockData = m_PartialLockData;
        for (uint32_t zIdx = m_PartialLockBox.front; zIdx < m_PartialLockBox.back; zIdx++)
        {
            float* curVolumeData2 = &m_Data[(zIdx * m_Width * m_Height) * volumeFloats];
            for (uint32_t yIdx = m_PartialLockBox.top; yIdx < m_PartialLockBox.bottom; yIdx++)
            {
                float* curVolumeData = &curVolumeData2[(yIdx * m_Width + m_PartialLockBox.left) * volumeFloats];
                memcpy(curVolumeData, curLockData, sizeof(float) * volumeFloats * lockWidth);
                curLockData += volumeFloats * lockWidth;
            }
        }

        YW_SAFE_DELETE_ARRAY(m_PartialLockData);
        return Yw3d_S_OK;
    }

    Yw3dFormat Yw3dVolume::GetFormat() const
    {
        return m_Format;
    }

    uint32_t Yw3dVolume::GetFormatFloats() const
    {   
        switch (m_Format)
        {
        case Yw3d_FMT_R32F:
            return 1;
        case Yw3d_FMT_R32G32F:
            return 2;
        case Yw3d_FMT_R32G32B32F:
            return 3;
        case Yw3d_FMT_R32G32B32A32F:
            return 4;
        default: /* This cannot happen. */
            return 0;
        }
    }
    
    uint32_t Yw3dVolume::GetWidth() const
    {
        return m_Width;
    }

    uint32_t Yw3dVolume::GetHeight() const
    {
        return m_Height;
    }

    uint32_t Yw3dVolume::GetDepth() const
    {
        return m_Depth;
    }

    Yw3dDevice* Yw3dVolume::AcquireDevice()
    {
        if (nullptr != m_Device)
        {
            m_Device->AddRef();
        }

        return m_Device;
    }
}
