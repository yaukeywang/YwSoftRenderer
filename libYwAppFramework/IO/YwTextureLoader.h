// Add by Yaukey at 2019-06-25.
// YW texture loader base class.

#ifndef __YW_TEXTURE_LOADER_H__
#define __YW_TEXTURE_LOADER_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // Base texture loader class.
    class ITextureLoader
    {
    public:
        // Constructor.
        ITextureLoader() {}

        // Destructor.
        virtual ~ITextureLoader() {}

    public:
        // Load texture data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        // @param[in] generateMipmap generate mipmap or not. NOTE: only textures whose size is pow of 2 are supported, or it will not generate mipmap even generateMipmap is set to true.
        // @return true if the texture loading ok, false if loading failed.
        bool Load(const StringA& fileName, class Yw3dDevice* device, class Yw3dTexture** texture, bool generateMipmap = true);

    private:
        // Load texture from kinds of data.
        // @param[in] data - texture raw data.
        // @param[in] dataLength - length in bytes of data.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        virtual bool LoadFromData(uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class Yw3dTexture** texture) = 0;
    };
}

#endif //!__YW_TEXTURE_LOADER_H__
