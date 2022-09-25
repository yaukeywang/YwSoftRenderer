// Add by Yaukey at 2019-08-19.
// YW texture loader base class.

#include "YwTextureLoader.h"
#include "Yw3dCubeTexture.h"
#include "Yw3d.h"
#include "YwFileIO.h"

namespace yw
{
    bool ITextureLoader::Load(const StringA& fileName, Yw3dDevice* device, IYw3dBaseTexture** texture, bool generateMipmap)
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

        // Generate texture mipmap.
        if (generateMipmap && !GenerateMipmap(*texture))
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        return true;
    }

    bool ITextureLoader::GenerateTextureMipmap(IYw3dBaseTexture* texture)
    {
        Yw3dTexture* inputTexture = dynamic_cast<Yw3dTexture*>(texture);
        if (nullptr == inputTexture)
        {
            return false;
        }

        // Generate texture mipmap.
        Yw3dResult resMip = inputTexture->GenerateMipSubLevels(0);
        if (YW3D_SUCCESSFUL(resMip))
        {
            return true;
        }
        
        return false;
    }

    bool ITextureLoader::GenerateCubeTextureMipmap(IYw3dBaseTexture* texture)
    {
        // $Note: Maybe problem with "GetWidth" and "GetHeight" when generating mipmap.

        // Generate texture mipmap.
        Yw3dCubeTexture* inputTexture = dynamic_cast<Yw3dCubeTexture*>(texture);
        if (nullptr == inputTexture)
        {
            return false;
        }

        Yw3dResult resMip = inputTexture->GenerateMipSubLevels(0);
        if (YW3D_SUCCESSFUL(resMip))
        {
            return true;
        }

        return false;
    }
}
