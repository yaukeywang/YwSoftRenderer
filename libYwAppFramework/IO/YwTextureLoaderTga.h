// Add by Yaukey at 2019-08-20.
// YW texture loader for tga class.

#ifndef __YW_TEXTURE_LOADER_TGA_H__
#define __YW_TEXTURE_LOADER_TGA_H__

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderTga : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderTga();

        // Destructor.
        ~TextureLoaderTga();

    private:
        // Load texture from kinds of data.
        // @param[in] data - texture raw data.
        // @param[in] dataLength - length in bytes of data.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        virtual bool LoadFormData(uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}

#endif // !__YW_TEXTURE_LOADER_TGA_H__
