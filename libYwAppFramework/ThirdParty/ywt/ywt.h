// Add by Yaukey at 2022-09-18.
// YWTexture raw texture format lib.
// This is raw texture data format, supporting mipmap data.
// Road map:
// 1.Maybe RLE compression will be supported;
// 2.Cube map will be supported;
// 3.Animated texture will be supported.

#ifndef __YW_T_H__
#define __YW_T_H__

#include <stdint.h>
#include <vector>

// ------------------------------------------------------------------
// Header info define.

// File magic number.
const uint32_t YWT_FILE_MAGIC = ('Y' << 24) | ('W' << 16) | ('T' << 8) | 'D';

// YWTexture file header.
#pragma pack(push, 1)
struct YwTextureFileHeader
{
    uint32_t fileType; // File type, magic number.
    uint32_t width; // The width of the texture.
    uint32_t height; // The hegiht of the texture.
    int8_t format; // The pixel format of the texture.
    uint8_t mipsCount; // How many mips count of this texture.
};
#pragma pack(pop)

// YWTexture mipmap file header.
#pragma pack(push, 1)
struct YwTextureFileMipmapHeader
{
    uint8_t mipLevel; // Current mip level.
    uint32_t mipWidth; // The width of this mip level.
    uint32_t mipHeight; // The height of this mip level.
    uint32_t mipDataSize; // The size, in bytes, of the mipmap data.
};
#pragma pack(pop)

// Mipmap data read.
struct YwTextureMipData
{
    uint32_t mipLevel;
    uint32_t mipWidth;
    uint32_t mipHeight;
    std::vector<uint8_t> mipData;

    YwTextureMipData(): mipLevel(0), mipWidth(0), mipHeight(0) {}
};

// All texture data read.
struct YwTextureData
{
    uint32_t width;
    uint32_t height;
    int8_t format;
    std::vector<YwTextureMipData> mipsData;

    YwTextureData(): width(0), height(0), format(0) {}
};

bool LoadYwTextureFromData(const uint8_t* data, const uint32_t dataLength, YwTextureData& result);
bool SaveYwTextureToData(const YwTextureData& textureData, uint8_t* data, uint32_t dataSize);
uint32_t GetYwTextureSaveDataSize(const YwTextureData& textureData);

#endif // !__YW_T_H__
