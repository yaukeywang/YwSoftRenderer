// Add by Yaukey at 2019-08-04.
// YW texture loader for ywt class.

#include "YwTextureLoaderYWT.h"
#include "Yw3d.h"
#include "ywt.h"

namespace yw
{
    // ------------------------------------------------------------------
    // For texture loader.

    TextureLoaderYWT::TextureLoaderYWT() :
        ITextureLoader()
    {

    }

    TextureLoaderYWT::~TextureLoaderYWT()
    {

    }

    bool TextureLoaderYWT::LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, IYw3dBaseTexture** texture)
    {
        // Read raw data to internal format.
        YwTextureData textureData;
        if (!LoadYwTextureFromData(data, dataLength, textureData))
        {
            return false;
        }

        if ((0 == textureData.width) || (0 == textureData.height))
        {
            return false;
        }

        Yw3dFormat textureFormat = (Yw3dFormat)textureData.format;
        if ((textureFormat < Yw3d_FMT_R32F) || (textureFormat > Yw3d_FMT_R32G32B32A32F))
        {
            return false;
        }

        // Convert texture dynamic instance class.
        Yw3dTexture** inputTexture = (Yw3dTexture**)texture;

        // Create texture from device.
        YW_SAFE_RELEASE(*inputTexture);
        if (YW3D_FAILED(device->CreateTexture(inputTexture, textureData.width, textureData.height, (uint32_t)textureData.mipsData.size(), textureFormat)))
        {
            return false;
        }

        // Lock texture mip data and apply.
        Yw3dTexture* rawTexture = *inputTexture;
        for (uint8_t i = 0; i < (uint8_t)textureData.mipsData.size(); i++)
        {
            const YwTextureMipData& textureMipData = textureData.mipsData[i];
            if (textureMipData.mipLevel != i)
            {
                LOGE(_T("TextureLoaderYWT.LoadFromData: Mip levels are not match."));
                return false;
            }

            if ((textureMipData.mipWidth != rawTexture->GetWidth(i)) || (textureMipData.mipHeight != rawTexture->GetHeight(i)))
            {
                LOGE(_T("TextureLoaderYWT.LoadFromData: Mip level dimension are not match."));
                return false;
            }

            float* textureData = nullptr;
            Yw3dResult resLock = rawTexture->LockRect(i, (void**)&textureData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                YW_SAFE_RELEASE(*texture);
                return false;
            }

            // Copy mip texture data.
            memcpy(textureData, textureMipData.mipData.data(), textureMipData.mipData.size());

            // Unlock texture.
            rawTexture->UnlockRect(i);
        }

        return true;
    }

    bool TextureLoaderYWT::GenerateMipmap(IYw3dBaseTexture* texture)
    {
        // Raw texture data has already got mipmap data, we do not need to generate it runtime anymore.
        return true;
    }
}
