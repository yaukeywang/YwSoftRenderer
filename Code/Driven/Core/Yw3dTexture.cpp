// Add by Yaukey at 2018-05-07.
// YW Soft Renderer texture class.

#include "stdafx.h"
#include "Yw3dTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dSurface.h"
#include "System/Support/YwUtility.h"

namespace yw
{
    Yw3dTexture::Yw3dTexture(Yw3dDevice* device) : 
        IYw3dBaseTexture(device),
        m_MipLevels(0),
        m_SquaredWidth(0),
        m_SquaredHeight(0),
        m_MipLevelsData(nullptr)
    {

    }

    Yw3dTexture::~Yw3dTexture()
    {
        for (uint32_t i = 0; i < m_MipLevels; i++)
        {
            YW_SAFE_RELEASE(m_MipLevelsData[i]);
        }

        YW_SAFE_DELETE_ARRAY(m_MipLevelsData);
    }

    Yw3dResult Yw3dTexture::Create(uint32_t width, uint32_t height, uint32_t mipLevels, Yw3dFormat format)
    {
        if ((0 == width) || (0 == height))
        {
            LOGE(_T("Yw3dTexture::Create: texture dimensions are invalid.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((format < Yw3d_FMT_R32F) || (format > Yw3d_FMT_R32G32B32A32F))
        {
            LOGE(_T("Yw3dTexture::Create: invalid format specified.\n"));
            return Yw3d_E_InvalidFormat;
        }

        m_SquaredWidth = (float)(width * width);
        m_SquaredHeight = (float)(height * height);

        if (0 == mipLevels)
        {
            // Create a full chain.
            uint32_t srcWidth = width;
            uint32_t srcHeight = height;

            do
            {
                ++mipLevels;
                srcWidth >>= 1;
                srcHeight >>= 1;
            } while ((0 == srcWidth) && (0 == srcHeight));
        }

        m_MipLevelsData = new Yw3dSurface*[mipLevels];
        if (nullptr == m_MipLevelsData)
        {
            LOGE(_T("Yw3dTexture::Create: out of memory, cannot create mip-levels.\n"));
            return Yw3d_E_OutOfMemory;
        }

        // Zero surface memory.
        memset(m_MipLevelsData, 0, sizeof(Yw3dSurface*) * mipLevels);

        // Create each surface for each mipmap level.
        Yw3dSurface** curMipLevelData = m_MipLevelsData;
        do
        {
            Yw3dResult resMipLevel = m_Device->CreateSurface(curMipLevelData, width, height, format);
            if (YW3D_FAILED(resMipLevel))
            {
                // destructor will perform cleanup
                LOGE(_T("Yw3dTexture::Create: creation of mip-level failed.\n"));
                return resMipLevel;
            }

            ++m_MipLevels;
            ++curMipLevelData;

            if (0 == --mipLevels)
            {
                break;
            }

            width >>= 1;
            height >>= 1;
        } while ((0 != width) && (0 == height));

        return Yw3d_S_OK;
    }

    Yw3dTextureSampleInput Yw3dTexture::GetTextureSampleInput()
    {
        return Yw3d_TSI_2Coords;
    }

    Yw3dResult Yw3dTexture::SampleTexture(Vector4& color, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient, const uint32_t* samplerStates)
    {
        return Yw3d_E_Unknown;
    }
}
