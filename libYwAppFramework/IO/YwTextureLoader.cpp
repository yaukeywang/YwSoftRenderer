// Add by Yaukey at 2019-08-19.
// YW texture loader base class.

#include "YwTextureLoader.h"
#include "Yw3d.h"
#include "YwFileIO.h"

namespace yw
{
    bool ITextureLoader::Load(const StringA& fileName, Yw3dDevice* device, Yw3dTexture** texture, bool generateMipmap)
    {
        if ((0 == fileName.length()) || (nullptr == device) || (nullptr == texture))
        {
            return false;
        }

        // Use file io.
        FileIO file;
        uint8_t* textureData = nullptr;
        uint32_t fileSize = file.ReadFile(fileName, &textureData, false);
        if ((0 == fileSize) || (nullptr == textureData))
        {
            YW_SAFE_DELETE_ARRAY(textureData);
            return false;
        }

        // Load texture from data.
        bool res = LoadFromData(fileName, textureData, fileSize, device, texture);
        if (!res)
        {
            YW_SAFE_DELETE_ARRAY(textureData);
            return false;
        }

        // Release texture data.
        YW_SAFE_DELETE_ARRAY(textureData);

        // Check loaded texture.
        if (nullptr == *texture)
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        if (generateMipmap && DetermineIfPowerOf2((*texture)->GetWidth()) && DetermineIfPowerOf2((*texture)->GetHeight()))
        {
            // Generate texture mipmap.
            Yw3dResult resMip = (*texture)->GenerateMipSubLevels(0);
            if (YW3D_FAILED(resMip))
            {
                YW_SAFE_RELEASE(*texture);
                return false;
            }
        }

        return true;
    }
}
