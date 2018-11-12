// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d types header file.

#ifndef __YW_3D_TYPES_H__
#define __YW_3D_TYPES_H__

#include "Math/YwMath.h"

// ------------------------------------------------------------------
// Platform-dependent definitions and includes.

// Windows platform.
#if defined(_WIN32) || defined(WIN32)
#define WIN32_LEAN_AND_MEAN // Restrict inclusions.
#include <windows.h>
typedef HWND WindowHandle;  // Define window-handle for the Win32-platform.
#endif

// Linux platform.
#ifdef LINUX_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
typedef Window WindowHandle;    // Define window-handle for the Linux-platform.
#endif

// Mac OSX platform.
#ifdef OSX
typedef xxx WindowHandle;   // Define window-handle for the Linux-platform.
#endif

// ------------------------------------------------------------------
// Constants

const uint32_t YW3D_VERTEX_CACHE_SIZE = 32;         // Specifies the size of the vertex cache. Minimum is 3!
const uint32_t YW3D_VERTEX_SHADER_REGISTERS = 8;    // Specifies the amount of available vertex shader input registers.
const uint32_t YW3D_PIXEL_SHADER_REGISTERS = 8;     // Specifies the amount of available vertex shader output registers, which are simulateously used as pixel shader input registers.
const uint32_t YW3D_NUM_SHADER_CONSTANTS = 32;      // Specifies the amount of available shader constants-registers for both vertex and pixel shaders.
const uint32_t YW3D_MAX_VERTEX_STREAMS = 8;         // Specifies the amount of available vertex streams.
const uint32_t YW3D_MAX_TEXTURE_SAMPLERS = 16;      // Specifies the amount of available texture samplers.
const uint32_t YW3D_CLIP_VERTEX_CACHE_SIZE = 20;    // Specifies the amount of clipping vertex cache size.

// ------------------------------------------------------------------
// Enumerations.

// Defines the transform states.
enum Yw3dTransformState
{
    Yw3d_TS_World = 0,          // Identifies the transform matrix to set for the world matrix at index. Multiple world matrices are used only for vertex blending. Otherwise only YW3D_TS_World is used.
    Yw3d_TS_View,               // Identifies the transformation matrix being set as the view transformation matrix. The default value is the identity matrix.
    Yw3d_TS_Projection,         // Identifies the transformation matrix being set as the projection transformation matrix. The default value is the identity matrix.
    Yw3d_TS_WVP,                // The combine of YW3D_TS_World, Yw3d_TS_View and Yw3d_TS_Projection.

    Yw3d_TS_NumTransformState,
};

// Defines the available renderstates.
enum Yw3dRenderState
{
	Yw3d_RS_ZEnable = 0,    // Set this to true to enable depth-buffering or to false to disable it.
	Yw3d_RS_ZWriteEnable,   // Set this to true(default) to enable writing to the depth-buffer during rasterization. If no depth-buffer is available or has been disabled by setting Yw3d_RS_ZEnable to false, this renderstate has no effect.
	Yw3d_RS_ZFunc,          // Compare-function used for depth-buffer. Set this renderstate to a member of the enumeration Yw3dCompareFunction. Default: Yw3d_CMP_Less.

    Yw3d_RS_StencilEnable,      // bool enable/disable stenciling.
    Yw3d_RS_StencilFail,        // D3DSTENCILOP to do if stencil test fails.
    Yw3d_RS_StencilZFail,       // D3DSTENCILOP to do if stencil test passes and Z test fails.
    Yw3d_RS_StencilPass,        // D3DSTENCILOP to do if both stencil and Z tests pass.
    Yw3d_RS_StencilFunc,        // D3DCMPFUNC fn. Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true.
    Yw3d_RS_StencilRef,         // Reference value used in stencil test.
    Yw3d_RS_StencilMask,        // Mask value used in stencil test.
    Yw3d_RS_StencilWriteMask,   // Write mask applied to values written to stencil buffer.

	Yw3d_RS_ColorWriteEnable,   // Set this to true(default) to enable writing to the color-buffer during rasteriation. If no color-buffer is available this renderstate has no effect.
	Yw3d_RS_FillMode,           // FillMode. Set this renderstate to a member of the enumeration Yw3dFill. Default: Yw3d_Fill_Solid.

	Yw3d_RS_CullMode,   // CullMode. Set this renderstate to a member of the enumeration Yw3dCull. Default: Yw3d_Cull_CCW.

	Yw3d_RS_SubdivisionMode,                // SubdivisionMode. Set this renderstate to a member of the enumeration Yw3dSubdivision. Default: Yw3d_Subdivision_None.
	Yw3d_RS_SubdivisionLevels,              // This renderstate specifies the number of recursive subdivision when using simple or smooth subdivision. It specifies the maximum number of recursive subdivisions of triangles' edges when using adaptive subdivision. In case subdivision has been disabled, this renderstate has no effect. Valid values are integers > 0 - if this renderstate has been set to 0, DrawPrimitive()-calls will fail. Default: 1.
	Yw3d_RS_SubdivisionPositionRegister,    // This renderstate is only used when using smooth subdivision. It specifies the vertex shader input register which holds position-data. Make sure vertex positions have been homogenized (w=1)! In case subdivision has been disabled, this renderstate has no effect. Valid values are integers e [0,YW3D_VERTEX_SHADER_REGISTERS]. Default: 0.
	Yw3d_RS_SubdivisionNormalRegister,      // This renderstate is only used when using smooth subdivision. It specifies the vertex shader input register which holds normal-data. For best results make sure that the normals have been normalized. In case subdivision has been disabled, this renderstate has no effect. Valid values are integers e [0,YW3D_VERTEX_SHADER_REGISTERS]. Default: 1.
	Yw3d_RS_SubdivisionMaxScreenArea,		// This renderstate is only used when using adaptive subdivision. Triangles, which cover more than the set area in screenspace (rendertarget's viewport is respected), are recursivly subdividied. In case subdivision has been disabled, this renderstate has no effect. Valid values are floats > 0.0f - if this renderstate has been set to 0.0f, DrawPrimitive()-calls will fail. Default: 1.0f.
	Yw3d_RS_SubdivisionMaxInnerLevels,      // This renderstate is only used when using adaptive subdivision. It specifies the maximum number of recursive subdivisions of triangles. In case subdivision has been disabled, this renderstate has no effect. Valid values are integers > 0 - if this renderstate has been set to 0, DrawPrimitive()-calls will fail. Default: 1.

	Yw3d_RS_ScissorTestEnable,  // Set this to true to enable scissor testing. Make sure that a scissor rect has been set before calling DrawPrimitive()-functions. Set this to false(default) to disable scissor testing.
	
	Yw3d_RS_LineThickness,  // Controls the thickness of rendered lines Valid values are integers >= 1. Default: 1.

	Yw3d_RS_NumRenderStates
};

// Defines the supported compare functions.
enum Yw3dCompareFunction
{
	Yw3d_CMP_Never,         // Compares will never pass.
    Yw3d_CMP_Equal,         // Compares of two values will pass if they are equal.
    Yw3d_CMP_NotEqual,      // Compares of two values will pass if they are not equal.
    Yw3d_CMP_Less,          // Compares of two values a and b will pass if a < b.
    Yw3d_CMP_LessEqual,     // Compares of two values a and b will pass if a <= b.
    Yw3d_CMP_Greater,       // Compares of two values a and b will pass if a > b.
    Yw3d_CMP_GreaterEqual,  // Compares of two values a and b will pass if a >= b.
    Yw3d_CMP_Always			// Compares will always pass.
};

// Defines the stencil operation.
enum Yw3dStencilOperaton
{
    Yw3d_StencilOp_Keep,        // Specifies to not change the stencil buffer.(that is, keep the value currently there)
    Yw3d_StencilOp_Zero,        // Specifies to set the stencil buffer entry to zero.
    Yw3d_StencilOp_Replace,     // REPLACE¡ªSpecifies to replace the stencil buffer entry with the stencil reference value.
    Yw3d_StencilOp_IncrSat,     // Specifies to increment the stencil buffer entry. If the incremented value exceeds the maximum allowed value, we clamp the entry to that maximum.
    Yw3d_StencilOp_DecrSat,     // Specifies to decrement the stencil buffer entry.If the decremented value is less than zero, we clamp the entry to zero.
    Yw3d_StencilOp_Invert,      // Specifies to invert the bits of the stencil buffer entry.
    Yw3d_StencilOp_Incr,        // Specifies to increment the stencil buffer entry. If the incremented value exceeds the maximum allowed value, we wrap to zero.
    Yw3d_StencilOp_Decr         // Specifies to decrement the stencil buffer entry.If the decremented value is less than zero, we wrap to the maximum allowed value.
};

// Defines the supported culling modes.
enum Yw3dCull
{
	Yw3d_Cull_None, // Back faces won't be culled.
	Yw3d_Cull_CW,   // Culls faces with clockwise vertices.
	Yw3d_Cull_CCW   // Culls faces with counterclockwise vertices (default).
};

// Defines the supported fill modes.
enum Yw3dFill
{
	Yw3d_Fill_Solid,    // Triangles are filled during rasterization (default).
	Yw3d_Fill_WireFrame // Only triangle's edges are drawn.
};

// Defines the available Texture Sampler States.
enum Yw3dTextureSamplerState
{
	Yw3d_TSS_AddressU = 0,  // Texture addressing mode for the u-coordinate. Set this renderstate to a member of the enumeration Yw3dTextureAddress. Default: Yw3d_TA_Wrap.
	Yw3d_TSS_AddressV,      // Texture addressing mode for the v-coordinate. Set this renderstate to a member of the enumeration Yw3dTextureAddress. Default: Yw3d_TA_Wrap.
	Yw3d_TSS_AddressW,      // Texture addressing mode for the w-coordinate. Set this renderstate to a member of the enumeration Yw3dTextureAddress. Default: Yw3d_TA_Wrap.
	Yw3d_TSS_MinFilter,     // Texture minification filtering mode. Set this renderstate to a member of the enumeration Yw3dTextureFilter. Default: Yw3d_TF_Linear.
	Yw3d_TSS_MagFilter,     // Texture magnification filtering mode. Set this renderstate to a member of the enumeration Yw3dTextureFilter. Default: Yw3d_TF_Linear.
	Yw3d_TSS_MipFilter,     // Mipmap filtering mode. Set this renderstate to a member of the enumeration Yw3dTextureFilter. Default: Yw3d_TF_Point.
	Yw3d_TSS_MipLodBias,    // Floating point value added to the mip-level when sampling textures. Default: 0.0f.
	Yw3d_TSS_MaxMipLevel,   // Floating point value which specifies the smallest mip-level to be used, e.g. 3.0f would mean that the third mip-level is the smallest to be used. Set this to 0.0f to force the use of the largest mip-level. default: 16.0f

	Yw3d_TSS_NumTextureSamplerStates
};

// Defines the supported texture addressing-modes.
enum Yw3dTextureAddress
{
    Yw3d_TA_Wrap,
    Yw3d_TA_Clamp
};

// Defines the supported texture filters.
enum Yw3dTextureFilter
{
    Yw3d_TF_Point,    // Specifies nearest point sampling.
    Yw3d_TF_Linear    // Specifies linear filtering.
};

// Specifies the supported subdivision modes.
enum Yw3dSubdivision
{
	Yw3d_Subdivision_None,       // Triangle subdivision is disabled.
	Yw3d_Subdivision_Simple,     // This subdivision mode simply subdivides each triangle an user specified number of times recursively.
	Yw3d_Subdivision_Smooth,     // This subdivision mode is a simplified implementation of ATI's TruForm: It subdivides each triangle an user specified number of times recursively and attempts to generate a smooth triangle-surface using vertex normals as a base. This subdivision mode requires access to vertex position and normal. (For best results make sure that normal-vectors are normalized.)
	Yw3d_Subdivision_Adaptive    // This subdivision mode splits each triangle's edges an user specified number of times recursively. The triangle is then subdivided until its sub-triangles cover no more than a user specified area in clipping space.
};

// Defines the supported texture and buffer formats.
// The default value for formats that contain undefined channels is 1.0f for the undefined alpha channel and 0.0f for undefined color channels.
// E.g. Yw3d_FMT_R32G32B32F doesn't define the alpha channel, which is therefore set to 1.0f.
enum Yw3dFormat
{
    // Texture formats
    Yw3d_FMT_R32F,          // 32-bit texture format, one float mapped to the red channel.
    Yw3d_FMT_R32G32F,       // 64-bit texture format, two floats mapped to the red and green channel.
    Yw3d_FMT_R32G32B32F,    // 96-bit texture format, three floats mapped to the three color channel.
    Yw3d_FMT_R32G32B32A32F, // 128-bit texture format, four floats mapped to the three color channel plus the alpha channel.

    // Index buffer formats.
    Yw3d_FMT_INDEX16,       // 16-bit index buffer format, indices are shorts.
    Yw3d_FMT_INDEX32        // 32-bit index buffer format, indices are integers.
};

// Defines the supported primitive types.
enum Yw3dPrimitiveType
{
    Yw3d_PT_TriangleFan,    // Triangle fan.
    Yw3d_PT_TriangleStrip,  // Triangle strip.
    Yw3d_PT_TriangleList    // Triangle lists.
};

// Defines the supported vertex elements.
enum Yw3dVertexelEmentType
{
    Yw3d_VET_Float,         // Specifies a single float. The vertex element is expanded to vector4(float, 0, 0, 1) and mapped to a vertex shader input register.
    Yw3d_VET_Vector2,       // Specifies two floats. The vertex element is expanded to vector4(float, float, 0, 1) and mapped to a vertex shader input register.
    Yw3d_VET_Vector3,       // Specifies three floats. The vertex element is expanded to vector4(float, float, float, 1) and mapped to a vertex shader input register.
    Yw3d_VET_Vector4        // Specifies four floats. The vertex element is directly mapped to a vertex shader input register.
};

// Specifies the available types of vectors used for sampling different textures.
enum Yw3dTextureSampleInput
{
	Yw3d_TSI_2Coords,   // 2 floating point coordinates used for standard 2d texture-sampling.
	Yw3d_TSI_3Coords,   // 3 floating point coordinates used for volume texture-sampling.
	Yw3d_TSI_Vector     // 3-dimensional vector used for cubemap-sampling.
};

// Specifies indices for the 6 cubemap faces.
enum Yw3dCubeFaces
{
	Yw3d_CF_Positive_X = 0, // face +x
	Yw3d_CF_Negative_X,     // face -x
	Yw3d_CF_Positive_Y,     // face +y
	Yw3d_CF_Negative_Y,     // face -y
	Yw3d_CF_Positive_Z,     // face +z
	Yw3d_CF_Negative_Z      // face -z
};

// Specifies the supported pixel shader types.
enum Yw3dPixelShaderOutput
{
	Yw3d_PSO_ColorOnly,     // Specifies that a given pixel shader only outputs color (default). A pixel's depth values will be automatically computed by the rasterizer through interpolation of depth values of a triangle's vertices.
	Yw3d_PSO_ColorDepth,    // Specifies that a given pixel shader outputs both color and depth. If you want to output only depth use renderstate Yw3d_RS_ColorWriteEnable to disable writing color.
};

// Specifies the type of a particular shader register.
enum Yw3dShaderRegisterType
{
	Yw3d_SRT_Unused = 0,    // Specifies that the register is unused.
	Yw3d_SRT_Float32,       // Specifies that the register should be treated as a single float.
	Yw3d_SRT_Vector2,       // Specifies that the register should be treated as a 2-dimensional vector.
	Yw3d_SRT_Vector3,       // Specifies that the register should be treated as a 3-dimensional vector.
	Yw3d_SRT_Vector4        // Specifies that the register should be treated as a 4-dimensional vector.
};

// Specifies the frustum clipping planes.
enum Yw3dClippingPlanes
{
    // Basic frustum planes.
	Yw3d_CP_Left = 0,   // Left frustum clipping plane.
	Yw3d_CP_Right,      // Right frustum clipping plane.
	Yw3d_CP_Top,        // Top frustum clipping plane.
	Yw3d_CP_Bottom,     // Bottom frustum clipping plane.
	Yw3d_CP_Near,       // Near frustum clipping plane.
	Yw3d_CP_Far,        // Far frustum clipping plane.

    // Basic frustum plane number.
    Yw3d_CP_NumFrustumPlanes,

	// User specified clipping planes.
	Yw3d_CP_User0 = Yw3d_CP_NumFrustumPlanes,
	Yw3d_CP_User1,
	Yw3d_CP_User2,
	Yw3d_CP_User3,

	Yw3d_CP_NumPlanes
};

// ------------------------------------------------------------------
// Structures.

namespace yw
{
    // Defines a rectangle.
    struct Yw3dRect
    {
        uint32_t left;
        uint32_t right;
        uint32_t top;
        uint32_t bottom;

        Yw3dRect() : left(0), right(0), top(0), bottom(0) {}
    };

    // Defines a box. Added for volume texture support.
    struct Yw3dBox
    {
        uint32_t left;
        uint32_t right;
        uint32_t top;
        uint32_t bottom;
        uint32_t front;
        uint32_t back;

        Yw3dBox() : left(0), right(1), bottom(0), top(1), back(0), front(1) {}
    };

    // This structure defines the device parameters.
    struct Yw3dDeviceParameters
    {
        // Handle to the output window.
        WindowHandle deviceWindow;

        // True if the application runs windowed, false if it runs in full-screen.
        bool windowed;

        // Bit-depth of backbuffer in fullscreen mode (ignored in windowed mode). Valid values: 32, 24, 16.
        uint32_t fullScreenColorBits;

        // Width of dimension of the backbuffer in Pixels.
        uint32_t backBufferWidth;

        // Height of dimension of the backbuffer in Pixels.
        uint32_t backBufferHeight;

        // Constructor.
        Yw3dDeviceParameters() : deviceWindow(nullptr), windowed(false), fullScreenColorBits(32), backBufferWidth(0), backBufferHeight(0) {}
        Yw3dDeviceParameters(WindowHandle windowHandle, bool useWindowed, uint32_t colorBits, uint32_t width, uint32_t height) : deviceWindow(windowHandle), windowed(useWindowed), fullScreenColorBits(colorBits), backBufferWidth(width), backBufferHeight(height) {}
    };

    // Describes a vertex element.
    struct Yw3dVertexElement
    {
        // Index of the stream this element is loaded from.
        uint32_t stream;

        // Type of this vertex element. Set this field to a member of the enumeration Yw3dVertexelEmentType.
        Yw3dVertexelEmentType type;

        // The register of the vertex shader the vertex element's value will be passed to.
        uint32_t shaderRegister;

        //Yw3dVertexElement() : stream(-1), type(Yw3d_VET_Vector3), shaderRegister(0) {}
    };

    // Helper-macro for vertex format declaration.
    #define YW3D_VERTEX_FORMAT_DECL(Stream, Type, Register) {(Stream), (Type), (Register)}
}

// ------------------------------------------------------------------
// Internal structures.

namespace yw
{
    // float point format of vector.
    typedef Vector2 float2;
    typedef Vector3 float3;
    typedef Vector4 float4;

    // float point format of matrix.
    typedef Matrix33 float33;
    typedef Matrix44 float44;

    // A shader register is 128-bits wide and is divided into four floating-point-values.
    typedef float4 Yw3dShaderRegister;

    // Describes the vertex shader input.
    // @note This structure is used internally by devices.
    struct Yw3dVSInput
    {
        // Vertex shader input registers.
        Yw3dShaderRegister shaderInputs[YW3D_VERTEX_SHADER_REGISTERS];

        Yw3dVSInput() {}
    };

    // Describes the vertex shader output.
    // @note This structure is used internally by devices.
    struct Yw3dVSOutput
    {
        // Original vertex shader input fetched from vertex streams; added for triangle subdivision.
        Yw3dVSInput sourceInput;

        // Position of this vertex.(projected)
        Vector4 position;

        // Vertex shader output registers, which are in turn used as pixel shader input registers.
        Yw3dShaderRegister shaderOutputs[YW3D_PIXEL_SHADER_REGISTERS];

        Yw3dVSOutput() {}
    };

    // Describes a structure that is used for triangle gradient storage.
    // @note This structure is used internally by devices.
    struct Yw3dTriangleInfo
    {
        // Gradient constant.
        //float commonGradient;

        // Base vertex for gradient computations.
        const Yw3dVSOutput* baseVertex;

        // z partial derivatives with respect to the screen-space x- and y-coordinates.
	    float zDdx;
        float zDdy;

        // w partial derivatives with respect to the screen-space x- and y-coordinates.
        float wDdx;
        float wDdy;

        // Shader register partial derivatives with respect to the screen-space x-coordinate.
        Yw3dShaderRegister shaderOutputsDdx[YW3D_PIXEL_SHADER_REGISTERS];

        // Shader register partial derivatives with respect to the screen-space y-coordinate.
        Yw3dShaderRegister shaderOutputsDdy[YW3D_PIXEL_SHADER_REGISTERS];

        // Integer-coordinates of current pixel; needed by pixel shader for computation of partial derivatives.
        uint32_t curPixelX;
        uint32_t curPixelY;

        // 1.0f / w of the current pixel; needed by pixel shader for computation of partial derivatives.
        float curPixelInvW;

        Yw3dTriangleInfo() : /*commonGradient(0.0f),*/ baseVertex(nullptr), zDdx(0.0f), zDdy(0.0f), wDdx(0.0f), wDdy(0.0f), curPixelX(0), curPixelY(0), curPixelInvW(1.0f) {}
    };

    // Describes a structure that is used for vertex caching.
    // @note This structure is used internally by devices.
    struct Yw3dVertexCacheEntry
    {
        // Index of the contained vertex in the vertex buffer.
        uint32_t vertexIndex;

        // Vertex shader output, vertex data.
        Yw3dVSOutput vertexOutput;

        // Whenever a vertex cache entry is reserved for drawing (updated or simply 'touched and returned') its fetch-time is set to m_FetchedVertices.
        uint32_t fetchTime;

        Yw3dVertexCacheEntry() : vertexIndex(0), fetchTime(0) {}
    };
}

#endif // !__YW_3D_TYPES_H__
