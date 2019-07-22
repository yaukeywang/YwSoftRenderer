// Add by Yaukey at 2019-06-26.
// YW texture loader for png class.

#include "YwTextureLoaderPng.h"
#include "Yw3d.h"
#include "YwFileIO.h"

#if defined(_WIN32) || defined(WIN32)
#include "png.h"
#elif defined(LINUX_X11) || defined(_LINUX)
#include <png.h>
#elif defined(_MAC_OSX)
#include <png.h>
#elif defined(__amigaos4__) || (_AMIGAOS4)
#include <libpng/png.h>
#endif

namespace yw
{
    // Png data read wrapper.
    struct PngDataWrapper
    {
        uint8_t* data;
        uint32_t offset;
    };

    // Callback to read png data.
    static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        PngDataWrapper* wrapper = (PngDataWrapper*)png_get_io_ptr(png_ptr);
        uint8_t* io_ptr = wrapper->data + wrapper->offset;
        memcpy(data, io_ptr, length);
        wrapper->offset += (uint32_t)length;
    }

    TextureLoaderPng::TextureLoaderPng() : 
        ITextureLoader()
    {

    }

    TextureLoaderPng::~TextureLoaderPng()
    {

    }

    bool TextureLoaderPng::Load(const StringA& fileName, Yw3dDevice* device, Yw3dTexture** texture)
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
        bool res = LoadFormData(textureData, device, texture);
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
        Yw3dResult resMip = (*texture)->GenerateMipSubLevels(0);
        if (YW3D_FAILED(resMip))
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        return true;
    }

    bool TextureLoaderPng::LoadFormData(uint8_t* data, Yw3dDevice* device, Yw3dTexture** texture)
    {
        // Create base structure.
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (nullptr == png_ptr)
        {
            return false;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (nullptr == info_ptr)
        {
            png_destroy_read_struct(&png_ptr, (png_infopp)nullptr, (png_infopp)nullptr);
            return false;
        }

        png_infop end_info = png_create_info_struct(png_ptr);
        if (nullptr == end_info)
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)nullptr);
            return false;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }

        // Set png data wrapper.
        PngDataWrapper dataWrapper;
        dataWrapper.data = data;
        dataWrapper.offset = 0;

        // Set png data read function.
        png_set_read_fn(png_ptr, (png_voidp*)(&dataWrapper), png_read_data);
        png_read_info(png_ptr, info_ptr);

        // Read texture dimension and format info.
        uint32_t dimensionX = png_get_image_width(png_ptr, info_ptr);
        uint32_t dimensionY = png_get_image_height(png_ptr, info_ptr);

        int32_t color_type = png_get_color_type(png_ptr, info_ptr);
        if ((PNG_COLOR_TYPE_PALETTE == color_type) && png_get_bit_depth(png_ptr, info_ptr) <= 8)
        {
            png_set_expand(png_ptr);
        }

        if ((PNG_COLOR_TYPE_GRAY == color_type) && png_get_bit_depth(png_ptr, info_ptr) < 8)
        {
            png_set_expand(png_ptr);
        }

        if ((PNG_COLOR_TYPE_GRAY == color_type) || (PNG_COLOR_TYPE_GRAY_ALPHA == color_type))
        {
            png_set_gray_to_rgb(png_ptr);
        }

        if (16 == png_get_bit_depth(png_ptr, info_ptr))
        {
            png_set_strip_16(png_ptr);
        }

        png_read_update_info(png_ptr, info_ptr);

        color_type = png_get_color_type(png_ptr, info_ptr);
        bool hasAlpha = (PNG_COLOR_TYPE_RGB_ALPHA == color_type);
        Yw3dFormat textureFormat = hasAlpha ? Yw3d_FMT_R32G32B32A32F : Yw3d_FMT_R32G32B32F;

        // Create texture from device.
        YW_SAFE_RELEASE(*texture);
        if (YW3D_FAILED(device->CreateTexture(texture, dimensionX, dimensionY, 0, textureFormat)))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }

        // Get raw data.
        uint8_t* rawData = new uint8_t[dimensionY * png_get_rowbytes(png_ptr, info_ptr)];
        uint8_t* curData = rawData;
        uint8_t** rowsData = new uint8_t * [dimensionY];
        for (uint32_t i = 0; i < dimensionY; i++)
        {
            rowsData[i] = curData;
            curData += png_get_rowbytes(png_ptr, info_ptr);
        }

        // Read the image.
        png_read_image(png_ptr, rowsData);

        // Release row data.
        YW_SAFE_DELETE_ARRAY(rowsData);

        png_read_end(png_ptr, end_info);
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

        // Lock texture data.
        float* textureData = nullptr;
        Yw3dResult resLock = (*texture)->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            YW_SAFE_DELETE_ARRAY(rawData);
            return false;
        }

        // Normalized color scale.
        const float colorScale = 1.0f / 255.0f;

        // Fill data.
        curData = rawData;
        for (uint32_t yIdx = 0; yIdx < dimensionY; yIdx++)
        {
            for (uint32_t xIdx = 0; xIdx < dimensionX; xIdx++)
            {
                (*textureData++) = (float)((*curData++) * colorScale);
                (*textureData++) = (float)((*curData++) * colorScale);
                (*textureData++) = (float)((*curData++) * colorScale);
                if (hasAlpha)
                {
                    (*textureData++) = (float)((*curData++) * colorScale);
                }
            }
        }

        YW_SAFE_DELETE_ARRAY(rawData);
        (*texture)->UnlockRect(0);

        return true;
    }
}
