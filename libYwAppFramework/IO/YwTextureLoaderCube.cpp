// Add by Yaukey at 2021-03-29.
// YW texture loader for cube texture class.

#include "YwTextureLoaderCube.h"
#include "YwTextureLoaderBMP.h"
#include "YwTextureLoaderPNG.h"
#include "YwTextureLoaderTGA.h"
#include "YwTextureLoaderRGBE.h"
#include <sstream>
#include "Yw3d.h"

namespace yw
{
    TextureLoaderCube::TextureLoaderCube():
        ITextureLoader()
    {

    }

    TextureLoaderCube::~TextureLoaderCube()
    {

    }

    bool TextureLoaderCube::LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, IYw3dBaseTexture** texture)
    {
        // Use string streaam to parse data.
        std::stringstream cubeData((const char*)data);

        // Alloc a temp buffer to read line.
        char buff[256];
        memset(buff, 0, sizeof(buff));

        // Cube map face textures.
        std::vector<StringA> faceTextureNames;

        while (true)
        {
            cubeData >> buff;
            if (!cubeData)
            {
                break;
            }

            // Exit if get a empty line, mean next line is image size, time to break.
            if (0 == strlen(buff))
            {
                break;
            }

            faceTextureNames.push_back(buff);
        }

        uint32_t numTextures = (uint32_t)faceTextureNames.size();
        if (Yw3d_CF_NumCubeFaces != numTextures)
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Wrong cube face file count."));
            return false;
        }

        Yw3dTexture* faceTextures[Yw3d_CF_NumCubeFaces];
        memset(faceTextures, 0, sizeof(faceTextures));

        StringA fileDataDir = GetFileDirectory(fileName);
        uint32_t cubeEdgeLength = 0;
        Yw3dFormat cubeFormat = Yw3d_FMT_R32G32B32F;
        uint32_t cubeFormatBytes = 0;
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            StringA faceFileName = fileDataDir + faceTextureNames[i];
            if (!LoadTextureByFileName(faceFileName, device, &faceTextures[i]))
            {
                ReleaseAllLoadedTextures(faceTextures, Yw3d_CF_NumCubeFaces);
                return false;
            }

            // $Note: Texture width and height must be equal.

            if (0 == i)
            {
                cubeEdgeLength = faceTextures[i]->GetWidth();
                cubeFormat = faceTextures[i]->GetFormat();
                cubeFormatBytes = faceTextures[i]->GetFormatFloats() * sizeof(float);
            }
        }

        // Convert texture dynamic instance class.
        Yw3dCubeTexture** inputTexture = (Yw3dCubeTexture**)texture;

        // Create cube texture.
        YW_SAFE_RELEASE(*inputTexture);
        if (YW3D_FAILED(device->CreateCubeTexture(inputTexture, cubeEdgeLength, 0, cubeFormat)))
        {
            LOGE(_T("TextureLoaderCube.LoadFromData: Create cube texture failed."));
            return false;
        }

        // Fill loaded textures into cube faces.
        uint32_t copyBytes = cubeEdgeLength * cubeEdgeLength * cubeFormatBytes;
        for (int32_t i = 0; i < (int32_t)Yw3d_CF_NumCubeFaces; i++)
        {
            uint8_t* dst = nullptr;
            (*inputTexture)->LockRect((Yw3dCubeFaces)i, 0, (void**)&dst, nullptr);

            uint8_t* src = nullptr;
            faceTextures[i]->LockRect(0, (void**)src, nullptr);
            memcpy(dst, src, copyBytes);
            faceTextures[i]->UnlockRect(0);
            YW_SAFE_RELEASE(faceTextures[i]);

            (*inputTexture)->UnlockRect((Yw3dCubeFaces)i, 0);
        }

        ReleaseAllLoadedTextures(faceTextures, Yw3d_CF_NumCubeFaces);

        return true;
    }

    bool TextureLoaderCube::GenerateMipmap(IYw3dBaseTexture* texture)
    {
        return GenerateCubeTextureMipmap(texture);
    }

    StringA TextureLoaderCube::GetFileExtension(const StringA& fileName)
    {
        StringA extension;
        int32_t pos = (int32_t)fileName.find_last_of('.');
        if (pos < 0)
        {
            return extension;
        }

        extension = fileName.substr(pos + 1, fileName.length() - pos);
        return extension;
    }

    StringA TextureLoaderCube::GetFileDirectory(const StringA& fileName)
    {
        StringA result;
        int32_t pos = (int32_t)fileName.find_last_of('/');
        if (pos < 0)
        {
            return result;
        }

        result = fileName.substr(0, pos + 1);
        return result;
    }

    bool TextureLoaderCube::LoadTextureByFileName(const StringA& fileName, Yw3dDevice* device, Yw3dTexture** texture)
    {
        StringA fileExt = GetFileExtension(fileName);
        ITextureLoader* textureLoader = nullptr;
        if ("bmp" == fileExt)
        {
            textureLoader = new TextureLoaderBMP();
        }
        else if ("png" == fileExt)
        {
            textureLoader = new TextureLoaderPNG();
        }
        else if ("tga" == fileExt)
        {
            textureLoader = new TextureLoaderTGA();
        }
        else if (("hdr" == fileExt) || ("rgbe" == fileExt) || ("xyze" == fileExt))
        {
            textureLoader = new TextureLoaderRGBE();
        }
        else
        {
            LOGE(_T("TextureLoaderCube.LoadTextureByFileName: Unsupported texture format."));
            return false;
        }

        IYw3dBaseTexture** baseTexture = (IYw3dBaseTexture**)texture;
        if (!textureLoader->Load(fileName, device, baseTexture, true))
        {
            YW_SAFE_DELETE(textureLoader);
            LOGE(_T("TextureLoaderCube.LoadTextureByFileName: Load texture failed."));
            return false;
        }

        YW_SAFE_DELETE(textureLoader);
        return true;
    }

    void TextureLoaderCube::ReleaseAllLoadedTextures(Yw3dTexture** textures, int32_t length)
    {
        for (int32_t i = 0; i < length; i++)
        {
            YW_SAFE_RELEASE(textures[i]);
        }
    }
}
