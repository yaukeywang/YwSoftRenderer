// Add by Yaukey at 2021-03-29.
// YW texture loader for cube texture class.

#ifndef __YW_TEXTURE_LOADER_CUBE_H__
#define __YW_TEXTURE_LOADER_CUBE_H__

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderCube : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderCube();

        // Destructor.
        ~TextureLoaderCube();

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

    private:
        // Get file extension by file name or path.
        StringA GetFileExtension(const StringA& fileName);

        // Get directory for file path.
        StringA GetFileDirectory(const StringA& fileName);

        // Get texture by file name, auto select loader.
        bool LoadTextureByFileName(const StringA& fileName, class Yw3dDevice* device, class Yw3dTexture** texture);

        // Release all loaded textures.
        void ReleaseAllLoadedTextures(class Yw3dTexture** textures, int32_t length);
    };
}

#endif // !__YW_TEXTURE_LOADER_CUBE_H__
