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
    };
}

#endif // !__YW_TEXTURE_DATA_CONVERTER_H__
