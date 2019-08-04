// Add by Yaukey at 2019-08-04.
// YW texture loader for bmp class.

#include "YwTextureLoader.h"

namespace yw
{
    class TextureLoaderBmp : public ITextureLoader
    {
    public:
        // Constructor.
        TextureLoaderBmp();

        // Destructor.
        ~TextureLoaderBmp();

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
