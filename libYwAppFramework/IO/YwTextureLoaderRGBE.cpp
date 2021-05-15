// Add by Yaukey at 2021-03-09.
// YW texture loader for rgbe class.

#include "YwTextureLoaderRGBE.h"
#include "Yw3d.h"

namespace yw
{
    // ------------------------------------------------------------------
    // RGBE file parsing code from: https://www.graphics.cornell.edu/~bjw/rgbe.html.
    // Changing a little bit avoid using "FILE".

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

    // Convert srgb color to linear.
    static float _srgb2linear(float value) {
        return (float)pow(value, 2.2f);
    }

    // Convert linear color to srgb.
    static float _linear2srgb(float value) {
        return (float)pow(value, 1.0f / 2.2f);
    }

    // RGBE header file magic number.
    #define RGBE_HEADER_MAGIC "#?RADIANCE"

    // RGBE format magic of rgbe.
    #define RGBE_FORMAT_RGBE_MAGIC "FORMAT=32-bit_rle_rgbe"

    // RGBE format magic of xyze.
    #define RGBE_FORMAT_XYZE_MAGIC "FORMAT=32-bit_rle_xyze"

    // Offsets to red, green, and blue components in a data (float) pixel.
    #define RGBE_DATA_RED 0
    #define RGBE_DATA_GREEN 1
    #define RGBE_DATA_BLUE 2

    /* RGBE data number of floats per pixel */
    #define RGBE_DATA_SIZE 3

    // RGBE file format.
    enum RGBEFormat
    {
        RGBEFormat_RGBE,
        RGBEFormat_XYZE
    };

    // RGBE header info.
    struct RGBEHeader{
        int32_t format;
        float exposure; /* a value of 1.0 in an image corresponds to <exposure> watts/steradian/m^2. defaults to 1.0 */
        float gamma; /* image has already been gamma corrected with given gamma.  defaults to 1.0 (no correction) */
        float primaries[8];
        int32_t width;
        int32_t height;
        
        RGBEHeader(): format(RGBEFormat_RGBE), exposure(0.0f), gamma(0.0f), width(0), height(0) {}
    };

    /* Standard conversion from rgbe to float pixels. */
    /* NOTE: Ward uses ldexp(col+0.5,exp-(128+8)).  However we wanted pixels */
    /*       in the range [0,1] to map back into the range [0,1].            */
    static void _rgbe2float(float* red, float* green, float* blue, uint8_t rgbe[4])
    {
        if (0 != rgbe[3]) {   /*nonzero pixel*/
            float f = ldexp(1.0f, rgbe[3] - (int32_t)(128 + 8));
            *red = rgbe[0] * f;
            *green = rgbe[1] * f;
            *blue = rgbe[2] * f;
        }
        else
        {
            *red = *green = *blue = 0.0;
        }
    }

    // Read rgbe file header, return data moved length from original data.
    static int32_t _RGBE_ReadHeader(const uint8_t* data, RGBEHeader* header)
    {
        // Alloc a temp buffer to read line.
        char buff[256];
        memset(buff, 0, sizeof(buff));

        // Pointer to raw data.
        const char* dataHead = (const char*)data;

        // How many data move from the data begining.
        int32_t dataMoved = 0;

        // Check header magic first.
        dataMoved += _GetLineFromString(dataHead + dataMoved, buff, sizeof(buff) / sizeof(buff[0]));
        if (0 != strcmp(buff, RGBE_HEADER_MAGIC))
        {
            LOGE(_T("YwTextureLoaderRGBE._RGBE_ReadHeader: Wrong RGBE file format."));
            return -1;
        }

        // Parsing other part.
        float exposure = 0.0f;
        float gamma = 0.0f;
        float primaries[8];
        while (true)
        {
            // Get line content first.
            dataMoved += _GetLineFromString(dataHead + dataMoved, buff, sizeof(buff) / sizeof(buff[0]));

            // Exit if get a empty line, mean next line is image size, time to break.
            if (0 == strlen(buff))
            {
                break;
            }
            
            // Check parameters.
            if ('#' == buff[0])
            {
                // Comments need skip.
                continue;
            }
            else if (0 == strcmp(RGBE_FORMAT_RGBE_MAGIC, buff))
            {
                // RGBE format.
                header->format = RGBEFormat_RGBE;
            }
            else if (0 == strcmp(RGBE_FORMAT_XYZE_MAGIC, buff))
            {
                // XYZE format.
                header->format = RGBEFormat_XYZE;
            }
            else if (sscanf(buff, "EXPOSURE=%g", &exposure) == 1)
            {
                header->exposure = exposure;
            }
            else if (sscanf(buff, "GAMMA=%g", &gamma) == 1)
            {
                header->gamma = gamma;
            }
            else if (sscanf(buff, "PRIMARIES=%g %g %g %g %g %g %g %g", &primaries[0], &primaries[1], &primaries[2], &primaries[3], &primaries[4], &primaries[5], &primaries[6], &primaries[7]) == 1)
            {
                memcpy(header->primaries, primaries, sizeof(primaries) * sizeof(primaries[0]));
            }
        }

        // Read line to get width and height.
        dataMoved += _GetLineFromString(dataHead + dataMoved, buff, sizeof(buff) / sizeof(buff[0]));

        // Maybe more format later: +Y -X.
        int32_t width = 0;
        int32_t height = 0;
        if (sscanf(buff, "-Y %d +X %d", &height, &width) != 2)
        {
            LOGE(_T("YwTextureLoaderRGBE._ReadRGBEHeader: Missing image size specifier."));
            return -1;
        }

        header->width = width;
        header->height = height;

        return dataMoved;
    }

    // Simple read routine. will not correctly handle run length encoding.
    static bool _RGBE_ReadPixels(const uint8_t* srcData, float* dstData, int pixels)
    {
        uint8_t* srcDataHead = (uint8_t*)srcData;
        uint8_t rgbe[4];
        while (pixels-- > 0) {
            // Get rgbe origin data.
            memcpy(rgbe, srcDataHead, sizeof(rgbe));
            srcDataHead += (sizeof(rgbe) / sizeof(rgbe[0]));

            // Convert rgbe to float.
            _rgbe2float(&dstData[RGBE_DATA_RED], &dstData[RGBE_DATA_GREEN], &dstData[RGBE_DATA_BLUE], rgbe);
            dstData += RGBE_DATA_SIZE;
        }

        return true;
    }

    // Reading pixels. Will correctly handle run length encoding.
    static bool _RGBE_ReadPixels_RLE(const uint8_t* srcData, float* dstData, int32_t scanline_width, int32_t num_scanlines)
    {
        uint8_t* srcDataHead = (uint8_t*)srcData;
        
        if ((scanline_width < 8) || (scanline_width > 0x7fff))
        {
            /* run length encoding is not allowed so read flat*/
            return _RGBE_ReadPixels(srcData, dstData, scanline_width * num_scanlines);
        }
        
        // Scanline buffer.
        uint8_t* scanline_buffer = nullptr;

        /* read in each successive scanline */
        while (num_scanlines > 0)
        {
            // Get rgbe origin data.
            uint8_t rgbe[4];
            memcpy(rgbe, srcDataHead, sizeof(rgbe));
            srcDataHead += (sizeof(rgbe) / sizeof(rgbe[0]));

            if ((rgbe[0] != 2) || (rgbe[1] != 2) || (rgbe[2] & 0x80))
            {
                /* this file is not run length encoded */
                _rgbe2float(&dstData[RGBE_DATA_RED], &dstData[RGBE_DATA_GREEN], &dstData[RGBE_DATA_BLUE], rgbe);
                dstData += RGBE_DATA_SIZE;
                YW_SAFE_DELETE_ARRAY(scanline_buffer);
                return _RGBE_ReadPixels(srcDataHead, dstData, scanline_width * num_scanlines - 1);
            }

            if ((((int)rgbe[2]) << 8 | rgbe[3]) != scanline_width)
            {
                YW_SAFE_DELETE_ARRAY(scanline_buffer);
                LOGE(_T("wrong scanline width"));
                return false;
            }

            if (nullptr == scanline_buffer)
            {
                scanline_buffer = new uint8_t[sizeof(uint8_t) * 4 * scanline_width];
            }

            if (nullptr == scanline_buffer)
            {
                LOGE(_T("unable to allocate buffer space"));
                return false;
            }

            uint8_t* ptr = &scanline_buffer[0];

            /* read each of the four channels for the scanline into the buffer */
            for (int32_t i = 0; i < 4; i++)
            {
                uint8_t* ptr_end = &scanline_buffer[(i + 1)*scanline_width];
                while (ptr < ptr_end)
                {
                    uint8_t buf[2];
                    memcpy(buf, srcDataHead, sizeof(buf));
                    srcDataHead += (sizeof(buf) / sizeof(buf[0]));

                    if (buf[0] > 128) 
                    {
                        /* a run of the same value */
                        int32_t count = buf[0] - 128;
                        if ((count == 0) || (count > ptr_end - ptr)) {
                            YW_SAFE_DELETE_ARRAY(scanline_buffer);
                            LOGE(_T("bad scanline data"));
                            return false;
                        }

                        while (count-- > 0)
                        {
                            *ptr++ = buf[1];
                        }
                    }
                    else 
                    {
                        /* a non-run */
                        int32_t count = buf[0];
                        if ((0 == count) || (count > ptr_end - ptr))
                        {
                            YW_SAFE_DELETE_ARRAY(scanline_buffer);
                            LOGE(_T("bad scanline data"));
                            return false;
                        }

                        *ptr++ = buf[1];
                        if (--count > 0)
                        {
                            memcpy(ptr, srcDataHead, sizeof(*ptr) * count);
                            srcDataHead += count;

                            ptr += count;
                        }
                    }
                }
            }

            /* now convert data from buffer into floats */
            for (int32_t i = 0; i < scanline_width; i++)
            {
                rgbe[0] = scanline_buffer[i];
                rgbe[1] = scanline_buffer[i + scanline_width];
                rgbe[2] = scanline_buffer[i + 2 * scanline_width];
                rgbe[3] = scanline_buffer[i + 3 * scanline_width];
                _rgbe2float(&dstData[RGBE_DATA_RED], &dstData[RGBE_DATA_GREEN], &dstData[RGBE_DATA_BLUE], rgbe);
                dstData += RGBE_DATA_SIZE;
            }

            num_scanlines--;
        }

        YW_SAFE_DELETE_ARRAY(scanline_buffer);
        return true;
    }

    // ------------------------------------------------------------------
    // For texture loader.

    TextureLoaderRGBE::TextureLoaderRGBE() :
        ITextureLoader()
    {

    }

    TextureLoaderRGBE::~TextureLoaderRGBE()
    {

    }

    bool TextureLoaderRGBE::LoadFromData(const StringA& fileName, const uint8_t* data, uint32_t dataLength, Yw3dDevice* device, Yw3dTexture** texture)
    {
        // Get file header first.
        RGBEHeader header;
        int32_t headerSize = _RGBE_ReadHeader(data, &header);
        if (headerSize < 0)
        {
            LOGE(_T("TextureLoaderRGBE¡£LoadFromData: Wrong RGBE file header."));
            return false;
        }

        // Read texture data.
        uint8_t* texDataRLE = (uint8_t*)(data + headerSize);
        int32_t texWidth = header.width;
        int32_t texHeight = header.height;
        int32_t bbp = 3;
        int32_t pitch = bbp * texWidth;

        float* texDataRaw = new float[texWidth * texHeight * bbp];
        if (!_RGBE_ReadPixels_RLE(texDataRLE, texDataRaw, texWidth, texHeight))
        {
            return false;
        }

        // Create texture from device.
        YW_SAFE_RELEASE(*texture);
        if (YW3D_FAILED(device->CreateTexture(texture, texWidth, texHeight, 0, Yw3d_FMT_R32G32B32F)))
        {
            return false;
        }

        // Lock texture data.
        float* textureData = nullptr;
        Yw3dResult resLock = (*texture)->LockRect(0, (void**)&textureData, nullptr);
        if (YW3D_FAILED(resLock))
        {
            YW_SAFE_RELEASE(*texture);
            return false;
        }

        // Normalized color scale.
        const float colorScale = 1.0f; // 1.0f / 255.0f;

        // Fill data.
        float* srcTextureData = texDataRaw;
        for (int32_t yIdx = 0; yIdx < texHeight; yIdx++)
        {
            for (int32_t xIdx = 0; xIdx < texWidth; xIdx++)
            {
                int32_t texIndex = yIdx * texWidth + xIdx;
                int32_t hdrIndex = yIdx * pitch + xIdx * bbp;

                Vector3* texData = (Vector3*)textureData + texIndex;
                float* hdrData = srcTextureData + hdrIndex;
                texData->r = _linear2srgb((float)((*hdrData) * colorScale));
                texData->g = _linear2srgb((float)((*(hdrData + 1)) * colorScale));
                texData->b = _linear2srgb((float)((*(hdrData + 2)) * colorScale));
            }
        }

        // Unlock texture.
        (*texture)->UnlockRect(0);

        // Release raw image data.
        YW_SAFE_DELETE_ARRAY(texDataRaw);

        return true;
    }
}
