// Add by Yaukey at 2018-01-31.
// YW Soft Renderer index buffer class.

#ifndef __YW_3D_INDEX_BUFFER_H__
#define __YW_3D_INDEX_BUFFER_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // Index buffers contain a list of vertex indices either in 16-bit or 32-bit format.
    class Yw3dIndexBuffer : public IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create an index buffer.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dIndexBuffer(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dIndexBuffer();

    protected:
        // Accessible by Yw3dDevice which is the only class that may create an index buffer.
        // @param[in] length length of the index buffer to be created in bytes.
        // @param[in] format format of the index buffer to be created. Member of the enumeration Yw3dFormat; either Yw3d_FMT_INDEX16 or Yw3d_FMT_INDEX32.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        // @return Yw3d_E_InvalidFormat if an invalid format was encountered.
        Yw3dResult Create(uint32_t length, Yw3dFormat format);

    public:
        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. 
        // Failure to call Release() when finished using the pointer will result in a memory leak.
        class Yw3dDevice* AquireDevice();

        // Returns a pointer to the desired position in the buffer.
        // @param[in] offset has to be specified in bytes.
        // @param[out] pointer receives the pointer to the index buffer.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult GetPointer(const uint32_t offset, void** pointer) const;

        // Returns the length of the buffer in bytes.
        uint32_t GetLength() const;

        // Returns the format of the buffer. Member of the enumeration Yw3dFormat; either Yw3d_FMT_INDEX16 or Yw3d_FMT_INDEX32.
        Yw3dFormat GetFormat() const;

    protected:
        // Accessible by Yw3dDevice: This function returns the index-value at a given index in the buffer, that means an index of 3 returns the third stored vertex-index in the buffer regardless of its format.
        // @param[in] arrayIndex index of the value in the ib-array.
        // @param[out] value receives the value of the index to be looked up.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        Yw3dResult GetVertexIndex(uint32_t arrayIndex, uint32_t& value) const;

    private:
        // Pointer to device.
        class Yw3dDevice* m_Device;

        // Length of the vertex buffer in bytes.
        uint32_t m_Length;

        // Format of the index buffer. Member of the enumeration Yw3dFormat; either Yw3d_FMT_INDEX16 or Yw3d_FMT_INDEX32.
        Yw3dFormat m_Format;

        // Pointer to the vertex buffer data.
        uint8_t* m_Data;
    };
}

#endif // !__YW_3D_INDEX_BUFFER_H__
