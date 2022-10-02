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
        struct MipConvertResult
        {
            uint8_t* resultData;
            uint32_t resultDataLength;

            MipConvertResult(): resultData(nullptr), resultDataLength(0){}
            MipConvertResult(uint8_t* data, uint32_t length): resultData(data), resultDataLength(length) {}

            void Release()
            {
                YW_SAFE_DELETE_ARRAY(resultData);
                resultDataLength = 0;
            }
        };

        struct TextureConvertResult
        {
            std::vector<MipConvertResult> allResults;

            void Release()
            {
                for (int32_t i = 0; i < (int32_t)allResults.size(); i++)
                {
                    allResults[i].Release();
                }

                allResults.clear();
            }
        };

    public:
        static bool TextureDataToBMP(class Yw3dTexture* texture, TextureConvertResult& results, bool withMipmap);
        static bool TextureDataToRGBE(class Yw3dTexture* texture, TextureConvertResult& results, bool withMipmap);
        static bool TextureDataToYWT(class Yw3dTexture* texture, uint8_t** resultData, uint32_t* resultDataLength);

    public:
        static bool SaveTextureDataToBMPFile(const StringA& fileName, class Yw3dTexture* texture, bool withMipmap);
        static bool SaveTextureDataToRGBEFile(const StringA& fileName, class Yw3dTexture* texture, bool withMipmap);
        static bool SaveTextureDataToYWTFile(const StringA& fileName, class Yw3dTexture* texture);

        static bool SaveCubeTextureDataToBMPFile(const StringA& fileName, class Yw3dCubeTexture* cubeTexture);
        static bool SaveCubeTextureDataToRGBEFile(const StringA& fileName, class Yw3dCubeTexture* cubeTexture);
        static bool SaveCubeTextureDataToYWTFile(const StringA& fileName, class Yw3dCubeTexture* cubeTexture);
    };
}

#endif // !__YW_TEXTURE_DATA_CONVERTER_H__
