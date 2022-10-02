// Add by Yaukey at 2022-10-01.
// rgbe file read/write implementation.

#include "rgbe.h"
#include <cmath>

// ------------------------------------------------------------------
// RGBE file parsing code from: https://www.graphics.cornell.edu/~bjw/rgbe.html.
// Changing a little bit avoid using "FILE".

#ifndef RGBE_SAFE_DELETE
#define RGBE_SAFE_DELETE(p) {if (nullptr != (p)) {delete (p); (p) = nullptr;}}
#endif

#ifndef RGBE_SAFE_DELETE_ARRAY
#define RGBE_SAFE_DELETE_ARRAY(p) {if (nullptr != (p)) {delete [] (p); (p) = nullptr;}}
#endif

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

/* RGBE error code. */
enum rgbe_error_codes {
    rgbe_read_error,
    rgbe_write_error,
    rgbe_format_error,
    rgbe_memory_error,
};

/* default error routine.  change this to change error handling */
static int32_t _rgbe_error(int32_t rgbe_error_code, char* msg)
{
    switch (rgbe_error_code) {
    case rgbe_read_error:
        perror("RGBE read error");
        break;
    case rgbe_write_error:
        perror("RGBE write error");
        break;
    case rgbe_format_error:
        fprintf(stderr, "RGBE bad file format: %s\n", msg);
        break;
    default:
    case rgbe_memory_error:
        fprintf(stderr, "RGBE error: %s\n", msg);
    }

    return RGBE_RETURN_FAILURE;
}

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

/* Standard conversion from float pixels to rgbe pixels. */
/* NOTE: you can remove the "inline"s if your compiler complains about it. */
static void _float2rgbe(uint8_t rgbe[4], float red, float green, float blue)
{
    float v = 0.0f;
    int32_t e = 0;

    v = red;
    if (green > v) v = green;
    if (blue > v) v = blue;
    if (v < 1e-32)
    {
        rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
    }
    else
    {
        v = frexp(v, &e) * 256.0f / v;
        rgbe[0] = (uint8_t)(red * v);
        rgbe[1] = (uint8_t)(green * v);
        rgbe[2] = (uint8_t)(blue * v);
        rgbe[3] = (uint8_t)(e + 128);
    }
}

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
        *red = *green = *blue = 0.0f;
    }
}

int32_t RGBEReadHeaderFromData(const uint8_t* data, RGBEHeader* header)
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
            memcpy(header->primaries, primaries, sizeof(primaries));
        }
    }

    // Read line to get width and height.
    dataMoved += _GetLineFromString(dataHead + dataMoved, buff, sizeof(buff) / sizeof(buff[0]));

    // Maybe more format later: +Y -X.
    int32_t width = 0;
    int32_t height = 0;
    if (sscanf(buff, "-Y %d +X %d", &height, &width) != 2)
    {
        return -1;
    }

    header->width = width;
    header->height = height;

    return dataMoved;
}

bool RGBEReadPixelsFromData(const uint8_t* srcData, float* dstData, int pixels)
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

bool RGBEReadPixelsRLEFromData(const uint8_t* srcData, float* dstData, int32_t scanlineWidth, int32_t numScanlines)
{
    uint8_t* srcDataHead = (uint8_t*)srcData;

    if ((scanlineWidth < 8) || (scanlineWidth > 0x7fff))
    {
        /* run length encoding is not allowed so read flat*/
        return RGBEReadPixelsFromData(srcData, dstData, scanlineWidth * numScanlines);
    }

    // Scanline buffer.
    uint8_t* scanline_buffer = nullptr;

    /* read in each successive scanline */
    while (numScanlines > 0)
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
            RGBE_SAFE_DELETE_ARRAY(scanline_buffer);
            return RGBEReadPixelsFromData(srcDataHead, dstData, scanlineWidth * numScanlines - 1);
        }

        if ((((int)rgbe[2]) << 8 | rgbe[3]) != scanlineWidth)
        {
            RGBE_SAFE_DELETE_ARRAY(scanline_buffer);
            _rgbe_error(rgbe_format_error, "wrong scanline width");

            return false;
        }

        if (nullptr == scanline_buffer)
        {
            scanline_buffer = new uint8_t[sizeof(uint8_t) * 4 * scanlineWidth];
        }

        if (nullptr == scanline_buffer)
        {
            _rgbe_error(rgbe_memory_error, "unable to allocate buffer space");
            return false;
        }

        uint8_t* ptr = &scanline_buffer[0];

        /* read each of the four channels for the scanline into the buffer */
        for (int32_t i = 0; i < 4; i++)
        {
            uint8_t* ptr_end = &scanline_buffer[(i + 1) * scanlineWidth];
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
                        RGBE_SAFE_DELETE_ARRAY(scanline_buffer);
                        _rgbe_error(rgbe_format_error, "bad scanline data");

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
                        RGBE_SAFE_DELETE_ARRAY(scanline_buffer);
                        _rgbe_error(rgbe_format_error, "bad scanline data");

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
        for (int32_t i = 0; i < scanlineWidth; i++)
        {
            rgbe[0] = scanline_buffer[i];
            rgbe[1] = scanline_buffer[i + scanlineWidth];
            rgbe[2] = scanline_buffer[i + 2 * scanlineWidth];
            rgbe[3] = scanline_buffer[i + 3 * scanlineWidth];
            _rgbe2float(&dstData[RGBE_DATA_RED], &dstData[RGBE_DATA_GREEN], &dstData[RGBE_DATA_BLUE], rgbe);
            dstData += RGBE_DATA_SIZE;
        }

        numScanlines--;
    }

    RGBE_SAFE_DELETE_ARRAY(scanline_buffer);
    return true;
}

uint32_t RGBEWriteHeaderToData(const RGBEHeader* header, std::vector<uint8_t>& rgbeData)
{
    if (nullptr == header)
    {
        return 0;
    }

    std::string result;
    result.append(RGBE_HEADER_MAGIC).append("\n");
    result.append("# Created by YwSoftRenderer. [https://github.com/yaukeywang/YwSoftRenderer]").append("\n");

    if (RGBEFormat_XYZE == header->format)
    {
        result.append(RGBE_FORMAT_XYZE_MAGIC).append("\n");
    }
    else
    {
        result.append(RGBE_FORMAT_RGBE_MAGIC).append("\n");
    }
    
    // Alloc a temp buffer to read line.
    char buff[256];
    memset(buff, 0, sizeof(buff));

    sprintf(buff, "EXPOSURE=%g", header->exposure);
    result.append(buff).append("\n");

    sprintf(buff, "GAMMA=%g", header->gamma);
    result.append(buff).append("\n");

    // We do not write this currently.
    //sprintf(buff, "PRIMARIES=%g %g %g %g %g %g %g %g", header->primaries[0], header->primaries[1], header->primaries[2], header->primaries[3], header->primaries[4], header->primaries[5], header->primaries[6], header->primaries[7]);
    //result.append(buff).append("\n");

    // Extra line before width and height.
    result.append("\n");

    sprintf(buff, "-Y %d +X %d", header->height, header->width);
    result.append(buff).append("\n");

    // Add to buffer.
    for (int32_t i = 0; i < result.length(); i++)
    {
        rgbeData.push_back(result.at(i));
    }

    return (uint32_t)result.length();
}

uint32_t RGBEWritePixelsToData(float* pixelData, uint32_t pixelCount, bool hasAlpha, std::vector<uint8_t>& rgbeData)
{
    if ((nullptr == pixelData) || (0 == pixelCount))
    {
        return 0;
    }

    rgbeData.reserve(rgbeData.size() + (pixelCount * 4));

    const int32_t stride = hasAlpha ? 4 : 3;
    for (uint32_t i = 0; i < pixelCount; i++)
    {
        uint8_t rgbe[4];
        _float2rgbe(rgbe, *pixelData, *(pixelData + 1), *(pixelData + 2));
        pixelData += stride;

        rgbeData.push_back(rgbe[0]);
        rgbeData.push_back(rgbe[1]);
        rgbeData.push_back(rgbe[2]);
        rgbeData.push_back(rgbe[3]);
    }
    
    return (uint32_t)rgbeData.size();
}

uint32_t RGBEWritePixelsRLEToData(float* pixelData, uint32_t pixelCount, bool hasAlpha, std::vector<uint8_t>& rgbeData)
{
    // Not implemented yet!
    return 0;
}
