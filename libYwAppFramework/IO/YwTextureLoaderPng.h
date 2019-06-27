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
        // @param[out] texture the loaded data to fill.
        // @return device used to create texture.
        // @return texture pointer of the loaded data, null if failed.
        virtual bool Load(const StringA& fileName, class Yw3dDevice* device, class Yw3dTexture** texture);

    private:
        // Load png texture from data.
        bool LoadFormData(uint8_t* data, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}
