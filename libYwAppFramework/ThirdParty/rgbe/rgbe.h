// Add by Yaukey at 2022-07-02.
// rgbe file header define.

#ifndef __YW_RGBE_H__
#define __YW_RGBE_H__

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
struct RGBEHeader {
    int32_t format;
    float exposure; /* a value of 1.0 in an image corresponds to <exposure> watts/steradian/m^2. defaults to 1.0 */
    float gamma; /* image has already been gamma corrected with given gamma.  defaults to 1.0 (no correction) */
    float primaries[8];
    int32_t width;
    int32_t height;

    RGBEHeader() : format(RGBEFormat_RGBE), exposure(0.0f), gamma(0.0f), width(0), height(0) {}
};

#endif // !__YW_RGBE_H__
