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
        // ------------------------------------------------------------------
        // Drawing.

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

        // Renders primitives assembled through the triangle assembler from the currently set vertex streams.
        // @param[in] startVertex Beginning at this vertex the correct number used for rendering this batch will be read from the vertex streams.
        // @param[in] numVertices specifies the number of vertices that will be used beginning from startVertex.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult DrawDynamicPrimitive(uint32_t startVertex, uint32_t numVertices);

        // ------------------------------------------------------------------
        // Resource creation.

        // ------------------------------------------------------------------
        // State management.

        // Sets a renderstate.
        // @param[in] renderState member of the enumeration Yw3dRenderState.
        // @param[in] value value to be set. Some renderstates require floating point values, which can be set through casting to a uint32-pointer and dereferencing it.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult SetRenderState(Yw3dRenderState renderState, uint32_t value);

        // Retrieves the value of a renderstate.
        // @param[in] renderState member of the enumeration Yw3dRenderState.
        // @param[out] value receives the value of the renderstate.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult GetRenderState(Yw3dRenderState renderState, uint32_t& value);

        // Sets a sampler state.
        // @param[in] samplerNumber number of the sampler.
        // @param[in] textureSamplerState sampler state. Member of the enumeration m3dtexturesamplerstate.
        // @param[in] state value to be set. Some sampler states require floating point values, which can be set through casting to a uint32-pointer and dereferencing it.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult SetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t state);

        // Retrieves a sampler state.
        // @param[in] samplerNumber number of the sampler.
        // @param[in] textureSamplerState sampler state. Member of the enumeration m3dtexturesamplerstate.
        // @param[out] state receives the value of the sampler state.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult GetTextureSamplerState(uint32_t samplerNumber, Yw3dTextureSamplerState textureSamplerState, uint32_t& state);

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

        // Sets the render target.
        // @param[in] renderTarget pointer to the render target.
        void SetRenderTarget(class Yw3dRenderTarget* renderTarget);

        // Returns a pointer to the active render target. Calling this function will increase the internal reference count of the render target.
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dRenderTarget* GetRenderTarget();

        // Sets the scissor rect.
        // @param[in] scissorRect the scissor rect.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if the scissor rectangle is invalid.
        Yw3dResult SetScissorRect(const Yw3dRect& scissorRect);

        // Returns the currently set scissor rect.
        Yw3dRect GetScissorRect();

        // Sets the bounding values for depth.
        // @param[in] minZ minimum allowed depth value, e [0,1], default: 0.
        // @param[in] maxZ maximum allowed depth value, e [0,1], default: 1.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if the bound values are invalid.
        Yw3dResult SetDepthBounds(float minZ, float maxZ);

        // Returns the currently set depth bounding values.
        void GetDepthBounds(float& minZ, float& maxZ);

    private:
        // Initializes renderstates to default values.
        void SetDefaultRenderStates();

        // Initializes samplerstates to default values.
        void SetDefaultTextureSamplerStates();

        // Initializes the frustum clipping planes.
        void SetDefaultClippingPlanes();

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

        // Performs simple-subdivision.
        // @param[in] subdivisionLevel number of times to subdivide.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void SubdivideTriangle_Simple(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Performs smooth-subdivision.
        // @param[in] subdivisionLevel number of times to subdivide.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void SubdivideTriangle_Smooth(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Performs adaptive-subdivision: Finds the triangle's center vertex and initiates splitting of triangle edges.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void SubdivideTriangle_Adaptive(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Helper function for adaptive-subdivision: Recursively splits triangle edges.
        // @param[in] subdivisionLevel number of times to subdivide.
        // @param[in] vsOutputEdge0 first output edge.
        // @param[in] vsOutputEdge1 second output edge.
        // @param[in] vsOutputCenter the output center.
        void SubdivideTriangle_Adaptive_SubdivideEdges(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutputEdge0, const Yw3dVSOutput* vsOutputEdge1, const Yw3dVSOutput* vsOutputCenter);

        // Helper function for adaptive-subdivision: Recursively subdivides triangles until their screen-area falls below a user-defined threshold.
        // @param[in] i_iSubdivisionLevel number of times to subdivide.
        // @param[in] i_pVSOutput0 vertex A.
        // @param[in] i_pVSOutput1 vertex B.
        // @param[in] i_pVSOutput2 vertex C.
        void SubdivideTriangle_Adaptive_SubdivideInnerPart(uint32_t subdivisionLevel, const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // DrawTriangle() takes care of backface culling, triangle clipping, vertex projection and begins rasterization.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void DrawTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

        // Clippes a polygon to the specified clipping plane.
        // @param[in] numVertices number of vertices of the polygon to clip.
        // @param[in] stage stage in the clipping pipeline.
        // @param[in] plane clipping plane.
        // @param[in] homogenous if true the w-coordinate of vertex position is taken into account.
        // @return number of vertices of the clipped polygon.
        uint32_t ClipToPlane(uint32_t numVertices, uint32_t stage, const Plane &plane, bool homogenous);

        // Performs back face culling in screen space.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        bool CullTriangle(const Yw3dVSOutput* vsOutput0, const Yw3dVSOutput* vsOutput1, const Yw3dVSOutput* vsOutput2);

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

        // Draws a single pixels. Writes the pixel color, which is outputted by the pixel shader, to the colorbuffer; writes the pixel depth, which has been interpolated from the vertices to the depth buffer. Does not support pixel-killing.
        // @param[in] x position in rendertarget along x-axis.
        // @param[in] y position in rendertarget along y-axis.
        // @param[in] vsOutput interpolated vertex data, already divided by position w component.
        void DrawPixel_ColorOnly(uint32_t x, uint32_t y, const Yw3dVSOutput* vsOutput);

        // Rasterizes a scanline span on screen. Writes the pixel color, which is outputted by the pixel shader, to the colorbuffer; writes the pixel depth, which has been computed by the pixel shader to the depth buffer.
        // @note Early depth-testing is disabled, which may lead to worse performance because regardless of the depth value the pixel shader will always be called for a given pixel.
        // @param[in] x position in rendertarget along x-axis.
        // @param[in] y position in rendertarget along y-axis.
        // @param[in] vsOutput interpolated vertex data, already divided by position w component.
        void DrawPixel_ColorDepth(uint32_t x, uint32_t y, const Yw3dVSOutput* vsOutput);

    private:
        // ------------------------------------------------------------------

        // Pointer to parent.
        class Yw3d* m_Parent;

        // Device parameters, initialize at device-creation time.
        Yw3dDeviceParameters m_DeviceParameters;

        // Base for rendering to screen.
        class Yw3dPresentTarget* m_PresentTarget;

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

        // The active scissor rect.
        Yw3dRect m_ScissorRect;

        // Internal Describes a texture sampler.
        // Note: This structure is used internally by devices.
        struct TextureSampler
        {
            // Pointer to the texture.
            class IYw3dBaseTexture* texture;

            // The samplers states.
            uint32_t textureSamplerStates[Yw3d_TSS_NumTextureSamplerStates];

            // Type of texture coordinates for sampling.
            Yw3dTextureSampleInput textureSampleInput;

            TextureSampler() : 
                texture(nullptr),
                textureSampleInput(Yw3d_TSI_2Coords)
            {
                memset(textureSamplerStates, 0, sizeof(uint32_t) * Yw3d_TSS_NumTextureSamplerStates);
            }
        };

        // The texture samplers.
        TextureSampler m_TextureSamplers[YW3D_MAX_TEXTURE_SAMPLERS];

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

        // ------------------------------------------------------------------

        // Storage for vertices, that are created during clipping.
        Yw3dVSOutput m_ClipVertices[YW3D_CLIP_VERTEX_CACHE_SIZE];

        // Keeps the next index of m_ClipVertices that can be used for the creation of vertices during clipping.
        uint32_t m_NextFreeClipVertex;

        // Pointers to polygon vertices, two stages: ping-pong during clipping.
        Yw3dVSOutput* m_ClipVerticesStages[2][YW3D_CLIP_VERTEX_CACHE_SIZE];
    };
}

#endif // !__YW_3D_DEVICE_H__
