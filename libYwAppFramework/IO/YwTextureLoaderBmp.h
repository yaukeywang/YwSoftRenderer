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

    private:
        // Load png texture from data.
        virtual bool LoadFormData(uint8_t* data, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}
