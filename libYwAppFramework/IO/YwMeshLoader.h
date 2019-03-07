// Add by Yaukey at 2019-03-01.
// YW mesh loader base class.

#ifndef __YW_MESH_LOADER_H__
#define __YW_MESH_LOADER_H__

#include "YwBase.h"
#include "YwMath.h"

namespace yw
{
    // Base mesh loader class.
    class IMeshLoader
    {
    public:
        // Constructor.
        IMeshLoader() {}
        
        // Destructor.
        virtual ~IMeshLoader() {}

    public:
        // Load mesh data from a file, classes derived from this need to implement their own.
        // @param[in] fileName the full path of the mesh file.
        // @return Mesh pointer of the loaded mesh, null if failed.
        virtual class Mesh* Load(const StringA& fileName) = 0;
    };
}

#endif //!__YW_MESH_LOADER_H__
