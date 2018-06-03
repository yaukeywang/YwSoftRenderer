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
        m_bLockedComplete(false),
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

        const indexRows[2] = { pixelY * m_Width, pixelY2 * m_Width };
        const indexSlices[2] = { pixelZ * m_Width * m_Height, pixelZ2 * m_Width * m_Height };
        const float interpolation[3] = { fX - pixelX, fY - pixelY, fZ - pixelZ };

        switch (m_Format)
        {
            case Yw3d_FMT_R32F:
                {
                    float colorSlices[2] = { 0.0f, 0.0f };
                    float colorRows[2] = { 0.0f, 0.0f };

                    colorRows[0] = Lerp(m_Data[pixelX + indexRows[0] + indexSlices[0]], m_Data[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    colorRows[1] = Lerp(m_Data[pixelX + indexRows[1] + indexSlices[0]], m_Data[pixelX2 + indexRows[1]] + indexSlices[0]], interpolation[0]);
                    colorSlices[0] = Lerp(colorRows[0], colorRows[1], interpolation[1]);

                    colorRows[0] = Lerp(m_Data[pixelX + indexRows[0] + indexSlices[1]], m_Data[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    colorRows[1] = Lerp(m_Data[pixelX + indexRows[1] + indexSlices[1]], m_Data[pixelX2 + indexRows[1]] + indexSlices[1]], interpolation[0]);
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

                    Vector2Lerp(colorRows[0], m_Data[pixelX + indexRows[0] + indexSlices[0]], m_Data[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    Vector2Lerp(colorRows[1], m_Data[pixelX + indexRows[1] + indexSlices[0]], m_Data[pixelX2 + indexRows[1]] + indexSlices[0]], interpolation[0]);
                    Vector2Lerp(colorSlices[0], colorRows[0], colorRows[1], interpolation[1]);

                    Vector2Lerp(colorRows[0], m_Data[pixelX + indexRows[0] + indexSlices[1]], m_Data[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    Vector2Lerp(colorRows[1], m_Data[pixelX + indexRows[1] + indexSlices[1]], m_Data[pixelX2 + indexRows[1]] + indexSlices[1]], interpolation[0]);
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

                    Vector3Lerp(colorRows[0], m_Data[pixelX + indexRows[0] + indexSlices[0]], m_Data[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    Vector3Lerp(colorRows[1], m_Data[pixelX + indexRows[1] + indexSlices[0]], m_Data[pixelX2 + indexRows[1]] + indexSlices[0]], interpolation[0]);
                    Vector3Lerp(colorSlices[0], colorRows[0], colorRows[1], interpolation[1]);

                    Vector3Lerp(colorRows[0], m_Data[pixelX + indexRows[0] + indexSlices[1]], m_Data[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    Vector3Lerp(colorRows[1], m_Data[pixelX + indexRows[1] + indexSlices[1]], m_Data[pixelX2 + indexRows[1]] + indexSlices[1]], interpolation[0]);
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

                    Vector4Lerp(colorRows[0], m_Data[pixelX + indexRows[0] + indexSlices[0]], m_Data[pixelX2 + indexRows[0] + indexSlices[0]], interpolation[0]);
                    Vector4Lerp(colorRows[1], m_Data[pixelX + indexRows[1] + indexSlices[0]], m_Data[pixelX2 + indexRows[1]] + indexSlices[0]], interpolation[0]);
                    Vector4Lerp(colorSlices[0], colorRows[0], colorRows[1], interpolation[1]);

                    Vector4Lerp(colorRows[0], m_Data[pixelX + indexRows[0] + indexSlices[1]], m_Data[pixelX2 + indexRows[0] + indexSlices[1]], interpolation[0]);
                    Vector4Lerp(colorRows[1], m_Data[pixelX + indexRows[1] + indexSlices[1]], m_Data[pixelX2 + indexRows[1]] + indexSlices[1]], interpolation[0]);
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

    }

    Yw3dResult Yw3dVolume::CopyToVolume(const Yw3dBox* srcBox, Yw3dVolume* destVolume, const Yw3dBox* destBox, Yw3dTextureFilter filter)
    {

    }

    Yw3dResult Yw3dVolume::LockBox(void** data, const Yw3dBox* box)
    {

    }

    Yw3dResult Yw3dVolume::UnlockBox()
    {

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
