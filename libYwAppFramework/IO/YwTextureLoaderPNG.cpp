// Add by Yaukey at 2019-06-26.
// YW texture loader for png class.

#include "YwTextureLoaderPNG.h"
#include "Yw3d.h"

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
    // ------------------------------------------------------------------
    // For png raw info.

    // Png data read wrapper.
    struct PngDataWrapper
    {
        const uint8_t* data;
        uint32_t offset;
    };

    // Callback to read png data.
    static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
    {
        PngDataWrapper* wrapper = (PngDataWrapper*)png_get_io_ptr(png_ptr);
        const uint8_t* io_ptr = wrapper->data + wrapper->offset;
        memcpy(data, io_ptr, length);
        wrapper->offset += (uint32_t)length;
    }

    // ------------------------------------------------------------------
    // For texture loader.

    TextureLoaderPNG::TextureLoaderPNG() : 
        ITextureLoader()
    {

    }

    TextureLoaderPNG::~TextureLoaderPNG()
    {

    }

    bool TextureLoaderPNG::LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, IYw3dBaseTexture** texture)
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

        // Convert texture dynamic instance class.
        Yw3dTexture* inputTexture = dynamic_cast<Yw3dTexture*>(*texture);

        // Create texture from device.
        YW_SAFE_RELEASE(inputTexture);
        if (YW3D_FAILED(device->CreateTexture(&inputTexture, dimensionX, dimensionY, 0, textureFormat)))
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return false;
        }

        // Get raw data.
        uint8_t* texDataRaw = new uint8_t[dimensionY * png_get_rowbytes(png_ptr, info_ptr)];
        uint8_t* curData = texDataRaw;
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
        Yw3dResult resLock = inputTexture->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            YW_SAFE_DELETE_ARRAY(texDataRaw);
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        // Normalized color scale.
        const float colorScale = 1.0f / 255.0f;

        // Fill data.
        uint8_t* srcTextureData = texDataRaw;
        for (uint32_t yIdx = 0; yIdx < dimensionY; yIdx++)
        {
            for (uint32_t xIdx = 0; xIdx < dimensionX; xIdx++)
            {
                (*textureData++) = (float)((*srcTextureData++) * colorScale);
                (*textureData++) = (float)((*srcTextureData++) * colorScale);
                (*textureData++) = (float)((*srcTextureData++) * colorScale);
                if (hasAlpha)
                {
                    (*textureData++) = (float)((*srcTextureData++) * colorScale);
                }
            }
        }
        
        inputTexture->UnlockRect(0);

        // Release raw image data.
        YW_SAFE_DELETE_ARRAY(texDataRaw);

        return true;
    }

    bool TextureLoaderPNG::GenerateMipmap(IYw3dBaseTexture* texture)
    {
        return GenerateTextureMipmap(texture);
    }
}
