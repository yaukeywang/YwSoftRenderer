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
        static bool TextureDataToBMP(class Yw3dTexture* texture, uint8_t** resultData, uint32_t* resultDataLength);
        static bool TextureDataToYWT(class Yw3dTexture* texture, uint8_t** resultData, uint32_t* resultDataLength);

    public:
        static bool SaveTextureDataToBMPFile(class Yw3dTexture* texture, const StringA& fileName);
        static bool SaveTextureDataToYWTFile(class Yw3dTexture* texture, const StringA& fileName);
    };
}

#endif // !__YW_TEXTURE_DATA_CONVERTER_H__
