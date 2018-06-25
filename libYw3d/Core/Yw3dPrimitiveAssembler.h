// Add by Yaukey at 2018-01-31.
// YW Soft Renderer primitive assembler class.

#ifndef __YW_3D_PRIMITIVE_ASSEMBLER_H__
#define __YW_3D_PRIMITIVE_ASSEMBLER_H__

#include "Yw3dBase.h"
#include "Yw3dTypes.h"

namespace yw
{
    // Defines the triangle assembler interface.
    class IYw3dPrimitiveAssembler : IBase
    {
        friend class Yw3dDevice;

    protected:
        // Accessible by Yw3dDevice.
        // This is the core function of a primitive assembler: It is used for DrawDynamicPrimitive() and executed after all vertices have been transformed. A primitive assembler returns indices to form primitives, which are in turn processed and rendered.
        // @param[out] vertexIndices output vector which receives three indices.
        // @param[in] numVertices number of vertices.
        // @return type of assembled primitives: member of the enumeration Yw3dPrimitiveType.
        virtual Yw3dPrimitiveType Execute(std::vector<uint32_t>& vertexIndices, uint32_t numVertices) = 0;
    };
}

#endif // !__YW_3D_PRIMITIVE_ASSEMBLER_H__
