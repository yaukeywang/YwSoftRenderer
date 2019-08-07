// Add by Yaukey at 2019-08-04.
// YW texture loader for bmp class.

#include "YwTextureLoaderBmp.h"
#include "Yw3d.h"
#include "YwFileIO.h"

namespace yw
{
    // ------------------------------------------------------------------
    // For bit map info struct.

    // File magic number.
    const uint16_t BITMAP_FILE_MAGIC = ('M' << 8) | 'B';

    // Bit map file header.
    #pragma pack(push, 1)
    struct BitMapFileHeader
    {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    };
    #pragma pack(pop)

    // Bit map info header.
    #pragma pack(push, 1)
    struct BitMapInfoHeader
    {
        uint32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t biXPelsPerMeter;
        int32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    };
    #pragma pack(pop)

    // ------------------------------------------------------------------
    // For texture loader.

    TextureLoaderBmp::TextureLoaderBmp() :
        ITextureLoader()
    {

    }

    TextureLoaderBmp::~TextureLoaderBmp()
    {

    }

    bool TextureLoaderBmp::Load(const StringA& fileName, Yw3dDevice* device, Yw3dTexture** texture)
    {
        if ((0 == fileName.length()) || (nullptr == device) || (nullptr == texture))
        {
            return false;
        }

        // Use file io.
        FileIO file;
        uint8_t* textureData = nullptr;
        uint32_t fileSize = file.ReadFile(fileName, &textureData, false);
        if ((0 == fileSize) || (nullptr == textureData))
        {
            YW_SAFE_DELETE_ARRAY(textureData);
            return false;
        }

        // Load texture from data.
        bool res = LoadFormData(textureData, device, texture);
        if (!res)
        {
            YW_SAFE_DELETE_ARRAY(textureData);
            return false;
        }

        // Release texture data.
        YW_SAFE_DELETE_ARRAY(textureData);

        // Check loaded texture.
        if (nullptr == *texture)
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        // Generate texture mipmap.
        Yw3dResult resMip = (*texture)->GenerateMipSubLevels(0);
        if (YW3D_FAILED(resMip))
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        return true;
    }

    bool TextureLoaderBmp::LoadFormData(uint8_t* data, class Yw3dDevice* device, class Yw3dTexture** texture)
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
        uint8_t* texDataStart = (uint8_t*)(data + fileHeader->bfOffBits);
        //uint8_t* texDataStart = (uint8_t*)(data + sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader));

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
        uint8_t* rawData = texDataStart;
        for (int32_t yIdx = 0; yIdx < texHeight; yIdx++)
        {
            for (int32_t xIdx = 0; xIdx < texWidth; xIdx++)
            {
                int32_t texIndex = (texHeight - 1 - yIdx) * texWidth + xIdx;
                int32_t bmpIndex = yIdx * pitch + xIdx * bbp;
                
                if (Yw3d_FMT_R32G32B32A32F == textureFormat)
                {
                    Vector4* texData = (Vector4*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(rawData + bmpIndex);
                    texData->b = (float)((*bmpData) * colorScale);
                    texData->g = (float)((*(bmpData + 1)) * colorScale);
                    texData->r = (float)((*(bmpData + 2)) * colorScale);
                    texData->a = (float)((*(bmpData + 3)) * colorScale);
                }
                else
                {
                    Vector3* texData = (Vector3*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(rawData + bmpIndex);
                    texData->b = (float)((*bmpData) * colorScale);
                    texData->g = (float)((*(bmpData + 1)) * colorScale);
                    texData->r = (float)((*(bmpData + 2)) * colorScale);
                }
            }
        }

        // Unlock texture.
        (*texture)->UnlockRect(0);

        return true;
    }
}
