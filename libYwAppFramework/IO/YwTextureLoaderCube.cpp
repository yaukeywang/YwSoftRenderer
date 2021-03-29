// Add by Yaukey at 2021-03-29.
// YW texture loader for cube texture class.

#include "YwTextureLoaderCube.h"

namespace yw
{
    // Get a line from string with specified delimiter, return moved distance from src.
    // NOTE: Content in dst not include delimiter.
    static int32_t _GetLineFromString(const char* src, char* dst, int32_t dstLength, const char delimiter = '\n')
    {
        char* p = (char*)src;
        char* q = dst;
        while (p - src < dstLength - 1)
        {
            if (delimiter == *p)
            {
                p++;
                break;
            }

            *q++ = *p++;
        }

        *q = '\0';
        return (int32_t)(p - src);
    }

    YwTextureLoaderCube::YwTextureLoaderCube():
        ITextureLoader()
    {

    }

    YwTextureLoaderCube::~YwTextureLoaderCube()
    {

    }

    bool YwTextureLoaderCube::LoadFromData(const uint8_t* data, uint32_t dataLength, class Yw3dDevice* device, class Yw3dTexture** texture)
    {
        // Alloc a temp buffer to read line.
        char buff[256];
        memset(buff, 0, sizeof(buff));

        // Pointer to raw data.
        const char* dataHead = (const char*)data;

        // How many data move from the data begining.
        int32_t dataMoved = 0;

        // Cube map face textures.
        std::vector<StringA> faceTextures;

        while (true)
        {
            // Check header magic first.
            dataMoved += _GetLineFromString(dataHead + dataMoved, buff, sizeof(buff) / sizeof(buff[0]));

            // Exit if get a empty line, mean next line is image size, time to break.
            if (0 == strlen(buff))
            {
                break;
            }

            faceTextures.push_back(buff);
        }

        uint32_t numTextures = (uint32_t)faceTextures.size();
        if (6 != numTextures)
        {
            LOGE(_T("YwTextureLoaderCube.LoadFromData: Wrong Cube face file count."));
            return false;
        }

        for (int32_t i = 0; i < (int32_t)numTextures; i++)
        {

        }

        return true;
    }
}
