// Add by Yaukey at 2018-01-31.
// YW Soft Renderer vertex format class.

#ifndef __YW_3D_VERTEX_FORMAT_H__
#define __YW_3D_VERTEX_FORMAT_H__

#include "../Yw3dBase.h"
#include "../Yw3dTypes.h"

namespace yw
{
    // A vertex format describes the data-layout of vertex streams.
    class Yw3dVertexFormat : IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a vertex format.
        // @param[in] device a pointer to the parent Yw3dDevice-object.
        Yw3dVertexFormat(class Yw3dDevice* device);

        // Accessible by IBase. The destructor is called when the reference count reaches zero.
        ~Yw3dVertexFormat();

    protected:
        // Accessible by Yw3dDevice which is the only class that may create a vertex format.
        // @param[in] vertexDeclaration pointer to the vertex format declaration.
        // @param[in] vertexDeclSize length of the vertex format declaration in bytes.
        // @return Yw3d_S_OK if the function succeeds.
        // @return Yw3d_E_InvalidParameters if one or more parameters were invalid.
        // @return Yw3d_E_OutOfMemory if memory allocation failed.
        Yw3dResult Create(const Yw3dVertexElement* vertexDeclaration, const uint32_t vertexDeclSize);

    public:
        // Returns a pointer to the associated device. Calling this function will increase the internal reference count of the device. Failure to call Release() when finished using the pointer will Yw3dResult in a memory leak.
        class Yw3dDevice* AcquireDevice();

    protected:
        // Accessible by Yw3dDevice. Returns the number of vertex elements described in this vertex format.
        uint32_t GetNumVertexElements() const;

        // Accessible by Yw3dDevice. Returns the highest index of the used vertex streams.
        uint32_t GetHighestStream() const;

        // Accessible by Yw3dDevice. Returns a pointer to the vertex elements description.
        Yw3dVertexElement* GetElements() const;

    private:
        // Pointer to device.
        class Yw3dDevice* m_Device;

        // Number of vertex elements.
        uint32_t m_NumVertexElements;

        // Highest index of the used vertex streams.
        uint32_t m_HighestStream;

        // Pointer to the vertex elements.
        Yw3dVertexElement* m_Elements;
    };
}

#endif // !__YW_3D_VERTEX_FORMAT_H__
