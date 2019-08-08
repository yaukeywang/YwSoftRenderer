// Add by Yaukey at 2019-06-26.
// YW texture loader for png class.

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderPng : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderPng();

        // Destructor.
        ~TextureLoaderPng();

    public:
        // Load texture data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the model file.
        // @param[in] device used to create texture.
        // @param[out] texture the loaded data to fill.
        // @param[in] generateMipmap generate mipmap or not. NOTE: only textures whose size is pow of 2 are supported, or it will not generate mipmap even generateMipmap is set to true.
        // @return true if the texture loading ok, false if loading failed.
        virtual bool Load(const StringA& fileName, class Yw3dDevice* device, class Yw3dTexture** texture, bool generateMipmap = true);

    private:
        // Load png texture from data.
        bool LoadFormData(uint8_t* data, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}
