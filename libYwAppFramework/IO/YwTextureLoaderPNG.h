// Add by Yaukey at 2019-06-26.
// YW texture loader for png class.

#ifndef __YW_TEXTURE_LOADER_PNG_H__
#define __YW_TEXTURE_LOADER_PNG_H__

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderPNG : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderPNG();

        // Destructor.
        ~TextureLoaderPNG();

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

#endif // !__YW_TEXTURE_LOADER_PNG_H__
