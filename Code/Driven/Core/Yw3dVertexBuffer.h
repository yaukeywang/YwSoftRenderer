// Add by Yaukey at 2018-01-31.
// YW Soft Renderer vertex buffer class.

#ifndef __YW_3D_VERTEX_BUFFER_H__
#define __YW_3D_VERTEX_BUFFER_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // Vertex buffers are memory buffers that contain vertex-data, which is described by a vertex format.
    class Yw3dVertexBuffer : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a vertex buffer.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dVertexBuffer(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dVertexBuffer();

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a vertex buffer.
        // @param[in] length length of the vertex buffer to be created in bytes.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        Yw3dResult Create(const uint32_t length);

    public:
        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. 
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dDevice* AquireDevice();

        // Returns a pointer to the desired position in the buffer.
        // @param[in] offset has to be specified in bytes.
        // @param[out] pointer receives the pointer to the vertex buffer.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult GetPointer(const uint32_t offset, void** pointer) const;

        // Returns the length of the buffer in bytes.
        uint32_t GetLength() const;

    private:
        // Pointer to device.
        class Yw3dDevice* m_Device;

        // Length of the vertex buffer in bytes.
        uint32_t m_Length;

        // Pointer to the vertex buffer data.
        uint8_t* m_Data;
    };
}

#endif // !__YW_3D_VERTEX_BUFFER_H__
