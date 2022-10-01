// Add by Yaukey at 2022-07-02.
// rgbe file header define.

#ifndef __YW_RGBE_H__
#define __YW_RGBE_H__

#include <stdint.h>

// ------------------------------------------------------------------
// RGBE file parsing code from: https://www.graphics.cornell.edu/~bjw/rgbe.html.
// Changing a little bit avoid using "FILE".

// RGBE file format.
enum RGBEFormat
{
    RGBEFormat_RGBE,
    RGBEFormat_XYZE
};

// RGBE header info.
struct RGBEHeader
{
    int32_t format;
    float exposure; /* a value of 1.0 in an image corresponds to <exposure> watts/steradian/m^2. defaults to 1.0 */
    float gamma; /* image has already been gamma corrected with given gamma.  defaults to 1.0 (no correction) */
    float primaries[8];
    int32_t width;
    int32_t height;

    RGBEHeader() : format(RGBEFormat_RGBE), exposure(0.0f), gamma(0.0f), width(0), height(0) {}
};

/* flags indicating which fields in an rgbe_header_info are valid */
#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA 0x02
#define RGBE_VALID_EXPOSURE 0x04

/* return codes for rgbe routines */
#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

// Read rgbe file header, return data moved length from original data.
int32_t RGBEReadHeaderFromData(const uint8_t* data, RGBEHeader* header);

// Simple read routine. will not correctly handle run length encoding.
bool RGBEReadPixelsFromData(const uint8_t* srcData, float* dstData, int pixels);

// Reading pixels. Will correctly handle run length encoding.
bool RGBEReadPixelsRLEFromData(const uint8_t* srcData, float* dstData, int32_t scanlineWidth, int32_t numScanlines);

#endif // !__YW_RGBE_H__
