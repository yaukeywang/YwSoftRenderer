// Add by Yaukey at 2022-07-03.
// Texture data to other format converter.

#ifndef __YW_TEXTURE_DATA_CONVERTER_H__
#define __YW_TEXTURE_DATA_CONVERTER_H__

#include "YwBase.h"

namespace yw
{
    class YwTextureDataConverter
    {
    public:
        struct ConvertResult
        {
            uint8_t* resultData;
            uint32_t resultDataLength;

            ConvertResult(): resultData(nullptr), resultDataLength(0) {}
            ConvertResult(uint8_t* data, uint32_t length): resultData(data), resultDataLength(length) {}
        };

    public:
        static bool TextureDataToBMP(class Yw3dTexture* texture, std::vector<ConvertResult>& results, bool withMipmap);
        static bool TextureDataToYWT(class Yw3dTexture* texture, uint8_t** resultData, uint32_t* resultDataLength);

    public:
        static bool SaveTextureDataToBMPFile(const StringA& fileName, class Yw3dTexture* texture, bool withMipmap);
        static bool SaveTextureDataToYWTFile(const StringA& fileName, class Yw3dTexture* texture);

        static bool SaveCubeTextureDataToBMPFile(const StringA& fileName, class Yw3dCubeTexture* cubeTexture);
        static bool SaveCubeTextureDataToYWTFile(const StringA& fileName, class Yw3dCubeTexture* cubeTexture);
    };
}

#endif // !__YW_TEXTURE_DATA_CONVERTER_H__
