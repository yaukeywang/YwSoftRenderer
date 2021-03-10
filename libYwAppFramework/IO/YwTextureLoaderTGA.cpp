// Add by Yaukey at 2019-08-20.
// YW texture loader for tga class.

#include "YwTextureLoaderTGA.h"
#include "Yw3d.h"
#include "libtarga.h"

namespace yw
{
    TextureLoaderTGA::TextureLoaderTGA() : 
        ITextureLoader()
    {

    }

    TextureLoaderTGA::~TextureLoaderTGA()
    {

    }

    bool TextureLoaderTGA::LoadFromData(const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, Yw3dTexture** texture)
    {
        int32_t texWidth = 0;
        int32_t texHeight = 0;
        int32_t alphaBits = 0;
        uint8_t* rawData = (uint8_t*)tga_load_data((void*)data, dataLength, &texWidth, &texHeight, &alphaBits, TGA_TRUECOLOR_32);
        if (nullptr == rawData)
        {
            return false;
        }

        bool hasAlpha = (alphaBits > 0);
        int32_t bbp = 4;
        int32_t pitch = bbp * texWidth;
        Yw3dFormat textureFormat = hasAlpha ? Yw3d_FMT_R32G32B32A32F : Yw3d_FMT_R32G32B32F;

        // Create texture from device.
        YW_SAFE_RELEASE(*texture);
        if (YW3D_FAILED(device->CreateTexture(texture, texWidth, texHeight, 0, textureFormat)))
        {
            return false;
        }

        // Lock texture data.
        float* textureData = nullptr;
        Yw3dResult resLock = (*texture)->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        // Normalized color scale.
        const float colorScale = 1.0f / 255.0f;

        // Fill data.
        // Tga file line is from bottom to top.
        for (int32_t yIdx = 0; yIdx < texHeight; yIdx++)
        {
            for (int32_t xIdx = 0; xIdx < texWidth; xIdx++)
            {
                int32_t texIndex = (texHeight - 1 - yIdx) * texWidth + xIdx;
                int32_t bmpIndex = yIdx * pitch + xIdx * bbp;

                if (hasAlpha)
                {
                    Vector4* texData = (Vector4*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(rawData + bmpIndex);
                    texData->r = (float)((*bmpData) * colorScale);
                    texData->g = (float)((*(bmpData + 1)) * colorScale);
                    texData->b = (float)((*(bmpData + 2)) * colorScale);
                    texData->a = (float)((*(bmpData + 3)) * colorScale);
                }
                else
                {
                    Vector3* texData = (Vector3*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(rawData + bmpIndex);
                    texData->r = (float)((*bmpData) * colorScale);
                    texData->g = (float)((*(bmpData + 1)) * colorScale);
                    texData->b = (float)((*(bmpData + 2)) * colorScale);
                }
            }
        }

        // Unlock texture.
        (*texture)->UnlockRect(0);

        // Release tga image data.
        tga_release(rawData);
        rawData = nullptr;

        return true;
    }
}
