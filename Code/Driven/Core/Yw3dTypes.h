// Add by Yaukey at 2018-01-29.
// YW Soft Renderer 3d types header file.

#ifndef __YW_3D_TYPES_H__
#define __YW_3D_TYPES_H__

// ------------------------------------------------------------------
// Constants

const uint32_t YW3D_VERTEX_CACHE_SIZE = 32;         // Specifies the size of the vertex cache. Minimum is 3!
const uint32_t YW3D_VERTEX_SHADER_REGISTERS = 8;    // Specifies the amount of available vertex shader input registers.
const uint32_t YW3D_PIXEL_SHADER_REGISTERS = 8;     // Specifies the amount of available vertex shader output registers, which are simulateously used as pixel shader input registers.
const uint32_t YW3D_NUM_SHADER_CONSTANTS = 32;      // Specifies the amount of available shader constants-registers for both vertex and pixel shaders.
const uint32_t YW3D_MAX_VERTEX_STREAMS = 8;         // Specifies the amount of available vertex streams.
const uint32_t YW3D_MAX_TEXTURE_SAMPLERS = 16;      // Specifies the amount of available texture samplers.

// ------------------------------------------------------------------
// Enumerations.

// Defines the available renderstates.
enum Yw3dRenderState
{
	Yw3d_RS_ZEnable = 0,    // Set this to true to enable depth-buffering or to false to disable it.
	Yw3d_RS_ZWriteEnable,   // Set this to true(default) to enable writing to the depth-buffer during rasterization. If no depth-buffer is available or has been disabled by setting Yw3d_RS_ZEnable to false, this renderstate has no effect.
	Yw3d_RS_ZFunc,          // Compare-function used for depth-buffer. Set this renderstate to a member of the enumeration Yw3dCmpFunc. Default: Yw3d_CMP_Less.

	Yw3d_RS_ColorWriteEnable,   // Set this to true(default) to enable writing to the color-buffer during rasteriation. If no color-buffer is available this renderstate has no effect.
	Yw3d_RS_FillMode,           // FillMode. Set this renderstate to a member of the enumeration Yw3dFill. Default: Yw3d_Fill_Solid.

	Yw3d_RS_CullMode,   // CullMode. Set this renderstate to a member of the enumeration Yw3dCull. Default: Yw3d_Cull_CCW.

	Yw3d_RS_SubdivisionMode,                // SubdivisionMode. Set this renderstate to a member of the enumeration Yw3dSubdiv. Default: Yw3d_Subdiv_None.
	Yw3d_RS_SubdivisionLevels,              // This renderstate specifies the number of recursive subdivision when using simple or smooth subdivision. It specifies the maximum number of recursive subdivisions of triangles' edges when using adaptive subdivision. In case subdivision has been disabled, this renderstate has no effect. Valid values are integers > 0 - if this renderstate has been set to 0, DrawPrimitive()-calls will fail. Default: 1.
	Yw3d_RS_SubdivisionPositionRegister,    // This renderstate is only used when using smooth subdivision. It specifies the vertex shader input register which holds position-data. Make sure vertex positions have been homogenized (w=1)! In case subdivision has been disabled, this renderstate has no effect. Valid values are integers e [0,YW3D_VERTEX_SHADER_REGISTERS]. Default: 0.
	Yw3d_RS_SubdivisionNormalRegister,      // This renderstate is only used when using smooth subdivision. It specifies the vertex shader input register which holds normal-data. For best results make sure that the normals have been normalized. In case subdivision has been disabled, this renderstate has no effect. Valid values are integers e [0,YW3D_VERTEX_SHADER_REGISTERS]. Default: 1.
	Yw3d_RS_SubdivisionMaxScreenArea,		// This renderstate is only used when using adaptive subdivision. Triangles, which cover more than the set area in screenspace (rendertarget's viewport is respected), are recursivly subdividied. In case subdivision has been disabled, this renderstate has no effect. Valid values are floats > 0.0f - if this renderstate has been set to 0.0f, DrawPrimitive()-calls will fail. Default: 1.0f.
	Yw3d_RS_SubdivisionMaxInnerLevels,      // This renderstate is only used when using adaptive subdivision. It specifies the maximum number of recursive subdivisions of triangles. In case subdivision has been disabled, this renderstate has no effect. Valid values are integers > 0 - if this renderstate has been set to 0, DrawPrimitive()-calls will fail. Default: 1.

	Yw3d_RS_ScissorTestEnable,  // Set this to true to enable scissor testing. Make sure that a scissor rect has been set before calling DrawPrimitive()-functions. Set this to false(default) to disable scissor testing.
	
	Yw3d_RS_LineThickness,  // Controls the thickness of rendered lines Valid values are integers >= 1. Default: 1.

	Yw3d_RS_NumRenderStates
};

// Defines the supported texture adressing-modes.
enum Yw3dTextureAddress
{
    Yw3d_TA_Wrap,
    Yw3d_TA_Clamp
};

// Defines the supported texturefilters.
enum Yw3dTextureFilter
{
    Yw3d_TF_Point,    // Specifies nearest point sampling.
    Yw3d_TF_Linear    // Specifies linear filtering.
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

    // Indexbuffer formats.
    Yw3d_FMT_INDEX16,       // 16-bit indexbuffer format, indices are shorts.
    Yw3d_FMT_INDEX32        // 32-bit indexbuffer format, indices are integers.
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
    Yw3d_VET_Float,         // Specifies a single float. The vertex element is expanded to vector4( float, 0, 0, 1 ) and mapped to a vertex shader input register.
    Yw3d_VET_Vector2,       // Specifies two floats. The vertex element is expanded to vector4( float, float, 0, 1 ) and mapped to a vertex shader input register.
    Yw3d_VET_Vector3,       // Specifies three floats. The vertex element is expanded to vector4( float, float, float, 1 ) and mapped to a vertex shader input register.
    Yw3d_VET_Vector4        // Specifies four floats. The vertex element is directly mapped to a vertex shader input register.
};

// ------------------------------------------------------------------
// Structures.

// Defines a rectangle.
struct Yw3dRect
{
    uint32_t left;
    uint32_t right;
    uint32_t top;
    uint32_t bottom;

    Yw3dRect() : left(0), right(0), top(0), bottom(0) {}
};

// This structure defines the device parameters.
struct Yw3dDeviceParameters
{

};

// Describes a vertex element.
struct Yw3dVertexElement
{
    uint32_t Stream;                // Index of the stream this element is loaded from.
    Yw3dVertexelEmentType Type;     // Type of this vertex element. Set this field to a member of the enumeration Yw3dVertexelEmentType.
    uint32_t Register;              // The register of the vertex shader the vertex element's value will be passed to.
};

// Helper-macro for vertex format declaration.
#define YW3D_VERTEX_FORMAT_DECL(Stream, Type, Register) {Stream, Type, Register}

// ------------------------------------------------------------------
// Internal structures.

#endif // !__YW_3D_TYPES_H__
