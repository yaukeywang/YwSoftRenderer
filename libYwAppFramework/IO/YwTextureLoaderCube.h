// Add by Yaukey at 2021-03-29.
// YW texture loader for cube texture class.

#ifndef __YW_TEXTURE_LOADER_CUBE_H__
#define __YW_TEXTURE_LOADER_CUBE_H__

#include "YwTextureLoader.h"

namespace yw
{
    class YwTextureLoaderCube : public ITextureLoader
    {
    public:
        // Constructor.
        YwTextureLoaderCube();

        // Destructor.
        ~YwTextureLoaderCube();

    private:
        // Load texture from kinds of data.
        // @param[in] fileName the full path of the texture file.
        // @param[in] data raw file data.
        // @param[in] dataLength length in bytes of data.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        virtual bool LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}

#endif // !__YW_TEXTURE_LOADER_CUBE_H__
