// Add by Yaukey at 2021-03-09.
// YW texture loader for rgbe class.

#include "YwTextureLoaderRGBE.h"
#include "Yw3d.h"
#include "rgbe.h"

namespace yw
{
    // ------------------------------------------------------------------
    // For texture loader.

    TextureLoaderRGBE::TextureLoaderRGBE() :
        ITextureLoader()
    {

    }

    TextureLoaderRGBE::~TextureLoaderRGBE()
    {

    }

    bool TextureLoaderRGBE::LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, IYw3dBaseTexture** texture)
    {
        // Get file header first.
        RGBEHeader header;
        int32_t headerSize = RGBEReadHeaderFromData(data, &header);
        if (headerSize < 0)
        {
            LOGE(_T("TextureLoaderRGBE.LoadFromData: Wrong RGBE file header."));
            return false;
        }

        // Read texture data.
        uint8_t* texDataRLE = (uint8_t*)(data + headerSize);
        int32_t texWidth = header.width;
        int32_t texHeight = header.height;
        int32_t bbp = 3;
        int32_t pitch = bbp * texWidth;

        float* texDataRaw = new float[texWidth * texHeight * bbp];
        if (!RGBEReadPixelsRLEFromData(texDataRLE, texDataRaw, texWidth, texHeight))
        {
            return false;
        }

        // Convert texture dynamic instance class.
        Yw3dTexture** inputTexture = (Yw3dTexture**)texture;

        // Create texture from device.
        YW_SAFE_RELEASE(*inputTexture);
        if (YW3D_FAILED(device->CreateTexture(inputTexture, texWidth, texHeight, 0, Yw3d_FMT_R32G32B32F)))
        {
            return false;
        }

        // Lock texture data.
        float* textureData = nullptr;
        Yw3dResult resLock = (*inputTexture)->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        // Normalized color scale.
        const float colorScale = 1.0f; // 1.0f / 255.0f;

        // Fill data.
        float* srcTextureData = texDataRaw;
        for (int32_t yIdx = 0; yIdx < texHeight; yIdx++)
        {
            for (int32_t xIdx = 0; xIdx < texWidth; xIdx++)
            {
                int32_t texIndex = yIdx * texWidth + xIdx;
                int32_t hdrIndex = yIdx * pitch + xIdx * bbp;

                Vector3* texData = (Vector3*)textureData + texIndex;
                float* hdrData = srcTextureData + hdrIndex;
                texData->r = (float)((*hdrData) * colorScale);
                texData->g = (float)((*(hdrData + 1)) * colorScale);
                texData->b = (float)((*(hdrData + 2)) * colorScale);
            }
        }

        // Unlock texture.
        (*inputTexture)->UnlockRect(0);

        // Release raw image data.
        YW_SAFE_DELETE_ARRAY(texDataRaw);

        return true;
    }

    bool TextureLoaderRGBE::GenerateMipmap(IYw3dBaseTexture* texture)
    {
        return GenerateTextureMipmap(texture);
    }
}
