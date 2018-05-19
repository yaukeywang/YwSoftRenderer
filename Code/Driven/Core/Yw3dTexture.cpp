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
        uint32_t texFilter = samplerStates[Yw3d_TSS_MinFilter];
        float texMipLevel = 0.0f;

        // Choose proper mip level and filter.
        if ((nullptr != xGradient) && (nullptr != yGradient))
        {
            // Compute the mip-level and determine the texture filter type.
            const float lenXGrad = xGradient->x * xGradient->x * m_SquaredWidth + xGradient->y * xGradient->y * m_SquaredHeight;
            const float lenYGrad = yGradient->x * yGradient->x * m_SquaredWidth + yGradient->y * yGradient->y * m_SquaredHeight;
            const float texelsPerScreenPixel = sqrtf(lenXGrad > lenYGrad ? lenXGrad : lenYGrad);

            if (texelsPerScreenPixel <= 1.0f)
            {
                // If texelsPerScreenPixel < 1.0f -> magnification, no mipmapping needed.
                texMipLevel = 0.0f;
                texFilter = samplerStates[Yw3d_TSS_MagFilter];
            }
            else
            {
                // Minification, need mipmapping.
                static const float invLog2 = 1.0f / logf(2.0f); // Calculate log2.
                texMipLevel = logf(texelsPerScreenPixel) * invLog2;
                texFilter = samplerStates[Yw3d_TSS_MinFilter];
            }
        }

        const float mipLodBias = *(float*)&samplerStates[Yw3d_TSS_MipLodBias];
        const float maxMipLevel = *(float*)&samplerStates[Yw3d_TSS_MaxMipLevel];
        texMipLevel = Clamp(texMipLevel + mipLodBias, 0.0f, maxMipLevel);

        if (Yw3d_TF_Linear == samplerStates[Yw3d_TSS_MipFilter])
        {
            uint32_t mipLevelA = ftol(texMipLevel);
            uint32_t mipLevelB = mipLevelA + 1;
            mipLevelA = (mipLevelA >= m_MipLevels) ? (m_MipLevels - 1) : mipLevelA;
            mipLevelB = (mipLevelB >= m_MipLevels) ? (m_MipLevels - 1) : mipLevelB;

            Vector4 colorA;
            Vector4 colorB;
            if (Yw3d_TF_Linear == texFilter)
            {
                m_MipLevelsData[mipLevelA]->SampleLinear(colorA, u, v);
                m_MipLevelsData[mipLevelB]->SampleLinear(colorB, u, v);
            }
            else
            {
                m_MipLevelsData[mipLevelA]->SamplePoint(colorA, u, v);
                m_MipLevelsData[mipLevelB]->SamplePoint(colorB, u, v);
            }

            const float interpolation = texMipLevel - (float)mipLevelA;// TODO: Not accurate.
            Vector4Lerp(color, colorA, colorB, interpolation);
        }
        else
        {
            uint32_t mipLevel = ftol(texMipLevel);
            mipLevel = (mipLevel >= m_MipLevels) ? (m_MipLevels - 1) : mipLevel;

            if (Yw3d_TF_Linear == texFilter)
            {
                m_MipLevelsData[mipLevel]->SampleLinear(color, u, v);
            }
            else
            {
                m_MipLevelsData[mipLevel]->SamplePoint(color, u, v);
            }
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dTexture::GenerateMipSubLevels(uint32_t srcLevel)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dTexture::Clear(uint32_t mipLevel, const Vector4& color, const Yw3dRect* rect)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dTexture::LockRect(uint32_t mipLevel, void** data, const Yw3dRect* rect)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dResult Yw3dTexture::UnlockRect(uint32_t mipLevel)
    {
        return Yw3d_E_Unknown;
    }

    Yw3dSurface* GetMipLevel(uint32_t mipLevel)
    {
        return nullptr;
    }

    Yw3dFormat Yw3dTexture::GetFormat()
    {
        return Yw3d_FMT_R32G32B32A32F;
    }

    uint32_t Yw3dTexture::GetFormatFloats()
    {
        return 0;
    }

    uint32_t Yw3dTexture::GetMipLevels()
    {
        return 0;
    }

    uint32_t Yw3dTexture::GetWidth(uint32_t)
    {
        return 0;
    }

    uint32_t Yw3dTexture::GetHeight(uint32_t)
    {
        return 0;
    }
}
