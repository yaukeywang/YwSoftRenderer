// Add by Yaukey at 2019-08-04.
// YW texture loader for bmp class.
// NOTE:
// 1. Compression formats are not supported;
// 2. Color table is not supported;
// 3. Only support 24-bit/32-bit depth true color format.

#include "YwTextureLoaderBMP.h"
#include "Yw3d.h"
#include "bmp.h"

namespace yw
{
    // ------------------------------------------------------------------
    // For texture loader.

    TextureLoaderBMP::TextureLoaderBMP() :
        ITextureLoader()
    {

    }

    TextureLoaderBMP::~TextureLoaderBMP()
    {

    }

    bool TextureLoaderBMP::LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, IYw3dBaseTexture** texture)
    {
        // Read bit map file header.
        BitMapFileHeader* fileHeader = (BitMapFileHeader*)data;
        if (BITMAP_FILE_MAGIC != fileHeader->bfType)
        {
            // Compare file header magic.
            return false;
        }

        // Read bit map info.
        BitMapInfoHeader* infoHeader = (BitMapInfoHeader*)(data + sizeof(BitMapFileHeader));
        int32_t texWidth = infoHeader->biWidth;
        int32_t texHeight = infoHeader->biHeight;
        Yw3dFormat textureFormat = (32 == infoHeader->biBitCount) ? Yw3d_FMT_R32G32B32A32F : Yw3d_FMT_R32G32B32F;
        uint16_t bbp = infoHeader->biBitCount / 8;
        int32_t pitch = ((texWidth * bbp) + 3) / 4 * 4;

        // Read texture data.
        uint8_t* texDataRaw = (uint8_t*)(data + fileHeader->bfOffBits);
        //uint8_t* texDataStart = (uint8_t*)(data + sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader));

        // Convert texture dynamic instance class.
        Yw3dTexture** inputTexture = (Yw3dTexture**)texture;

        // Create texture from device.
        YW_SAFE_RELEASE(*inputTexture);
        if (YW3D_FAILED(device->CreateTexture(inputTexture, texWidth, texHeight, 0, textureFormat)))
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
        const float colorScale = 1.0f / 255.0f;

        // Fill data.
        uint8_t* srcTextureData = texDataRaw;
        for (int32_t yIdx = 0; yIdx < texHeight; yIdx++)
        {
            for (int32_t xIdx = 0; xIdx < texWidth; xIdx++)
            {
                int32_t texIndex = (texHeight - 1 - yIdx) * texWidth + xIdx;
                int32_t bmpIndex = yIdx * pitch + xIdx * bbp;
                
                if (Yw3d_FMT_R32G32B32A32F == textureFormat)
                {
                    Vector4* texData = (Vector4*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(srcTextureData + bmpIndex);
                    texData->b = (float)((*bmpData) * colorScale);
                    texData->g = (float)((*(bmpData + 1)) * colorScale);
                    texData->r = (float)((*(bmpData + 2)) * colorScale);
                    texData->a = (float)((*(bmpData + 3)) * colorScale);
                }
                else
                {
                    Vector3* texData = (Vector3*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(srcTextureData + bmpIndex);
                    texData->b = (float)((*bmpData) * colorScale);
                    texData->g = (float)((*(bmpData + 1)) * colorScale);
                    texData->r = (float)((*(bmpData + 2)) * colorScale);
                }
            }
        }

        // Unlock texture.
        (*inputTexture)->UnlockRect(0);

        return true;
    }

    bool TextureLoaderBMP::GenerateMipmap(IYw3dBaseTexture* texture)
    {
        return GenerateTextureMipmap(texture);
    }
}
