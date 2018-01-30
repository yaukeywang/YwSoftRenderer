// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d types header file.

#ifndef __YW_3D_TYPES_H__
#define __YW_3D_TYPES_H__

// ------------------------------------------------------------------
// Enumerations.

enum Yw3dFormat
{
    // Texture formats
    Yw3d_FMT_R32F,          // 32-bit texture format, one float mapped to the red channel.
    Yw3d_FMT_R32G32F,       // 64-bit texture format, two floats mapped to the red and green channel.
    Yw3d_FMT_R32G32B32F,    // 96-bit texture format, three floats mapped to the three color channel.
    Yw3d_FMT_R32G32B32A32F, // 128-bit texture format, four floats mapped to the three color channel plus the alpha channel.

    // Indexbuffer formats.
    Yw3d_FMT_INDEX16,       // 16-bit indexbuffer format, indices are shorts.
    Yw3d_FMT_INDEX32        // 32-bit indexbuffer format, indices are integers.
};

// ------------------------------------------------------------------
// Structures.

// Defines the supported texturefilters.
enum Yw3dTextureFilter
{
    Yw3d_TF_Point,    // Specifies nearest point sampling.
    Yw3d_TF_Linear    // Specifies linear filtering.
};

// Defines a rectangle.
struct Yw3dRect
{
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;

    Yw3dRect() : left(0), right(0), top(0), bottom(0) {}
};

#endif // !__YW_3D_TYPES_H__
