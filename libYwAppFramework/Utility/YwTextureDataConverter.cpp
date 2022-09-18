// Add by Yaukey at 2022-07-03.
// Texture data to other format converter.

#include "YwTextureDataConverter.h"
#include "Yw3d.h"
#include "bmp.h"
#include "ywt.h"
#include "YwFileIO.h"

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
        const int32_t totalSaveDataSize = totalBmpHeaderSize + textureSize;
        
        // Createa data buffer.
        *resultData = new uint8_t[totalSaveDataSize];
        *resultDataLength = totalSaveDataSize;

        // Fill header.
        uint8_t* bmpRawData = *resultData;
        BitMapFileHeader* bmpFileHeader = (BitMapFileHeader*)bmpRawData;
        bmpFileHeader->bfType = BITMAP_FILE_MAGIC;
        bmpFileHeader->bfSize = totalSaveDataSize;
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
        uint8_t* saveTextureData = (bmpRawData + totalBmpHeaderSize);

        // Lock and read texture data.
        float* textureData = nullptr;
        Yw3dResult resLock = texture->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            YW_SAFE_DELETE_ARRAY(*resultData);
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
                    uint8_t* bmpData = (uint8_t*)(saveTextureData + bmpIndex);
                    *(bmpData + 0) = (uint8_t)(texData->b * colorScale);
                    *(bmpData + 1) = (uint8_t)(texData->g * colorScale);
                    *(bmpData + 2) = (uint8_t)(texData->r * colorScale);
                    *(bmpData + 3) = (uint8_t)(texData->a * colorScale);
                }
                else
                {
                    Vector3* texData = (Vector3*)textureData + texIndex;
                    uint8_t* bmpData = (uint8_t*)(saveTextureData + bmpIndex);
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

    bool YwTextureDataConverter::TextureDataToYWT(class Yw3dTexture* texture, uint8_t** resultData, uint32_t* resultDataLength)
    {
        if ((nullptr == texture) || (nullptr == resultData) || (nullptr == resultDataLength))
        {
            return false;
        }

        YwTextureData saveTextureData;
        saveTextureData.width = texture->GetWidth();
        saveTextureData.height = texture->GetHeight();
        saveTextureData.format = texture->GetFormat();

        uint8_t textureMipLevels = (uint8_t)texture->GetMipLevels();
        for (uint8_t i = 0; i < textureMipLevels; i++)
        {
            float* textureData = nullptr;
            Yw3dResult resLock = texture->LockRect(i, (void**)&textureData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                return false;
            }

            saveTextureData.mipsData.push_back(YwTextureMipData());
            YwTextureMipData& saveTextureMipData = saveTextureData.mipsData[i];

            saveTextureMipData.mipLevel = i;
            saveTextureMipData.mipWidth = texture->GetWidth(i);
            saveTextureMipData.mipHeight = texture->GetHeight(i);

            uint32_t mipDataSize = saveTextureMipData.mipWidth * saveTextureMipData.mipHeight * texture->GetFormatFloats() * sizeof(float);
            saveTextureMipData.mipData.resize(mipDataSize);

            // Copy mip texture data.
            memcpy(&saveTextureMipData.mipData[0], textureData, mipDataSize);

            // Unlock texture.
            texture->UnlockRect(i);
        }

        // Createa data buffer.
        uint32_t totalSaveDataSize = GetYwTextureSaveDataSize(saveTextureData);
        if (0 == totalSaveDataSize)
        {
            return false;
        }

        uint8_t* saveData = new uint8_t[totalSaveDataSize];
        bool convertResult = SaveYwTextureToData(saveTextureData, saveData, totalSaveDataSize);
        if (!convertResult)
        {
            YW_SAFE_DELETE_ARRAY(saveData);
            return false;
        }

        *resultData = saveData;
        *resultDataLength = totalSaveDataSize;

        return true;
    }

    bool YwTextureDataConverter::SaveTextureDataToBMPFile(Yw3dTexture* texture, const StringA& fileName)
    {
        uint8_t* textureData = nullptr;
        uint32_t textureDataLength = 0;
        if (!TextureDataToBMP(texture, &textureData, &textureDataLength))
        {
            return false;
        }

        FileIO file;
        if (0 == file.WriteFile(fileName, textureData, textureDataLength, false))
        {
            return false;
        }

        YW_SAFE_DELETE_ARRAY(textureData);

        return true;
    }

    bool YwTextureDataConverter::SaveTextureDataToYWTFile(Yw3dTexture* texture, const StringA& fileName)
    {
        uint8_t* textureData = nullptr;
        uint32_t textureDataLength = 0;
        if (!TextureDataToYWT(texture, &textureData, &textureDataLength))
        {
            return false;
        }

        FileIO file;
        if (0 == file.WriteFile(fileName, textureData, textureDataLength, false))
        {
            return false;
        }

        YW_SAFE_DELETE_ARRAY(textureData);

        return true;
    }
}
