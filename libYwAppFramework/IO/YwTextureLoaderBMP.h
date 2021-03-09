// Add by Yaukey at 2019-08-04.
// YW texture loader for bmp class.

#ifndef __YW_TEXTURE_LOADER_BMP_H__
#define __YW_TEXTURE_LOADER_BMP_H__

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderBMP : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderBMP();

        // Destructor.
        ~TextureLoaderBMP();

    private:
        // Load texture from kinds of data.
        // @param[in] data - texture raw data.
        // @param[in] dataLength - length in bytes of data.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        virtual bool LoadFromData(uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}

#endif // !__YW_TEXTURE_LOADER_BMP_H__