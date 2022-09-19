// Add by Yaukey at 2022-09-18.
// YWTexture raw texture format lib.

#include "ywt.h"

bool LoadYwTextureFromData(const uint8_t* data, const uint32_t dataLength, YwTextureData& result)
{
    if (nullptr == data)
    {
        return false;
    }

    // Read bit map file header.
    const YwTextureFileHeader* fileHeader = (const YwTextureFileHeader*)data;
    if (YWT_FILE_MAGIC != fileHeader->fileType)
    {
        // Compare file header magic.
        return false;
    }

    result.width = fileHeader->width;
    result.height = fileHeader->height;
    result.format = fileHeader->format;

    const uint8_t* dataBegin = data + sizeof(YwTextureFileHeader);
    for (uint8_t i = 0; i < fileHeader->mipsCount; i++)
    {
        const YwTextureFileMipmapHeader* mipHeader = (const YwTextureFileMipmapHeader*)dataBegin;
        dataBegin += sizeof(YwTextureFileMipmapHeader);
        if (0 == mipHeader->mipDataSize)
        {
            break;
        }

        YwTextureMipData mipData;
        mipData.mipLevel = mipHeader->mipLevel;
        mipData.mipWidth = mipHeader->mipWidth;
        mipData.mipHeight = mipHeader->mipHeight;
        mipData.mipData.resize(mipHeader->mipDataSize);
        memcpy(&mipData.mipData[0], dataBegin, mipHeader->mipDataSize);
        result.mipsData.push_back(mipData);
    }

    if (fileHeader->mipsCount != result.mipsData.size())
    {
        return false;
    }

    return true;
}

bool SaveYwTextureToData(const YwTextureData& textureData, uint8_t* data, uint32_t dataSize)
{
    if ((nullptr == data) || (0 == dataSize))
    {
        return false;
    }

    const uint32_t saveDataSize = GetYwTextureSaveDataSize(textureData);
    if (saveDataSize != dataSize)
    {
        return false;
    }

    // Start to save data.
    uint8_t* dataPointer = data;
    
    // Save texture header.
    YwTextureFileHeader* header = (YwTextureFileHeader*)dataPointer;
    dataPointer += sizeof(YwTextureFileHeader);

    header->fileType = YWT_FILE_MAGIC;
    header->width = textureData.width;
    header->height = textureData.height;
    header->format = textureData.format;
    header->mipsCount = (uint8_t)textureData.mipsData.size();

    // Save all mips data.
    for (uint8_t i = 0; i < (uint8_t)textureData.mipsData.size(); i++)
    {
        // Save mip header.
        YwTextureFileMipmapHeader* mipHeader = (YwTextureFileMipmapHeader*)dataPointer;
        dataPointer += sizeof(YwTextureFileMipmapHeader);

        const YwTextureMipData& textureMipData = textureData.mipsData[i];
        mipHeader->mipLevel = textureMipData.mipLevel;
        mipHeader->mipWidth = textureMipData.mipWidth;
        mipHeader->mipHeight = textureMipData.mipHeight;
        mipHeader->mipDataSize = (uint32_t)textureMipData.mipData.size();

        // Save mip data content.
        memcpy(dataPointer, &textureMipData.mipData[0], mipHeader->mipDataSize);
        dataPointer += mipHeader->mipDataSize;
    }

    // Save an extra empty mip data header.
    YwTextureFileMipmapHeader* extraMipHeader = (YwTextureFileMipmapHeader*)dataPointer;
    dataPointer += sizeof(YwTextureFileMipmapHeader);

    extraMipHeader->mipLevel = 0;
    extraMipHeader->mipWidth = 0;
    extraMipHeader->mipHeight = 0;
    extraMipHeader->mipDataSize = 0;

    if ((dataPointer - data) != saveDataSize)
    {
        return false;
    }

    return true;
}

uint32_t GetYwTextureSaveDataSize(const YwTextureData& textureData)
{
    uint32_t dataSize = sizeof(YwTextureFileHeader);
    for (uint8_t i = 0; i < (uint8_t)textureData.mipsData.size(); i++)
    {
        dataSize += sizeof(YwTextureFileMipmapHeader);
        dataSize += (uint32_t)textureData.mipsData[i].mipData.size();
    }

    dataSize += sizeof(YwTextureFileMipmapHeader);
    return dataSize;
}
