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
        /// Renders nonindexed primitives of the specified type from the currently set vertex streams.
        /// @param[in] primitiveType member of the enumeration Yw3dPrimitiveType, specifies the primitive's type.
        /// @param[in] startVertex Beginning at this vertex the correct number used for rendering this batch will be read from the vertex streams.
        /// @param[in] primitiveCount Amount of primitives to render.
        /// @return Yw3d_S_OK if the function succeeds.
        /// @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        /// @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult DrawPrimitive(Yw3dPrimitiveType primitiveType, uint32_t startVertex, uint32_t primitiveCount);

        /// Renders geometric primitives through indexing into an array of vertices.
        /// @param[in] primitiveType member of the enumeration Yw3dPrimitiveType, specifies the primitive's type.
        /// @param[in] baseVertexIndex added to each index before accessing a vertex from the array.
        /// @param[in] minIndex specifies the minimum index for vertices used during this batch.
        /// @param[in] numVertices specifies the number of vertices that will be used beginning from baseVertexIndex + minIndex.
        /// @param[in] startIndex Location in the index buffer to start reading from.
        /// @param[in] primitiveCount Amount of primitives to render.
        /// @return Yw3d_S_OK if the function succeeds.
        /// @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        /// @return Yw3d_E_InvalidState if an invalid state was encountered.
        Yw3dResult DrawIndexedPrimitive(Yw3dPrimitiveType primitiveType, int32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount);

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
        result DecodeVertexStream(VSInput& vertexShaderInput, uint32_t vertexIndex);

        // Fetches a vertex from the current vertex streams and transforms it by calling the vertex shader.
        // This function also takes care of caching transformed vertices.
        // @param[in,out] vertexCacheEntry receives a pointer to the cache-entry holding the transformed vertex. (in-parameter, because a check is performed to see if the pointer already points to the desired vertex)
        // @param[in] vertexIndex index of the vertex.
        result FetchVertex(VertexCacheEntry** vertexCacheEntry, uint32_t vertexIndex);

        // Begins the processing-pipeline that works on a per-triangle base. Either continues to the clipping-stage or takes care of subdivision.
        // @param[in] vsOutput0 vertex A.
        // @param[in] vsOutput1 vertex B.
        // @param[in] vsOutput2 vertex C.
        void ProcessTriangle(const VSOutput* vsOutput0, const VSOutput* vsOutput1, const VSOutput* vsOutput2);
    };
}

#endif // !__YW_3D_DEVICE_H__
