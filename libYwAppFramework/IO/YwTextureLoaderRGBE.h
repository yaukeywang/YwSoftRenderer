// Add by Yaukey at 2021-03-09.
// YW texture loader for rgbe class.

#ifndef __YW_TEXTURE_LOADER_REBE_H__
#define __YW_TEXTURE_LOADER_REBE_H__

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderRGBE : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderRGBE();

        // Destructor.
        ~TextureLoaderRGBE();

    private:
        // Load texture from kinds of data.
        // @param[in] data - texture raw data.
        // @param[in] dataLength - length in bytes of data.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        virtual bool LoadFromData(const uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}

#endif // !__YW_TEXTURE_LOADER_REBE_H__
