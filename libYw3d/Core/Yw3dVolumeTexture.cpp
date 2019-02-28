// Add by Yaukey at 2018-05-23.
// YW Soft Renderer 3-dimensional texture class.

#include "Yw3dVolumeTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dVolume.h"

namespace yw
{
    Yw3dVolumeTexture::Yw3dVolumeTexture(Yw3dDevice* device) : 
        IYw3dBaseTexture(device),
        m_MipLevels(0),
        m_SquaredWidth(0.0f),
        m_SquaredHeight(0.0f),
        m_SquaredDepth(0.0f),
        m_MipLevelsData(nullptr)
    {

    }

    Yw3dVolumeTexture::~Yw3dVolumeTexture()
    {
        for (uint32_t level = 0; level < m_MipLevels; level++)
        {
            YW_SAFE_RELEASE(m_MipLevelsData[level]);
        }

        YW_SAFE_DELETE_ARRAY(m_MipLevelsData);
    }

    Yw3dResult Yw3dVolumeTexture::Create(uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, Yw3dFormat format)
    {
        if ((0 == width) || (0 == height) || (0 == depth))
        {
            LOGE(_T("Yw3dVolumeTexture::Create: texture dimensions are invalid.\n"));
            return Yw3d_E_InvalidParameters;
        }

        if ((format < Yw3d_FMT_R32F) || (format > Yw3d_FMT_R32G32B32A32F))
        {
            LOGE(_T("Yw3dVolumeTexture::Create: invalid format specified.\n"));
            return Yw3d_E_InvalidFormat;
        }

        m_SquaredWidth = (float)(width * width);
        m_SquaredHeight = (float)(height * height);
        m_SquaredDepth = (float)(depth * depth);

        if (0 == mipLevels)
        {
            // Create a full chain.
            uint32_t srcWidth = width;
            uint32_t srcHeight = height;
            uint32_t srcDepth = depth;

            do 
            {
                mipLevels++;
                srcWidth >>= 1;
                srcHeight >>= 1;
                srcDepth >>= 1;
            } while ((srcWidth > 0) && (srcHeight > 0) && (srcDepth > 0));
        }

        m_MipLevelsData = new Yw3dVolume*[mipLevels];
        if (nullptr == m_MipLevelsData)
        {
            LOGE(_T("Yw3dVolumeTexture::Create: out of memory, cannot create mip-levels.\n"));
            return Yw3d_E_OutOfMemory;
        }

        // Zero volume memory.
        memset(m_MipLevelsData, 0, sizeof(Yw3dVolume*) * mipLevels);

        // Create volume for each mipmap level.
        Yw3dVolume** curMipLevelData = m_MipLevelsData;
        do 
        {
            Yw3dResult resMipLevel = m_Device->CreateVolume(curMipLevelData, width, height, depth, format);
            if (YW3D_FAILED(resMipLevel))
            {
                // Destructor will perform cleanup.
                LOGE(_T("Yw3dVolumeTexture::Create: creation of mip-level failed.\n"));
                return resMipLevel;
            }

            m_MipLevels++;
            curMipLevelData++;

            if (0 == --mipLevels)
            {
                break;
            }

            width >>= 1;
            height >>= 1;
            depth >>= 1;

        } while ((width > 0) && (height > 0) && (depth > 0));

        return Yw3d_S_OK;
    }

    Yw3dTextureSampleInput Yw3dVolumeTexture::GetTextureSampleInput() const
    {
        return Yw3d_TSI_3Coords;
    }

    Yw3dResult Yw3dVolumeTexture::SampleTexture(Vector4& color, float u, float v, float w, const Vector4* xGradient, const Vector4* yGradient, const uint32_t* samplerStates)
    {
        uint32_t texFilter = samplerStates[Yw3d_TSS_MinFilter];
        float texMipLevel = 0.0f;

        // Choose proper mip level and filter.
        if ((nullptr != xGradient) && (nullptr != yGradient))
        {
            const float lenXGrad = xGradient->x * xGradient->x * m_SquaredWidth + xGradient->y * xGradient->y * m_SquaredHeight + xGradient->z * xGradient->z * m_SquaredDepth;
            const float lenYGrad = yGradient->x * yGradient->x * m_SquaredWidth + yGradient->y * yGradient->y * m_SquaredHeight + yGradient->z * yGradient->z * m_SquaredDepth;
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
                m_MipLevelsData[mipLevelA]->SampleLinear(colorA, u, v, w);
                m_MipLevelsData[mipLevelB]->SampleLinear(colorB, u, v, w);
            }
            else
            {
                m_MipLevelsData[mipLevelA]->SamplePoint(colorA, u, v, w);
                m_MipLevelsData[mipLevelB]->SamplePoint(colorB, u, v, w);
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
                m_MipLevelsData[mipLevel]->SampleLinear(color, u, v, w);
            }
            else
            {
                m_MipLevelsData[mipLevel]->SamplePoint(color, u, v, w);
            }
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dVolumeTexture::GenerateMipSubLevels(uint32_t srcLevel)
    {
        if ((srcLevel + 1) > m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::GenerateMipSubLevels: srcLevel refers either to last mip-level or is larger than the number of mip-levels.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Generate by each mip level.
        for (uint32_t level = srcLevel + 1; level < m_MipLevels; level++)
        {
            // Lock mip surface data on source data.
            const float* srcData = nullptr;
            Yw3dResult resLock = LockBox(level - 1, (void**)&srcData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                return resLock;
            }

            // Lock mip surface data on dest data.
            float* destData = nullptr;
            resLock = LockBox(level, (void**)&destData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                UnlockBox(level - 1);
                return resLock;
            }

            // Get original width, height and depth from source mip level.
            const uint32_t srcWidth = GetWidth(level - 1);
            const uint32_t srcHeight = GetHeight(level - 1);
            const uint32_t srcDepth = GetDepth(level - 1);

            switch (GetFormat())
            {
            case Yw3d_FMT_R32F:
                {
                    for (uint32_t idxZ = 0; idxZ < srcDepth; idxZ += 2)
                    {
                        const uint32_t indexSlices[2] = { idxZ * srcWidth * srcHeight, (idxZ + 1) * srcWidth * srcHeight };
                        for (uint32_t idxY = 0; idxY < srcHeight; idxY += 2)
                        {
                            const uint32_t indexRows[2] = { idxY * srcWidth, (idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < srcWidth; idxX += 2, destData++)
                            {
                                const float srcPixels[8] = 
                                {
                                    srcData[indexSlices[0] + indexRows[0] + idxX],
                                    srcData[indexSlices[0] + indexRows[0] + idxX + 1],
                                    srcData[indexSlices[0] + indexRows[1] + idxX],
                                    srcData[indexSlices[0] + indexRows[1] + idxX + 1],
                                    srcData[indexSlices[1] + indexRows[0] + idxX],
                                    srcData[indexSlices[1] + indexRows[0] + idxX + 1],
                                    srcData[indexSlices[1] + indexRows[1] + idxX],
                                    srcData[indexSlices[1] + indexRows[1] + idxX + 1]
                                };

                                *destData = (srcPixels[0] + srcPixels[1] + srcPixels[2] + srcPixels[3] + 
                                    srcPixels[4] + srcPixels[5] + srcPixels[6] + srcPixels[7]) * 0.125f;
                            }
                        }
                    }
                }
                break;
            case Yw3d_FMT_R32G32F:
                {
                    for (uint32_t idxZ = 0; idxZ < srcDepth; idxZ += 2)
                    {
                        const uint32_t indexSlices[2] = { idxZ * srcWidth * srcHeight, (idxZ + 1) * srcWidth * srcHeight };
                        for (uint32_t idxY = 0; idxY < srcHeight; idxY += 2)
                        {
                            const uint32_t indexRows[2] = { idxY * srcWidth, (idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < srcWidth; idxX += 2, destData += 2)
                            {
                                const Vector2* srcPixels[8] =
                                {
                                    &((Vector2*)srcData)[indexSlices[0] + indexRows[0] + idxX],
                                    &((Vector2*)srcData)[indexSlices[0] + indexRows[0] + idxX + 1],
                                    &((Vector2*)srcData)[indexSlices[0] + indexRows[1] + idxX],
                                    &((Vector2*)srcData)[indexSlices[0] + indexRows[1] + idxX + 1],
                                    &((Vector2*)srcData)[indexSlices[1] + indexRows[0] + idxX],
                                    &((Vector2*)srcData)[indexSlices[1] + indexRows[0] + idxX + 1],
                                    &((Vector2*)srcData)[indexSlices[1] + indexRows[1] + idxX],
                                    &((Vector2*)srcData)[indexSlices[1] + indexRows[1] + idxX + 1]
                                };

                                *(Vector2*)destData = (*srcPixels[0] + *srcPixels[1] + *srcPixels[2] + *srcPixels[3] +
                                    *srcPixels[4] + *srcPixels[5] + *srcPixels[6] + *srcPixels[7]) * 0.125f;
                            }
                        }
                    }
                }
                break;
            case Yw3d_FMT_R32G32B32F:
                {
                    for (uint32_t idxZ = 0; idxZ < srcDepth; idxZ += 2)
                    {
                        const uint32_t indexSlices[2] = { idxZ * srcWidth * srcHeight, (idxZ + 1) * srcWidth * srcHeight };
                        for (uint32_t idxY = 0; idxY < srcHeight; idxY += 2)
                        {
                            const uint32_t indexRows[2] = { idxY * srcWidth, (idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < srcWidth; idxX += 2, destData += 3)
                            {
                                const Vector3* srcPixels[8] =
                                {
                                    &((Vector3*)srcData)[indexSlices[0] + indexRows[0] + idxX],
                                    &((Vector3*)srcData)[indexSlices[0] + indexRows[0] + idxX + 1],
                                    &((Vector3*)srcData)[indexSlices[0] + indexRows[1] + idxX],
                                    &((Vector3*)srcData)[indexSlices[0] + indexRows[1] + idxX + 1],
                                    &((Vector3*)srcData)[indexSlices[1] + indexRows[0] + idxX],
                                    &((Vector3*)srcData)[indexSlices[1] + indexRows[0] + idxX + 1],
                                    &((Vector3*)srcData)[indexSlices[1] + indexRows[1] + idxX],
                                    &((Vector3*)srcData)[indexSlices[1] + indexRows[1] + idxX + 1]
                                };

                                *(Vector3*)destData = (*srcPixels[0] + *srcPixels[1] + *srcPixels[2] + *srcPixels[3] +
                                    *srcPixels[4] + *srcPixels[5] + *srcPixels[6] + *srcPixels[7]) * 0.125f;
                            }
                        }
                    }
                }
                break;
            case Yw3d_FMT_R32G32B32A32F:
                {
                    for (uint32_t idxZ = 0; idxZ < srcDepth; idxZ += 2)
                    {
                        const uint32_t indexSlices[2] = { idxZ * srcWidth * srcHeight, (idxZ + 1) * srcWidth * srcHeight };
                        for (uint32_t idxY = 0; idxY < srcHeight; idxY += 2)
                        {
                            const uint32_t indexRows[2] = { idxY * srcWidth, (idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < srcWidth; idxX += 2, destData += 4)
                            {
                                const Vector4* srcPixels[8] =
                                {
                                    &((Vector4*)srcData)[indexSlices[0] + indexRows[0] + idxX],
                                    &((Vector4*)srcData)[indexSlices[0] + indexRows[0] + idxX + 1],
                                    &((Vector4*)srcData)[indexSlices[0] + indexRows[1] + idxX],
                                    &((Vector4*)srcData)[indexSlices[0] + indexRows[1] + idxX + 1],
                                    &((Vector4*)srcData)[indexSlices[1] + indexRows[0] + idxX],
                                    &((Vector4*)srcData)[indexSlices[1] + indexRows[0] + idxX + 1],
                                    &((Vector4*)srcData)[indexSlices[1] + indexRows[1] + idxX],
                                    &((Vector4*)srcData)[indexSlices[1] + indexRows[1] + idxX + 1]
                                };

                                *(Vector4*)destData = (*srcPixels[0] + *srcPixels[1] + *srcPixels[2] + *srcPixels[3] +
                                    *srcPixels[4] + *srcPixels[5] + *srcPixels[6] + *srcPixels[7]) * 0.125f;
                            }
                        }
                    }
                }
                break;
            default:
                // This can not happen.
                break;
            }

            // Unlock data.
            UnlockBox(level);
            UnlockBox(level - 1);
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dVolumeTexture::Clear(uint32_t mipLevel, const Vector4& color, const Yw3dBox* box)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::Clear: invalid mip-level specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_MipLevelsData[mipLevel]->Clear(color, box);
    }

    Yw3dResult Yw3dVolumeTexture::LockBox(uint32_t mipLevel, void** data, const Yw3dBox* box)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::LockBox: invalid mip-level specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_MipLevelsData[mipLevel]->LockBox(data, box);
    }

    Yw3dResult Yw3dVolumeTexture::UnlockBox(uint32_t mipLevel)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::UnlockBox: invalid mip-level specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_MipLevelsData[mipLevel]->UnlockBox();
    }

    Yw3dVolume* Yw3dVolumeTexture::AcquireMipLevel(uint32_t mipLevel)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::AcquireMipLevel: invalid mip-level specified.\n"));
            return nullptr;
        }

        m_MipLevelsData[mipLevel]->AddRef();
        return m_MipLevelsData[mipLevel];
    }

    Yw3dFormat Yw3dVolumeTexture::GetFormat() const
    {
        return m_MipLevelsData[0]->GetFormat();
    }

    uint32_t Yw3dVolumeTexture::GetFormatFloats() const
    {
        return m_MipLevelsData[0]->GetFormatFloats();
    }

    uint32_t Yw3dVolumeTexture::GetMipLevels() const
    {
        return m_MipLevels;
    }

    uint32_t Yw3dVolumeTexture::GetWidth(uint32_t mipLevel) const
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::GetWidth: invalid mip-level specified.\n"));
            return 0;
        }

        return m_MipLevelsData[mipLevel]->GetWidth();
    }

    uint32_t Yw3dVolumeTexture::GetHeight(uint32_t mipLevel) const
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::GetHeight: invalid mip-level specified.\n"));
            return 0;
        }

        return m_MipLevelsData[mipLevel]->GetHeight();
    }

    uint32_t Yw3dVolumeTexture::GetDepth(uint32_t mipLevel) const
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dVolumeTexture::GetDepth: invalid mip-level specified.\n"));
            return 0;
        }

        return m_MipLevelsData[mipLevel]->GetDepth();
    }
}
