// Add by Yaukey at 2018-01-30.
// YW Soft Renderer 3d device class.

#ifndef __YW_3D_DEVICE_H__
#define __YW_3D_DEVICE_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // The Yw3d device.
    class Yw3dDevice : public IBase
    {
        friend class Yw3d;

    protected:
        // Accessible by Yw3d which is the only class that may create a device.
	    // @param[in] yw3d a pointer to the parent Yw3d-object.
	    // @param[in] deviceParameters pointer to a Yw3dDeviceParameters-structure.
        Yw3dDevice(class Yw3d* yw3d, const Yw3dDeviceParameters* deviceParameters);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dDevice();

    protected:
        // Creates and initializes the device.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @return Yw3d_E_Unknown if a presenttarget-specific problem was encountered.
        Yw3dResult Create();

    public:
        // Renders nonindexed primitives of the specified type from the currently set vertex streams.
        // @param[in] primitiveType member of the enumeration Yw3dPrimitiveType, specifies the primitive's type.
        // @param[in] startVertex Beginning at this vertex the correct number used for rendering this batch will be read from the vertex streams.
        // @param[in] primitiveCount Amount of primitives to render.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult DrawPrimitive(Yw3dPrimitiveType primitiveType, uint32_t startVertex, uint32_t primitiveCount);

        // Renders geometric primitives through indexing into an array of vertices.
        // @param[in] primitiveType member of the enumeration Yw3dPrimitiveType, specifies the primitive's type.
        // @param[in] baseVertexIndex added to each index before accessing a vertex from the array.
        // @param[in] minIndex specifies the minimum index for vertices used during this batch.
        // @param[in] numVertices specifies the number of vertices that will be used beginning from baseVertexIndex + minIndex.
        // @param[in] startIndex Location in the index buffer to start reading from.
        // @param[in] primitiveCount Amount of primitives to render.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult DrawIndexedPrimitive(Yw3dPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount);

        // Samples the texture and returns the looked-up color.
        // @param[out] color receives the color of the pixel to be looked up.
        // @param[in] samplerNumber number of the sampler.
        // @param[in] u u-component of the lookup-vector.
        // @param[in] v v-component of the lookup-vector.
        // @param[in] w w-component of the lookup-vector.
        // @param[in] xGradient partial derivatives of the texture coordinates with respect to the screen-space x coordinate (optional, base for mip-level calculations).
        // @param[in] yGradient partial derivatives of the texture coordinates with respect to the screen-space y coordinate (optional, base for mip-level calculations).
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult SampleTexture(Vector4& color, uint32_t samplerNumber, float u, float v, float w = 0.0f, const Vector4* xGradient = nullptr, const Vector4* yGradient = nullptr);

    private:
        // Prepares internal structure with information used for rendering.
        // Checks if all necessary objects (vertexbuffer, vertex format, etc.) have been set + if renderstates are valid.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult PreRender();

        // Performs cleanup: Unlocking frame- and depthbuffer, etc.
        void PostRender();

        // Loads data of a particular vertex from the vertex streams using the active vertex format as a description.
        // @param[out] vertexShaderInput filled with vertex data from the streams.
        // @param[in] vertexIndex index of the vertex.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult DecodeVertexStream(Yw3dVSInput& vertexShaderInput, uint32_t vertexIndex);

        // Fetches a vertex from the current vertex streams and transforms it by calling the vertex shader.
        // This function also takes care of caching transformed vertices.
        // @param[in,out] vertexCacheEntry receives a pointer to the cache-entry holding the transformed vertex. (in-parameter, because a check is performed to see if the pointer already points to the desired vertex)
        // @param[in] vertexIndex index of the vertex.
        Yw3dResult FetchVertex(Yw3dVertexCacheEntry** vertexCacheEntry, uint32_t vertexIndex);

        // Begins the processing-pipeline that works on a per-triangle base. Either continues to the clipping-stage or takes care of subdivision.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void ProcessTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Interpolates between two vertex shader inputs (used for subdivision).
        // @param[out] vsInput output.
        // @param[in] vsInputA input A.
        // @param[in] vsInputB input B.
        // @param[in] interpolation interpolation value e [0,1].
        void InterpolateVertexShaderInput(Yw3dVSInput* vsInput, const Yw3dVSInput* vsInputA, const Yw3dVSInput* vsInputB, float interpolation);

        // Interpolates between two vertex shader outputs.
        // @param[out] vsOutput output.
        // @param[in] vsOutputA input A.
        // @param[in] vsOutputB input B.
        // @param[in] interpolation interpolation value e [0;1].
        void InterpolateVertexShaderOutput(Yw3dVSOutput* vsOutput, const Yw3dVSOutput* vsOutputA, const Yw3dVSOutput* vsOutputB, float interpolation);

        // Multiples a vertex shader output's registers by a floating point value.
        // @param[out] dest vertex shader output destination.
        // @param[in] src vertex shader output source.
        // @param[in] value floating point value to multiply registers with.
        void MultiplyVertexShaderOutputRegisters(Yw3dVSOutput* dest, const Yw3dVSOutput* src, float value);

        // Projects a vertex and prepares it for interpolation during rasterization.
        // @param[in,out] vsOutput the vertex.
        void ProjectVertex(Yw3dVSOutput* vsOutput);

        // Calculates gradients for shader registers.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void CalculateTriangleGradients(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Sets shader registers from triangle gradients.
        // @param[in,out] vsOutput vertex shader output.
        // @param[in] x screen space x-coordinate.
        // @param[in] y screen space y-coordinate.
        void SetVSOutputFromGradient(Yw3dVSOutput* vsOutput, float x, float y);

        // Updates shader registers from triangle gradients performing a step to the next pixel in the current scanline.
        // @param[in,out] vsOutput vertex shader output.
        void StepXVSOutputFromGradient(Yw3dVSOutput* vsOutput);

        // Rasterizes a single triangle: Performs triangle setup and does scanline-conversion.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void RasterizeTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Rasterizes a line.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        void RasterizeLine(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1);

        // Rasterizes a scanline span on screen. Writes the pixel color, which is outputted by the pixel shader, to the colorbuffer; writes the pixel depth, which has been interpolated from the base triangle's vertices to the depth buffer. Does not support pixel-killing.
        // @param[in] y position in rendertarget along y-axis.
        // @param[in] x1 left position in rendertarget along x-axis.
        // @param[in] x2 right position in rendertarget along x-axis.
        // @param[in,out] vsOutput interpolated vertex data.
        void RasterizeScanline_ColorOnly(uint32_t y, uint32_t x1, uint32_t x2, Yw3dVSOutput* vsOutput);

        // Rasterizes a scanline span on screen. Writes the pixel color, which is outputted by the pixel shader, to the colorbuffer; writes the pixel depth, which has been interpolated from the base triangle's vertices to the depth buffer.
        // @param[in] y position in rendertarget along y-axis.
        // @param[in] x1 left position in rendertarget along x-axis.
        // @param[in] x2 right position in rendertarget along x-axis.
        // @param[in,out] io_pVSOutput interpolated vertex data.
        void RasterizeScanline_ColorOnly_MightKillPixels(uint32_t y, uint32_t x1, uint32_t x2, Yw3dVSOutput* vsOutput);

        // Rasterizes a scanline span on screen. Writes the pixel color, which is outputted by the pixel shader, to the colorbuffer; writes the pixel depth, which has been computed by the pixel shader to the depth buffer.
        // @note Early depth-testing is disabled, which may lead to worse performance because regardless of the depth value the pixel shader will always be called for a given pixel.
        // @param[in] y position in rendertarget along y-axis.
        // @param[in] x1 left position in rendertarget along x-axis.
        // @param[in] x2 right position in rendertarget along x-axis.
        // @param[in,out] io_pVSOutput interpolated vertex data.
        void RasterizeScanline_ColorDepth(uint32_t y, uint32_t x1, uint32_t x2, Yw3dVSOutput* vsOutput);

    private:
        // ------------------------------------------------------------------

        // The vertex format.
        class Yw3dVertexFormat* m_VertexFormat;

        // The primitive assembler; used for DrawDynamicPrimitive().
        class IYw3dPrimitiveAssembler* m_PrimitiveAssembler;

        // The vertex shader.
        class IYw3dVertexShader* m_VertexShader;

        // The triangle shader (optional).
        class IYw3dTriangleShader* m_TriangleShader;
        
        // The pixel shader.
        class IYw3dPixelShader* m_PixelShader;

        // The index buffer.
        class Yw3dIndexBuffer* m_IndexBuffer;

        // ------------------------------------------------------------------

        // @internal Describes a vertex stream.
        // @note This structure is used internally by devices.
        struct VertexStream
        {
            // Pointer to the vertex buffer.
            class Yw3dVertexBuffer* vertexBuffer;

            // Offset from the beginning of the vertex buffer in bytes.
            uint32_t offset;

            //< Stride in bytes.
            uint32_t stride;

            VertexStream() : vertexBuffer(nullptr), offset(0), stride(0) {}
        };

        // The vertex streams;
        VertexStream m_VertexStreams[YW3D_MAX_VERTEX_STREAMS];

        // ------------------------------------------------------------------

        // The render state.
        uint32_t m_RenderStates[Yw3d_RS_NumRenderStates];

        // The render target.
        class Yw3dRenderTarget* m_RenderTarget;

        // Render information struct of this device.
        struct RenderInfo
        {
            // ------------------------------------------------------------------
            // Shader info.

            // Holds information about the type of a particular input-register.
            Yw3dShaderRegisterType vsInputRegisterTypes[YW3D_VERTEX_SHADER_REGISTERS];

            // Type of vertex shader output-registers.
            Yw3dShaderRegisterType vsOutputRegisterTypes[YW3D_PIXEL_SHADER_REGISTERS];

            // ------------------------------------------------------------------
            // Frame and color info.

            // Holds a pointer to the colorbuffer data.
            float* frameData;

            // Number of floats in colorbuffer, e.g. 2 for a vector2-texture.
            uint32_t colorFloats;

            // Colorbuffer width * number of floats; pitch in multiples of sizeof(float).
            uint32_t colorBufferPitch;

            // True if writing to the colorbuffer has been enabled + if a colorbuffer is available.
            bool colorWriteEnabled;

            // ------------------------------------------------------------------
            // Depth info.

            // Holds a pointer to the depthbuffer data.
            float* depthData;

            // Depthbuffer width * 1 (depthbuffers may only contain a single float); pitch in multiples of sizeof(float).
            uint32_t depthBufferPitch;

            // Depth compare-function. If no depthbuffer is available this is m3dcmp_always.
            Yw3dCompareFunction depthCompare;

            // True if writing to the depthbuffer has been enabled + if a depthbuffer is available.
            bool depthWriteEnabled;

            // ------------------------------------------------------------------
            // Rasterize info.

            // Rasterization-function for scanlines (triangle-drawing).
            void (Yw3dDevice::*RasterizeScanline)(uint32_t, uint32_t, uint32_t, Yw3dVSOutput*);

            // Drawing-function for individual pixels.
            void (Yw3dDevice::*DrawPixel)(uint32_t, uint32_t, const Yw3dVSOutput*);

            // Counts the number of pixels that pass the depth-test.
            uint32_t renderedPixels;

            // Active viewport rectangle.
            Yw3dRect viewportRect;

            // ------------------------------------------------------------------
            // Clip info.

            // Planes used for clipping, frustum planes are initialized at device creation time.
            Plane clippingPlanes[Yw3d_CP_NumPlanes];

            // Signals if a particular clipping plane is enabled.
            bool clippingPlaneEnabled[Yw3d_CP_NumPlanes];

            // Scissor planes used for clipping created from m_ScissorRect;
            Plane scissorPlanes[4];

            RenderInfo() : 
                frameData(nullptr), colorFloats(4), colorBufferPitch(0), colorWriteEnabled(true), 
                depthData(nullptr), depthBufferPitch(0), depthCompare(Yw3d_CMP_Less), depthWriteEnabled(true), 
                RasterizeScanline(nullptr), DrawPixel(nullptr), renderedPixels(0)
            {
                // Init clip plans.
                for (uint32_t i = 0; i < Yw3d_CP_NumPlanes; i++)
                {
                    clippingPlaneEnabled[i] = true;
                }
            }
        };

        // Current render information of this device.
        RenderInfo m_RenderInfo;

        // Contains gradient information that serves as the base for scanline-conversion.
        Yw3dTriangleInfo m_TriangleInfo;

        // ------------------------------------------------------------------

        // Number of valid vertex cache entries - reset before each draw-call.
        uint32_t m_NumValidCacheEntries;

        // Amount of fetched vertices - reset before each draw-call.
        uint32_t m_FetchedVertices;

        // Vertex cache contents.        
        Yw3dVertexCacheEntry m_VertexCache[YW3D_VERTEX_CACHE_SIZE];
    };
}

#endif // !__YW_3D_DEVICE_H__
