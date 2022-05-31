// Add by Yaukey at 2022-07-02.
// bmp file header define.

#ifndef __YW_BMP_H__
#define __YW_BMP_H__

// ------------------------------------------------------------------
// For bit map info struct.

// File magic number.
const uint16_t BITMAP_FILE_MAGIC = ('M' << 8) | 'B';

// Bit map file header.
#pragma pack(push, 1)
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
struct BitMapFileHeader
{
    uint16_t bfType; // The file type; must be BM.
    uint32_t bfSize; // The size, in bytes, of the bitmap file.
    uint16_t bfReserved1; // Reserved; must be zero.
    uint16_t bfReserved2; // Reserved; must be zero.
    uint32_t bfOffBits; // The offset, in bytes, from the beginning of the BITMAPFILEHEADER structure to the bitmap bits.
};
#pragma pack(pop)

// Bit map info header.
// https://docs.microsoft.com/zh-cn/previous-versions/dd183376(v=vs.85)
#pragma pack(push, 1)
struct BitMapInfoHeader
{
    uint32_t biSize; // The number of bytes required by the structure.
    int32_t biWidth; // The width of the bitmap, in pixels. The width of the bitmap, in pixels.
    int32_t biHeight; // The height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner. If biHeight is negative, indicating a top-down DIB, biCompression must be either BI_RGB or BI_BITFIELDS. Top-down DIBs cannot be compressed. If biCompression is BI_JPEG or BI_PNG, the biHeight member specifies the height of the decompressed JPEG or PNG image file, respectively.
    uint16_t biPlanes; // The number of planes for the target device. This value must be set to 1.ssss
    uint16_t biBitCount; // The number of bits-per-pixel. The biBitCount member of the BITMAPINFOHEADER structure determines the number of bits that define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values. See More in documents.
    uint32_t biCompression; // The number of bits-per-pixel. The biBitCount member of the BITMAPINFOHEADER structure determines the number of bits that define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values. See More in documents.
    uint32_t biSizeImage; // The size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps. If biCompression is BI_JPEG or BI_PNG, biSizeImage indicates the size of the JPEG or PNG image buffer, respectively.
    int32_t biXPelsPerMeter; // The horizontal resolution, in pixels-per-meter, of the target device for the bitmap. An application can use this value to select a bitmap from a resource group that best matches the characteristics of the current device.
    int32_t biYPelsPerMeter; // The vertical resolution, in pixels-per-meter, of the target device for the bitmap.
    uint32_t biClrUsed; // The number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap uses the maximum number of colors corresponding to the value of the biBitCount member for the compression mode specified by biCompression. See More in documents.
    uint32_t biClrImportant; // The number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are required.ssssssss
};
#pragma pack(pop)

#endif // !__YW_BMP_H__
