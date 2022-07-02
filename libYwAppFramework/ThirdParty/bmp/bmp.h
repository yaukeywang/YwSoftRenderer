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
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};
#pragma pack(pop)

// Bit map info header.
// https://docs.microsoft.com/zh-cn/previous-versions/dd183376(v=vs.85)
#pragma pack(push, 1)
struct BitMapInfoHeader
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

#endif // !__YW_BMP_H__
