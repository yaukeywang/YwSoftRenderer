// Add by Yaukey at 2022-07-03.
// Texture data to other format converter.

#include "YwTextureDataConverter.h"
#include "Yw3d.h"
#include "bmp.h"

namespace yw
{
    bool YwTextureDataConverter::TextureDataToBMP(Yw3dTexture* texture, uint8_t** resultData, uint32_t* resultDataLength)
    {
        if ((nullptr == texture) || (nullptr == resultData) || (nullptr == resultDataLength))
        {
            return false;
        }

        const int32_t textureWidth = texture->GetWidth();
        const int32_t textureHeight = texture->GetHeight();
        const Yw3dFormat textureFormat = texture->GetFormat();
        const int32_t texturebbp = texture->GetFormatFloats();
        const int32_t texturePitch = ((textureWidth * texturebbp) + 3) / 4 * 4;
        const int32_t textureSize = textureWidth * textureHeight * texturebbp;
        const int32_t totalBmpHeaderSize = sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader);
        const int32_t totalBmpDataSize = totalBmpHeaderSize + textureSize;
        
        // Createa data buffer.
        *resultData = new uint8_t[totalBmpDataSize];
        *resultDataLength = totalBmpDataSize;

        // Fill header.
        uint8_t* bmpRawData = *resultData;
        BitMapFileHeader* bmpFileHeader = (BitMapFileHeader*)bmpRawData;
        bmpFileHeader->bfType = BITMAP_FILE_MAGIC;
        bmpFileHeader->bfSize = totalBmpDataSize;
        bmpFileHeader->bfReserved1 = 0;
        bmpFileHeader->bfReserved2 = 0;
        bmpFileHeader->bfOffBits = totalBmpHeaderSize;

        BitMapInfoHeader* bmpInfoHeader = (BitMapInfoHeader*)(bmpRawData + sizeof(BitMapFileHeader));
        bmpInfoHeader->biSize = sizeof(BitMapInfoHeader);
        bmpInfoHeader->biWidth = textureWidth;
        bmpInfoHeader->biHeight = textureHeight;
        bmpInfoHeader->biPlanes = 1;
        bmpInfoHeader->biBitCount = texturebbp * 8;
        bmpInfoHeader->biCompression = 0; // BI_RGB;
        bmpInfoHeader->biSizeImage = 0;
        bmpInfoHeader->biXPelsPerMeter = 0;
        bmpInfoHeader->biYPelsPerMeter = 0;
        bmpInfoHeader->biClrUsed = 0;
        bmpInfoHeader->biClrImportant = 0;

        // Get bmp texture data start.
        uint8_t* bmpTextureData = (bmpRawData + totalBmpHeaderSize);

        // Lock and read texture data.
        float* textureData = nullptr;
        Yw3dResult resLock = texture->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            return false;
        }

        // Color scale.
        const float colorScale = 255.0f;

        // Fill data.
        for (int32_t yIdx = 0; yIdx < textureHeight; yIdx++)
        {
            for (int32_t xIdx = 0; xIdx < textureWidth; xIdx++)
            {
                int32_t texIndex = (textureHeight - 1 - yIdx) * textureWidth + xIdx;
                int32_t bmpIndex = yIdx * texturePitch + xIdx * texturebbp;

                if (Yw3d_FMT_R32G32B32A32F == textureFormat)
                {
                    Vector4* texData = (Vector4*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(bmpTextureData + bmpIndex);
                    *(bmpData + 0) = (uint8_t)(texData->b * colorScale);
                    *(bmpData + 1) = (uint8_t)(texData->g * colorScale);
                    *(bmpData + 2) = (uint8_t)(texData->r * colorScale);
                    *(bmpData + 3) = (uint8_t)(texData->a * colorScale);
                }
                else
                {
                    Vector3* texData = (Vector3*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(bmpTextureData + bmpIndex);
                    *(bmpData + 0) = (uint8_t)(texData->b * colorScale);
                    *(bmpData + 1) = (uint8_t)(texData->g * colorScale);
                    *(bmpData + 2) = (uint8_t)(texData->r * colorScale);
                }
            }
        }

        // Unlock texture.
        texture->UnlockRect(0);

        return true;
    }
}
