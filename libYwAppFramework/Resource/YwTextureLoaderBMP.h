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
        // @param[in] fileName the full path of the texture file.
        // @param[in] data raw file data.
        // @param[in] dataLength length in bytes of data.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        virtual bool LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class IYw3dBaseTexture** texture);

        // Generate mipmap for texture.
        // @param[out] texture the loaded data to fill.
        virtual bool GenerateMipmap(class IYw3dBaseTexture* texture);
    };
}

#endif // !__YW_TEXTURE_LOADER_BMP_H__
