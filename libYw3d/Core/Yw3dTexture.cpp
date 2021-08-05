// Add by Yaukey at 2018-05-07.
// YW Soft Renderer texture class.

#include "Yw3dTexture.h"
#include "Yw3dDevice.h"
#include "Yw3dSurface.h"

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
            // Get max mipmap levels.
            mipLevels = (uint32_t)floor(log2(max(width, height))) + 1;
        }

        m_MipLevelsData = new Yw3dSurface*[mipLevels];
        if (nullptr == m_MipLevelsData)
        {
            LOGE(_T("Yw3dTexture::Create: out of memory, cannot create mip-levels.\n"));
            return Yw3d_E_OutOfMemory;
        }

        // Zero surface memory.
        memset(m_MipLevelsData, 0, sizeof(Yw3dSurface*) * mipLevels);

        // Create surface for each mipmap level.
        Yw3dSurface** curMipLevelData = m_MipLevelsData;
        do
        {
            Yw3dResult resMipLevel = m_Device->CreateSurface(curMipLevelData, width, height, format);
            if (YW3D_FAILED(resMipLevel))
            {
                // Destructor will perform cleanup.
                LOGE(_T("Yw3dTexture::Create: creation of mip-level failed.\n"));
                return resMipLevel;
            }

            ++m_MipLevels;
            ++curMipLevelData;

            if (0 == --mipLevels)
            {
                break;
            }

            width = (uint32_t)floor(width >> 1);
            height = (uint32_t)floor(height >> 1);
        } while ((0 != width) && (0 != height));

        return Yw3d_S_OK;
    }

    Yw3dTextureSampleInput Yw3dTexture::GetTextureSampleInput() const
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

            const float interpolation = texMipLevel - (float)mipLevelA; // TODO: Not accurate.
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
        if ((srcLevel + 1) > m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::GenerateMipSubLevels: srcLevel refers either to last mip-level or is larger than the number of mip-levels.\n"));
            return Yw3d_E_InvalidParameters;
        }

        // Generate by each mip level.
        for (uint32_t level = srcLevel + 1; level < m_MipLevels; level++)
        {
            // Lock mip surface data on source data.
            const float* srcData = nullptr;
            Yw3dResult resLock = LockRect(level - 1, (void**)&srcData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                return resLock;
            }

            // Lock mip surface data on dest data.
            float* destData = nullptr;
            resLock = LockRect(level, (void**)&destData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                UnlockRect(level - 1);
                return resLock;
            }

            // Get original width and height from source mip level.
            const uint32_t srcWidth = GetWidth(level - 1);
            const uint32_t srcHeight = GetHeight(level - 1);
            const uint32_t dstWidth = GetWidth(level);
            const uint32_t dstHeight = GetHeight(level);

            switch (GetFormat())
            {
            case Yw3d_FMT_R32F:
                {
                    for (uint32_t idxY = 0; idxY < dstHeight; idxY++)
                    {
                        if (DetermineIfPowerOf2(srcHeight))
                        {
                            const uint32_t indexRows[2] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData++)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const float srcPixels[4] =
                                    {
                                        srcData[indexRows[0] + (2 * idxX)],
                                        srcData[indexRows[0] + (2 * idxX + 1)],
                                        srcData[indexRows[1] + (2 * idxX)],
                                        srcData[indexRows[1] + (2 * idxX + 1)]
                                    };

                                    *destData = (srcPixels[0] + srcPixels[1] + srcPixels[2] + srcPixels[3]) * 0.25f;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const float row0 = w0 * srcData[indexRows[0] + (2 * idxX)] + w1 * srcData[indexRows[0] + (2 * idxX + 1)] + w2 * srcData[indexRows[0] + (2 * idxX + 2)];
                                    const float row1 = w0 * srcData[indexRows[1] + (2 * idxX)] + w1 * srcData[indexRows[1] + (2 * idxX + 1)] + w2 * srcData[indexRows[1] + (2 * idxX + 2)];

                                    *destData = (row0 + row1) * 0.5f;
                                }
                            }
                        }
                        else
                        {
                            const float h0 = (float)(dstHeight - idxY) / (float)(2 * dstHeight + 1);
                            const float h1 = (float)dstHeight / (2 * dstHeight + 1);
                            const float h2 = (float)(1 + idxY) / (2 * dstHeight + 1);

                            const uint32_t indexRows[3] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth, (2 * idxY + 2) * srcWidth };

                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData++)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const float row0 = (srcData[indexRows[0] + (2 * idxX)] + srcData[indexRows[0] + (2 * idxX + 1)]) * 0.5f;
                                    const float row1 = (srcData[indexRows[1] + (2 * idxX)] + srcData[indexRows[1] + (2 * idxX + 1)]) * 0.5f;
                                    const float row2 = (srcData[indexRows[2] + (2 * idxX)] + srcData[indexRows[2] + (2 * idxX + 1)]) * 0.5f;

                                    *destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const float row0 = w0 * srcData[indexRows[0] + (2 * idxX)] + w1 * srcData[indexRows[0] + (2 * idxX + 1)] + w2 * srcData[indexRows[0] + (2 * idxX + 2)];
                                    const float row1 = w0 * srcData[indexRows[1] + (2 * idxX)] + w1 * srcData[indexRows[1] + (2 * idxX + 1)] + w2 * srcData[indexRows[1] + (2 * idxX + 2)];
                                    const float row2 = w0 * srcData[indexRows[2] + (2 * idxX)] + w1 * srcData[indexRows[2] + (2 * idxX + 1)] + w2 * srcData[indexRows[2] + (2 * idxX + 2)];

                                    *destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                            }
                        }
                    }
                }
                break;
            case Yw3d_FMT_R32G32F:
                {
                    for (uint32_t idxY = 0; idxY < dstHeight; idxY++)
                    {
                        if (DetermineIfPowerOf2(srcHeight))
                        {
                            const uint32_t indexRows[2] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData += 2)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const Vector2* srcPixels[4] =
                                    {
                                        &((Vector2*)srcData)[indexRows[0] + (2 * idxX)],
                                        &((Vector2*)srcData)[indexRows[0] + (2 * idxX + 1)],
                                        &((Vector2*)srcData)[indexRows[1] + (2 * idxX)],
                                        &((Vector2*)srcData)[indexRows[1] + (2 * idxX + 1)]
                                    };

                                    *(Vector2*)destData = (*srcPixels[0] + *srcPixels[1] + *srcPixels[2] + *srcPixels[3]) * 0.25f;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const Vector2 row0 = w0 * ((Vector2*)srcData)[indexRows[0] + (2 * idxX)] + w1 * ((Vector2*)srcData)[indexRows[0] + (2 * idxX + 1)] + w2 * ((Vector2*)srcData)[indexRows[0] + (2 * idxX + 2)];
                                    const Vector2 row1 = w0 * ((Vector2*)srcData)[indexRows[1] + (2 * idxX)] + w1 * ((Vector2*)srcData)[indexRows[1] + (2 * idxX + 1)] + w2 * ((Vector2*)srcData)[indexRows[1] + (2 * idxX + 2)];

                                    *(Vector2*)destData = (row0 + row1) * 0.5f;
                                }
                            }
                        }
                        else
                        {
                            const float h0 = (float)(dstHeight - idxY) / (float)(2 * dstHeight + 1);
                            const float h1 = (float)dstHeight / (2 * dstHeight + 1);
                            const float h2 = (float)(1 + idxY) / (2 * dstHeight + 1);

                            const uint32_t indexRows[3] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth, (2 * idxY + 2) * srcWidth };

                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData += 2)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const Vector2 row0 = (((Vector2*)srcData)[indexRows[0] + (2 * idxX)] + ((Vector2*)srcData)[indexRows[0] + (2 * idxX + 1)]) * 0.5f;
                                    const Vector2 row1 = (((Vector2*)srcData)[indexRows[1] + (2 * idxX)] + ((Vector2*)srcData)[indexRows[1] + (2 * idxX + 1)]) * 0.5f;
                                    const Vector2 row2 = (((Vector2*)srcData)[indexRows[2] + (2 * idxX)] + ((Vector2*)srcData)[indexRows[2] + (2 * idxX + 1)]) * 0.5f;

                                    *(Vector2*)destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const Vector2 row0 = w0 * ((Vector2*)srcData)[indexRows[0] + (2 * idxX)] + w1 * ((Vector2*)srcData)[indexRows[0] + (2 * idxX + 1)] + w2 * ((Vector2*)srcData)[indexRows[0] + (2 * idxX + 2)];
                                    const Vector2 row1 = w0 * ((Vector2*)srcData)[indexRows[1] + (2 * idxX)] + w1 * ((Vector2*)srcData)[indexRows[1] + (2 * idxX + 1)] + w2 * ((Vector2*)srcData)[indexRows[1] + (2 * idxX + 2)];
                                    const Vector2 row2 = w0 * ((Vector2*)srcData)[indexRows[2] + (2 * idxX)] + w1 * ((Vector2*)srcData)[indexRows[2] + (2 * idxX + 1)] + w2 * ((Vector2*)srcData)[indexRows[2] + (2 * idxX + 2)];

                                    *(Vector2*)destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                            }
                        }
                    }
                }
                break;
            case Yw3d_FMT_R32G32B32F:
                {
                    for (uint32_t idxY = 0; idxY < dstHeight; idxY++)
                    {
                        if (DetermineIfPowerOf2(srcHeight))
                        {
                            const uint32_t indexRows[2] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth };
                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData += 3)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const Vector3* srcPixels[4] =
                                    {
                                        &((Vector3*)srcData)[indexRows[0] + (2 * idxX)],
                                        &((Vector3*)srcData)[indexRows[0] + (2 * idxX + 1)],
                                        &((Vector3*)srcData)[indexRows[1] + (2 * idxX)],
                                        &((Vector3*)srcData)[indexRows[1] + (2 * idxX + 1)]
                                    };

                                    *(Vector3*)destData = (*srcPixels[0] + *srcPixels[1] + *srcPixels[2] + *srcPixels[3]) * 0.25f;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const Vector3 row0 = w0 * ((Vector3*)srcData)[indexRows[0] + (2 * idxX)] + w1 * ((Vector3*)srcData)[indexRows[0] + (2 * idxX + 1)] + w2 * ((Vector3*)srcData)[indexRows[0] + (2 * idxX + 2)];
                                    const Vector3 row1 = w0 * ((Vector3*)srcData)[indexRows[1] + (2 * idxX)] + w1 * ((Vector3*)srcData)[indexRows[1] + (2 * idxX + 1)] + w2 * ((Vector3*)srcData)[indexRows[1] + (2 * idxX + 2)];

                                    *(Vector3*)destData = (row0 + row1) * 0.5f;
                                }
                            }
                        }
                        else
                        {
                            const float h0 = (float)(dstHeight - idxY) / (float)(2 * dstHeight + 1);
                            const float h1 = (float)dstHeight / (2 * dstHeight + 1);
                            const float h2 = (float)(1 + idxY) / (2 * dstHeight + 1);

                            const uint32_t indexRows[3] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth, (2 * idxY + 2) * srcWidth };

                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData += 3)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const Vector3 row0 = (((Vector3*)srcData)[indexRows[0] + (2 * idxX)] + ((Vector3*)srcData)[indexRows[0] + (2 * idxX + 1)]) * 0.5f;
                                    const Vector3 row1 = (((Vector3*)srcData)[indexRows[1] + (2 * idxX)] + ((Vector3*)srcData)[indexRows[1] + (2 * idxX + 1)]) * 0.5f;
                                    const Vector3 row2 = (((Vector3*)srcData)[indexRows[2] + (2 * idxX)] + ((Vector3*)srcData)[indexRows[2] + (2 * idxX + 1)]) * 0.5f;

                                    *(Vector3*)destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const Vector3 row0 = w0 * ((Vector3*)srcData)[indexRows[0] + (2 * idxX)] + w1 * ((Vector3*)srcData)[indexRows[0] + (2 * idxX + 1)] + w2 * ((Vector3*)srcData)[indexRows[0] + (2 * idxX + 2)];
                                    const Vector3 row1 = w0 * ((Vector3*)srcData)[indexRows[1] + (2 * idxX)] + w1 * ((Vector3*)srcData)[indexRows[1] + (2 * idxX + 1)] + w2 * ((Vector3*)srcData)[indexRows[1] + (2 * idxX + 2)];
                                    const Vector3 row2 = w0 * ((Vector3*)srcData)[indexRows[2] + (2 * idxX)] + w1 * ((Vector3*)srcData)[indexRows[2] + (2 * idxX + 1)] + w2 * ((Vector3*)srcData)[indexRows[2] + (2 * idxX + 2)];

                                    *(Vector3*)destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                            }
                        }
                    }
                }
                break;
            case Yw3d_FMT_R32G32B32A32F:
                {
                    for (uint32_t idxY = 0; idxY < dstHeight; idxY++)
                    {
                        if (DetermineIfPowerOf2(srcHeight))
                        {
                            const uint32_t indexRows[2] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth};
                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData += 4)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const Vector4* srcPixels[4] =
                                    {
                                        &((Vector4*)srcData)[indexRows[0] + (2 * idxX)],
                                        &((Vector4*)srcData)[indexRows[0] + (2 * idxX + 1)],
                                        &((Vector4*)srcData)[indexRows[1] + (2 * idxX)],
                                        &((Vector4*)srcData)[indexRows[1] + (2 * idxX + 1)]
                                    };

                                    *(Vector4*)destData = (*srcPixels[0] + *srcPixels[1] + *srcPixels[2] + *srcPixels[3]) * 0.25f;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const Vector4 row0 = w0 * ((Vector4*)srcData)[indexRows[0] + (2 * idxX)] + w1 * ((Vector4*)srcData)[indexRows[0] + (2 * idxX + 1)] + w2 * ((Vector4*)srcData)[indexRows[0] + (2 * idxX + 2)];
                                    const Vector4 row1 = w0 * ((Vector4*)srcData)[indexRows[1] + (2 * idxX)] + w1 * ((Vector4*)srcData)[indexRows[1] + (2 * idxX + 1)] + w2 * ((Vector4*)srcData)[indexRows[1] + (2 * idxX + 2)];

                                    *(Vector4*)destData = (row0 + row1) * 0.5f;
                                }
                            }
                        }
                        else
                        {
                            const float h0 = (float)(dstHeight - idxY) / (float)(2 * dstHeight + 1);
                            const float h1 = (float)dstHeight / (2 * dstHeight + 1);
                            const float h2 = (float)(1 + idxY) / (2 * dstHeight + 1);

                            const uint32_t indexRows[3] = { 2 * idxY * srcWidth, (2 * idxY + 1) * srcWidth, (2 * idxY + 2) * srcWidth };

                            for (uint32_t idxX = 0; idxX < dstWidth; idxX++, destData += 4)
                            {
                                if (DetermineIfPowerOf2(srcWidth))
                                {
                                    const Vector4 row0 = (((Vector4*)srcData)[indexRows[0] + (2 * idxX)] + ((Vector4*)srcData)[indexRows[0] + (2 * idxX + 1)]) * 0.5f;
                                    const Vector4 row1 = (((Vector4*)srcData)[indexRows[1] + (2 * idxX)] + ((Vector4*)srcData)[indexRows[1] + (2 * idxX + 1)]) * 0.5f;
                                    const Vector4 row2 = (((Vector4*)srcData)[indexRows[2] + (2 * idxX)] + ((Vector4*)srcData)[indexRows[2] + (2 * idxX + 1)]) * 0.5f;

                                    *(Vector4*)destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
                                else
                                {
                                    const float w0 = (float)(dstWidth - idxX) / (float)(2.0f * dstWidth + 1.0f);
                                    const float w1 = (float)dstWidth / (2.0f * dstWidth + 1.0f);
                                    const float w2 = (float)(1 + idxX) / (2.0f * dstWidth + 1.0f);

                                    const Vector4 row0 = w0 * ((Vector4*)srcData)[indexRows[0] + (2 * idxX)] + w1 * ((Vector4*)srcData)[indexRows[0] + (2 * idxX + 1)] + w2 * ((Vector4*)srcData)[indexRows[0] + (2 * idxX + 2)];
                                    const Vector4 row1 = w0 * ((Vector4*)srcData)[indexRows[1] + (2 * idxX)] + w1 * ((Vector4*)srcData)[indexRows[1] + (2 * idxX + 1)] + w2 * ((Vector4*)srcData)[indexRows[1] + (2 * idxX + 2)];
                                    const Vector4 row2 = w0 * ((Vector4*)srcData)[indexRows[2] + (2 * idxX)] + w1 * ((Vector4*)srcData)[indexRows[2] + (2 * idxX + 1)] + w2 * ((Vector4*)srcData)[indexRows[2] + (2 * idxX + 2)];

                                    *(Vector4*)destData = h0 * row0 + h1 * row1 + h2 * row2;
                                }
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
            UnlockRect(level);
            UnlockRect(level - 1);
        }

        return Yw3d_S_OK;
    }

    Yw3dResult Yw3dTexture::Clear(uint32_t mipLevel, const Vector4& color, const Yw3dRect* rect)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::Clear: invalid mip-level specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_MipLevelsData[mipLevel]->Clear(color, rect);
    }

    Yw3dResult Yw3dTexture::LockRect(uint32_t mipLevel, void** data, const Yw3dRect* rect)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::LockRect: invalid mip-level specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_MipLevelsData[mipLevel]->LockRect(data, rect);
    }

    Yw3dResult Yw3dTexture::UnlockRect(uint32_t mipLevel)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::UnlockRect: invalid mip-level specified.\n"));
            return Yw3d_E_InvalidParameters;
        }

        return m_MipLevelsData[mipLevel]->UnlockRect();
    }

    Yw3dSurface* Yw3dTexture::AcquireMipLevel(uint32_t mipLevel)
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::GetMipLevel: invalid mip-level specified.\n"));
            return nullptr;
        }

        m_MipLevelsData[mipLevel]->AddRef();
        return m_MipLevelsData[mipLevel];
    }

    Yw3dFormat Yw3dTexture::GetFormat() const
    {
        return m_MipLevelsData[0]->GetFormat();
    }

    uint32_t Yw3dTexture::GetFormatFloats() const
    {
        return m_MipLevelsData[0]->GetFormatFloats();
    }

    uint32_t Yw3dTexture::GetMipLevels() const
    {
        return m_MipLevels;
    }

    uint32_t Yw3dTexture::GetWidth(uint32_t mipLevel) const
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::GetWidth: invalid mip-level specified.\n"));
            return 0;
        }

        return m_MipLevelsData[mipLevel]->GetWidth();
    }

    uint32_t Yw3dTexture::GetHeight(uint32_t mipLevel) const
    {
        if (mipLevel >= m_MipLevels)
        {
            LOGE(_T("Yw3dTexture::GetHeight: invalid mip-level specified.\n"));
            return 0;
        }

        return m_MipLevelsData[mipLevel]->GetHeight();
    }
}
