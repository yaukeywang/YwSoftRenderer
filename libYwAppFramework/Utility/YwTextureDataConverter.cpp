// Add by Yaukey at 2022-07-03.
// Texture data to other format converter.

#include <sstream>
#include "YwTextureDataConverter.h"
#include "Yw3d.h"
#include "bmp.h"
#include "ywt.h"
#include "YwFileIO.h"

namespace yw
{
    bool YwTextureDataConverter::TextureDataToBMP(Yw3dTexture* texture, std::vector<ConvertResult>& results, bool withMipmap)
    {
        if (nullptr == texture)
        {
            return false;
        }

        // Clear buffer first.
        results.clear();

        // Create buffer data by mipmap.
        const int32_t mipmapLevels = withMipmap ? texture->GetMipLevels() : 1;
        for (int32_t i = 0; i < mipmapLevels; i++)
        {
            const int32_t textureWidth = (int32_t)texture->GetWidth(i);
            const int32_t textureHeight = (int32_t)texture->GetHeight(i);
            const Yw3dFormat textureFormat = texture->GetFormat();
            const int32_t texturebbp = (int32_t)texture->GetFormatFloats();
            const int32_t texturePitch = ((textureWidth * texturebbp) + 3) / 4 * 4;
            const int32_t textureSize = textureWidth * textureHeight * texturebbp;
            const int32_t totalBmpHeaderSize = sizeof(BitMapFileHeader) + sizeof(BitMapInfoHeader);
            const int32_t totalSaveDataSize = totalBmpHeaderSize + textureSize;

            // Create a data buffer.
            results.push_back(ConvertResult(new uint8_t[totalSaveDataSize], totalSaveDataSize));

            // Fill header.
            uint8_t* bmpRawData = results[i].resultData;
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
            Yw3dResult resLock = texture->LockRect(i, (void**)&textureData, nullptr);
            if (YW3D_FAILED(resLock))
            {
                YW_SAFE_DELETE_ARRAY(results[i].resultData);
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
            texture->UnlockRect(i);
        }

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

    bool YwTextureDataConverter::SaveTextureDataToBMPFile(const StringA& fileName, Yw3dTexture* texture, bool withMipmap)
    {
        if (fileName.empty() || (nullptr == texture))
        {
            return false;
        }

        std::vector<ConvertResult> results;
        if (!TextureDataToBMP(texture, results, withMipmap))
        {
            return false;
        }

        for (int32_t i = 0; i < (int32_t)results.size(); i++)
        {
            // Get content.
            ConvertResult& convertResult = results[i];

            // Get proper file name.
            char finalFileName[256];
            sprintf(finalFileName, "%s_mip%d.bmp", fileName.c_str(), i);

            // Save data to file.
            FileIO file;
            if (0 == file.WriteFile(finalFileName, convertResult.resultData, convertResult.resultDataLength, false))
            {
                // Release texture data.
                YW_SAFE_DELETE_ARRAY(convertResult.resultData);
                convertResult.resultDataLength = 0;

                return false;
            }

            // Release texture data.
            YW_SAFE_DELETE_ARRAY(convertResult.resultData);
            convertResult.resultDataLength = 0;
        }

        return true;
    }

    bool YwTextureDataConverter::SaveTextureDataToYWTFile(const StringA& fileName, Yw3dTexture* texture)
    {
        if (fileName.empty() || (nullptr == texture))
        {
            return false;
        }

        uint8_t* textureData = nullptr;
        uint32_t textureDataLength = 0;
        if (!TextureDataToYWT(texture, &textureData, &textureDataLength))
        {
            return false;
        }

        FileIO file;
        if (0 == file.WriteFile(fileName, textureData, textureDataLength, false))
        {
            YW_SAFE_DELETE_ARRAY(textureData);
            return false;
        }

        YW_SAFE_DELETE_ARRAY(textureData);

        return true;
    }

    bool YwTextureDataConverter::SaveCubeTextureDataToBMPFile(const StringA& fileName, Yw3dCubeTexture* cubeTexture)
    {
        if (fileName.empty() || (nullptr == cubeTexture))
        {
            return false;
        }

        // Cube face names.
        const char* cubeFaceNames[Yw3d_CF_NumCubeFaces] = { "px", "nx", "py", "ny", "pz", "nz" };

        // Save all cube faces.
        std::vector<StringA> cubeFaceTextureNames;
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            Yw3dTexture* cubeFaceTexture = cubeTexture->AcquireCubeFace((Yw3dCubeFaces)i);

            std::vector<ConvertResult> results;
            if (!TextureDataToBMP(cubeFaceTexture, results, false))
            {
                return false;
            }

            if (1 != results.size())
            {
                return false;
            }

            // Get content.
            ConvertResult& convertResult = results[0];

            // Get proper file name.
            char finalFileName[256];
            sprintf(finalFileName, "%s_%s.bmp", fileName.c_str(), cubeFaceNames[i]);
            cubeFaceTextureNames.push_back(finalFileName);

            // Save data to file.
            FileIO cubeFacefile;
            if (0 == cubeFacefile.WriteFile(finalFileName, convertResult.resultData, convertResult.resultDataLength, false))
            {
                // Release texture data.
                YW_SAFE_DELETE_ARRAY(convertResult.resultData);
                convertResult.resultDataLength = 0;

                return false;
            }

            // Release texture data.
            YW_SAFE_DELETE_ARRAY(convertResult.resultData);
            convertResult.resultDataLength = 0;

            // Release acquired cube face texture.
            YW_SAFE_RELEASE(cubeFaceTexture);
        }

        // Save cube texture.
        StringA cubeTextureContent;
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            cubeTextureContent += cubeFaceTextureNames[i];

            // As long as you read/write text streams, or files in text mode, \n will be translated into the correct sequence for the platform.
            // http://en.cppreference.com/w/c/io
            cubeTextureContent += "\n";
        }

        // Save cube texture.
        FileIO cubefile;
        if (0 == cubefile.WriteFile(fileName + ".cube", (uint8_t*)cubeTextureContent.c_str(), (uint32_t)cubeTextureContent.length(), true))
        {
            return false;
        }

        return true;
    }

    bool YwTextureDataConverter::SaveCubeTextureDataToYWTFile(const StringA& fileName, Yw3dCubeTexture* cubeTexture)
    {
        if (fileName.empty() || (nullptr == cubeTexture))
        {
            return false;
        }

        // Cube face names.
        const char* cubeFaceNames[Yw3d_CF_NumCubeFaces] = { "px", "nx", "py", "ny", "pz", "nz" };

        // Save all cube faces.
        std::vector<StringA> cubeFaceTextureNames;
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            Yw3dTexture* cubeFaceTexture = cubeTexture->AcquireCubeFace((Yw3dCubeFaces)i);

            uint8_t* textureData = nullptr;
            uint32_t textureDataLength = 0;
            if (!TextureDataToYWT(cubeFaceTexture, &textureData, &textureDataLength))
            {
                return false;
            }

            // Get proper file name.
            char finalFileName[256];
            sprintf(finalFileName, "%s_%s.ywt", fileName.c_str(), cubeFaceNames[i]);
            cubeFaceTextureNames.push_back(finalFileName);

            // Save data to file.
            FileIO cubeFacefile;
            if (0 == cubeFacefile.WriteFile(finalFileName, textureData, textureDataLength, false))
            {
                YW_SAFE_DELETE_ARRAY(textureData);
                return false;
            }

            // Release texture data.
            YW_SAFE_DELETE_ARRAY(textureData);

            // Release acquired cube face texture.
            YW_SAFE_RELEASE(cubeFaceTexture);
        }

        // Save cube texture.
        StringA cubeTextureContent;
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            cubeTextureContent += cubeFaceTextureNames[i];

            // As long as you read/write text streams, or files in text mode, \n will be translated into the correct sequence for the platform.
            // http://en.cppreference.com/w/c/io
            cubeTextureContent += "\n";
        }

        // Save cube texture.
        FileIO cubefile;
        if (0 == cubefile.WriteFile(fileName + ".cube", (uint8_t*)cubeTextureContent.c_str(), (uint32_t)cubeTextureContent.length(), true))
        {
            return false;
        }

        return true;
    }
}
