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

    private:
        // Load png texture from data.
        virtual bool LoadFormData(uint8_t* data, class Yw3dDevice* device, class Yw3dTexture** texture);
    };
}
